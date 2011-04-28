
;Very important memory locations. Memory above 70h is accessible in any bank
	cblock 0x70
	errorByte
	bankSelection
	interruptBankSave
	temp
	myStatus
	saveW
	programCounter 
	current_process ; memory address of the currently running process's PCB
	pclTemp
	accumulator
	exchange
	eepromSize
	instruction
	stackPtr
	stackTemp
	stackTemp2
	endc

;Process space
	cblock 0x20
wait_queue_head
wait_queue_tail
pcb_block: PROCESS_MEMORY_SPACE * MAX_PROCESS_COUNT
	endc

;Page tabe is in bank 1
      cblock 0xA0
page_table: PAGE_COUNT
page_memory: PAGE_SIZE * PAGE_COUNT
free_page_count 
      endc

