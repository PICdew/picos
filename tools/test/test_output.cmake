execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${FILE1} ${FILE2} RESULT_VARIABLE test_failed)

if(test_failed)
	message(SEND_ERROR "Files Differ!")
endif(test_failed)
