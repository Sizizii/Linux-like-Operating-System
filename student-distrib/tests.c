#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "filesystem.h"
#include "keyboard_handler.h"
#include "rtc.h"
#include "terminal.h"
#include "system_call.h"
#include "pcb.h"

#define PASS 1
#define FAIL 0

#define KEYBOARD_VEC_NUM 		0x21
#define RTC_VEC_NUM 			0x28
#define KERNEL_MEM_START		0x400000
#define KERNEL_MEM_END			0x7FFFFF
#define VIDEO_MEM_START			0xB8000
#define VIDEO_MEM_END			0xB8FFF

/* format these macros as you see fit */
#define TEST_HEADER 	\
    printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
    printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
    /* Use exception #15 for assertions, otherwise
       reserved by Intel */
    asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
    TEST_HEADER;

    int i;
    int result = PASS;
    for (i = 0; i < 20; ++i){
        if ((idt[i].offset_15_00 == NULL) && 
            (idt[i].offset_31_16 == NULL)){
            assertion_failure();
            result = FAIL;
        }
    }

    if (((idt[KEYBOARD_VEC_NUM].offset_15_00 == NULL) && (idt[KEYBOARD_VEC_NUM].offset_31_16 == NULL)) || (idt[KEYBOARD_VEC_NUM].present == 0)){
            assertion_failure();
            result = FAIL;
        }

    if (((idt[RTC_VEC_NUM].offset_15_00 == NULL) && (idt[RTC_VEC_NUM].offset_31_16 == NULL)) || (idt[KEYBOARD_VEC_NUM].present == 0)){
            assertion_failure();
            result = FAIL;
        }

    return result;
}

/* div_zero_test: cause Divide-by-zero exception
 *
 * Inputs: None
 * Outputs: None
 * Side Effects: call the exception handler 0, and halt the system
 * Coverage: exception handler
 * Files: idt.h/c
 */
int div_zero_test(){
    TEST_HEADER;
    int a,b,c;
    a = 1;
    b = 0;
    c = a / b;
    // if success, will never return FAIL and will halt the system
    return FAIL;
}

/* paging_table_directory_test
 *
 * Check values in Paging
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: page table and page directory
 * Files: paging.h/c, paging_asm.S
 */
int paging_table_directory_test(){
	TEST_HEADER;
	int result = PASS;
	if (page_directory[0] != (((uint32_t)page_table) | READ | PRESENT | ACCESS)) {
		printf("table");
		result = FAIL;
	}
	if (page_table[VIDEO>>12] != (page_table[VIDEO>>12] | READ | PRESENT)) {
		printf("video");
		result = FAIL;
	}
	if (page_directory[1] != (FOURM | READ_P | MSIZE | ACCESS | DIRTY)) {
		printf("kernel");
		result = FAIL;
	}
	return result;
}


/* paging_out_of_range_test
* Checks whether the range is correct
* Inputs: None
* Outputs: None
* Side Effects: will cause out of range exception
* Coverage: paging memory range
* Files: paging.h/c, paging_asm.S
*/
int paging_out_of_range_test() {
    TEST_HEADER;
    char result;
    char* pointer;
    pointer = (char*)0xB9000;  // out of range address
    result = *pointer;
    // will never reach here since it is out of range,
    // and will cause exception
    return PASS;
}

/* null_test
* Checks whether dereference NULL will cause exception
* Inputs: None
* Outputs: None
* Side Effects: will cause exception
* Coverage: exception
* Files: idt.h/c
*/
int null_test() {
    TEST_HEADER;
    char result;
    char* pointer;
    pointer = (char*)NULL;  // out of range address
    result = *pointer;
    // will never reach here since NULL cannot be dereference,
    // and will cause exception
    return PASS;
}

/* paging_range_test
* Checks the range of the paging is correct
* Inputs: None
* Outputs: None
* Side Effects: will halt the system if failed
* Coverage: paging tange
* Files: paging.c/h, paging_asm.S
*/
int paging_range_test() {
    TEST_HEADER;
    char result;
    char* pointer;
	pointer = (char*)KERNEL_MEM_START; // kernel memory starts
    result = *pointer;

    pointer = (char*)VIDEO_MEM_START; // video memory starts
    result = *pointer;

    pointer = (char*)KERNEL_MEM_END; // kernel memory ends
    result = *pointer;

    pointer = (char*)VIDEO_MEM_END; // video memory ends
    result = *pointer;

    return PASS;
}


/* Checkpoint 2 tests */


/* boot_block_test
* Check if the boot block address is correct
* Inputs: None
* Outputs: number of directory entries
* Side Effects: none
* Coverage: boot block address
* Files: filesystem.c/h
*/
int boot_block_test() {
	TEST_HEADER;
    printf("number of dentry: %d\n", boot_block->num_dentry);
	return PASS;
}

/* read_file_test
* print the file content
* Inputs: None
* Outputs: file content printed to the screen
* Side Effects: none
* Coverage: file open, read, close
* Files: filesystem.c/h
*/
int read_file_test(uint8_t* filename) {
	TEST_HEADER;
    int succ = file_open(filename);
    if (succ == -1) {
        printf("fail to open the file.\n");
		return FAIL;
    }
	// a large enough buffer
    int32_t n = 10240;
    uint8_t buf[10240];
    int32_t read_length = file_read(1, buf, n);
	file_close(1);
	// print to the screen
    int32_t written_nbytes = terminal_write(1, buf, read_length);
	if (written_nbytes == read_length) {
		uint8_t buf[1];
		buf[0] = '\n';
		terminal_write(1, buf, 1);
		return PASS;
	} else {
		uint8_t buf[1];
		buf[0] = '\n';
		terminal_write(1, buf, 1);
		printf("terminal write incomplete\n");
		return FAIL;
	}
}

/* dir_check
* call the dir_read to show the ls 
* Inputs: None
* Outputs: None
* Side Effects: will print out the content in the directory
* Coverage: file system
* Files: file system.c/h
*/
int dir_check() {
	TEST_HEADER;
	int fd = 0;
	uint8_t buf[33];
	while (0 != dir_read(fd, buf, MAXIMUM_FILENAME_LENGTH)) {
		int n = strlen((int8_t*)buf);
		buf[n++] = '\n';
		terminal_write(1, buf, n);
	}
	return PASS;
}


/* terminal_test1
* Check buffer size equal to 128
* Inputs: None
* Outputs: None
* Side Effects: none
* Coverage: terminal open, read, write, close
* Files: terminal.c/h
*/
int terminal_test1(){
	TEST_HEADER;
    terminal_open(0);
	uint8_t buf[128];
	printf("Please try to enter >= 128 chars.\n");
	int32_t n = terminal_read(0, (void*)buf, 128);
	terminal_write(0, (void*)buf, n);
	printf("\nnumber of read chars: %d (including \\n)\n", n);
	terminal_close(0);
	return PASS;
}

/* terminal_test2
* Check buffer size less than 128
* Inputs: None
* Outputs: None
* Side Effects: none
* Coverage: terminal open, read, write, close
* Files: terminal.c/h
*/
int terminal_test2(){
	TEST_HEADER;
    terminal_open(0);
	uint8_t buf[128];
	const int32_t SIZE = 10;
	printf("Please enter any number of chars.\n");
	printf("terminal_read is set to read a size of %d.\n", SIZE);
	int32_t n = terminal_read(0, (void*)buf, SIZE);
	terminal_write(0, (void*)buf, n);
	printf("\nnumber of read chars: %d (can including \\n)\n", n);
	terminal_close(0);
	return PASS;
}

/* rtc_test
* Checks rtc open/close/read/write and frequency changes
* Inputs: None
* Outputs: Tests and sequences of '1's to check if frequency changes successfully
* Side Effects: none
* Coverage: rtc open/close/read/write
* Files: rtc.c
*/
int rtc_test(){
	TEST_HEADER;
	uint32_t rtc_buf;
	int rtc_counter;

	/* Test invalid frequencies */
	rtc_buf = 5;
	printf("Test invalid frequency %d Hz that is not power of 2:\n", rtc_buf);
	rtc_open(0);
	if((rtc_write(0, &rtc_buf, 4)) == -1){	
		printf("TEST PASSED: Invalid frequency is rejected. \n");
		rtc_close(0);
	}
	else{	
		printf("TEST FAILED: Invalid frequency isn't rejected. \n"); 
		rtc_close(0);
	}

	int invalid_nbytes = 3;
	printf("Test invalid nbytes:\n");
	rtc_open(0);
	if((rtc_write(0, &rtc_buf, invalid_nbytes)) == -1){	
		printf("TEST PASSED: Invalid nbytes is rejected. \n");
		rtc_close(0);
	}
	else{	
		printf("TEST FAILED: Invalid nbytes isn't rejected. \n"); 
		rtc_close(0);
	}

	/* Test frequencies change */
	printf("Test frequency changes from 2 Hz to 1024 Hz. \n");
	for (rtc_buf = 2; rtc_buf <= 1024; rtc_buf*=2)
	{
		rtc_open(0);
		if((rtc_write(0, &rtc_buf, 4)) == -1){
			printf("Invalid frequency. \n");
			rtc_close(0);
			continue;
		}
		else{
			printf("Testing frequency %d Hz: \n", rtc_buf);
		}

		for (rtc_counter = 0; rtc_counter < (rtc_buf < 128 ? 16:512); rtc_counter++)
		{
			rtc_read(0, &rtc_buf, 4);
			uint8_t buf[1];
			buf[0] = '1';
			terminal_write(1, buf, 1);
			// putc('1');
		}
		uint8_t buf[1];
		buf[0] = '\n';
		terminal_write(1, buf, 1);
		rtc_close(0);
	}
	
	return PASS;
}


/* Checkpoint 3 tests */

/*
 * stdio_test: check the stdin and stdout functionality
 * call the open, read, write functions as well
 * 
 * Input:None
 * Output: the sentence of terminal 
 * Return value: 0 if success, -1 if fail
 * Side_effect: none
*/
int stdio_test() {
	TEST_HEADER;
	int stdin, stdout, succ;
	char buffer[256];
	stdin = open((uint8_t*)"stdin");
	stdout = open((uint8_t*)"stdout");
	if (stdin == -1 || stdout == -1)
		return FAIL;
	succ = read(stdin, buffer, 100);
	if (succ < 0)
		return FAIL;
	succ = write(stdout, buffer, succ);
	if (succ < 0)
		return FAIL;
	return PASS;
}
/*
 * systemcall_file_test: test whether can open a file
 * through system call
 * Input: filename
 * Output: the content of the file
 * Return value: 0 if success, -1 if fail
 * Side_effect: print the file content to the screen
*/
int systemcall_file_test(const uint8_t* filename) {
	TEST_HEADER;
	int stdout, fd;
	stdout = open((uint8_t*)"stdout");
	fd = open((uint8_t*)filename);
	// invalid filename
	if (fd < 0) {
		printf("%s does not exist.\n");
		return FAIL;
	}
	// the size of buffer is set to be large enough to contain the file
	char buffer[10240];
	int ret = read(fd, buffer, 10240);
	if (ret < 0)
		return FAIL;
	ret = write(stdout, buffer, ret);
	if (ret < 0)
		return FAIL;
	write(stdout, "\n", 1);
	close(fd);
	ret = read(fd, buffer, 10240);
	if (ret != -1)
		return FAIL;
	return PASS;
}

/*
 * execute_test: call execute to make a process switch
 * 
 * Input: none
 * Output: open the shell 
 * Return value: 0 if success, -1 if fail
 * Side_effect: none
 * 
*/
int execute_test() {
	TEST_HEADER;
	int ret = execute((uint8_t*)"shell");
	if (ret != 0) {
		printf("The return value is %d\n", ret);
		return FAIL;
	}
	return PASS;
}

/* Helper functions */

/* NEXT_TEST_SPLIT
* split the text with an interactive option
* Inputs: None
* Outputs: None
* Side Effects: none
*/
inline void NEXT_TEST_SPLIT() {
	uint8_t* next_one_sentence = (uint8_t*)"\n Press Enter for next test...\n";
	int next_one_sentence_n = strlen((int8_t*)next_one_sentence);
	terminal_write(1, next_one_sentence, next_one_sentence_n);
	terminal_read(0, (void*)keyboard_buffer, 128);
	clear_screen();
}

inline void CHECKPOINT3_INIT() {
	curr_pid = create_process();
	bind_new_pcb(curr_pid, curr_parent_pid);
}


/* Test suite entry point */
void launch_tests() {
	clear_screen();

	// checkpoint 1 test
    // TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("paging_table_directory_test", paging_table_directory_test());
    // TEST_OUTPUT("paging_range_test", paging_range_test());
    // TEST_OUTPUT("null_test", null_test());
    // TEST_OUTPUT("paging_out_of_range_test", paging_out_of_range_test());
    // TEST_OUTPUT("divide_by_zero_test", div_zero_test());

	// checkpoint 2 test
	// TEST_OUTPUT("boot_block_test", boot_block_test());
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("rtc_test", rtc_test());
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("dir_check", dir_check());
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("read_file_test1", read_file_test((uint8_t*)"frame0.txt"));
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("read_file_test2", read_file_test((uint8_t*)"frame1.txt"));
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("read_file_test3", read_file_test((uint8_t*)"cat"));
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("read_file_test4", read_file_test((uint8_t*)"verylargetextwithverylongname.txt"));
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("terminal_test2", terminal_test2());
	// while(1) {
	// 	NEXT_TEST_SPLIT();
	// 	TEST_OUTPUT("terminal_test1", terminal_test1());
	// }
	// printf("\nALL TESTS COMPLETED\n");

	// checkpoint 3 test
	// CHECKPOINT3_INIT();
	// TEST_OUTPUT("stdio_test", stdio_test());
	// TEST_OUTPUT("systemcall_file_test", systemcall_file_test((uint8_t*)"frame0.txt"));
	// TEST_OUTPUT("stdio_test", stdio_test());
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("systemcall_file_test", systemcall_file_test((uint8_t*)"frame0.txt"));
	// NEXT_TEST_SPLIT();
	// TEST_OUTPUT("execute_test", execute_test());
	while(1);
	//execute((uint8_t*)"shell");
}
