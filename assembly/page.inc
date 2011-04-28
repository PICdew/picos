;Using the first page address (page_head)
;Arguments: on stack, page index of form <BANK:INDEX>
;Returns: in w-reg, ram address of the corresponding page
;	bank byte. FSR will already have this address loaded too.
TRANSLATE_PAGE_ADDRESS_MAC macro process_location,temp,page_table,page_head,errorByte,POP_STACK,PEEK_STACK,ERROR_RETURN
TRANSLATE_PAGE_ADDRESS call PEEK_STACK
	andlw 0xf0 ; put page bank number in temp
	movwf count
	swapf count,F
	call PUSH_STACK
	movlw low page_table
	movwf FSR
	movlw 0xff ; so that count, and thus the page index argument, is zero-indexed
	movwf count ; use this to count the bank numbers belonging to the process
	banksel process_location
	decf FSR,F
FIND_BANK_LOOP incf FSR,F
	movlw low page_table + PAGE_COUNT
	xorwf FSR,W
	btfsc STATUS,Z
	goto TPA_NO_SUCH_ADDRESS
	movf process_location,W
	xorwf INDF,W
	btfsc STATUS,Z
	incf count,F
	call PEEK_STACK
	xorwf count,W
	btfss STATUS,Z
	goto FIND_BANK_LOOP
	call POP_STACK ; page bank number. <BANK:INDEX> address still on stack.
	movf count,W
	call PUSH_STACK
	movlw PAGE_SIZE
	call PUSH_STACK
	call MULTIPLY_BYTE
	movwf count
	call POP_STACK
	andlw 0xf
	addwf count,W
	movlw SUCCESS
	movwf errorByte
	movf FSR,W
	return
TPA_NO_SUCH_ADDRESS movlw NO_SUCH_PAGE_ADDRESS
	movwf errorByte
	goto ERROR_RETURN
	
;Multiples 2 byte sized twos compliment integers
;Arguments: on stack, X and Y twos-compliment ints
;Returns: in w-reg, (possibly left-truncated) product X*Y
MULTIPLY_BYTE clrf temp
MULTIPLY_BYTE_LOOP movf stackPtr,W
	movwf FSR
	movf INDF,W
	incf FSR,F
	btfsc INDF,0
	addwf temp
	bcf STATUS,C
	rrf INDF,F
	bcf STATUS,C
	decf FSR,F
	rlf INDF,F
	incf FSR
	movf INDF,F
	btfss STATUS,Z
	goto MULTIPLY_BYTE_LOOP
	call POP_STACK ; X
	call POP_STACK ; Y(or what's left of it :-/ )
	movf temp,W
	return
	endm

;Moves through the processes and removes pages whos owners
;equal the value initially in W
FREE_PAGES_MAC macro page_table_addr,free_page_count,count,temp
FREE_PAGES movwf temp
	movlw low page_table_addr
	movwf FSR
	movlw PAGE_COUNT
	movwf count
FREE_PAGES_LOOP movf temp,W ; move through page table. see if the owner is the value passed to W-reg
	xorwf INDF,W
	btfss STATUS,Z
	goto $+6
	clrf INDF
	bsf STATUS,RP0
	incf free_page_count,F
	bcf STATUS,RP0
	incf FSR,F
	decfsz count,F
	goto FREE_PAGES_LOOP
	return
    endm
