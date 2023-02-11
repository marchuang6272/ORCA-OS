#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystems.h"
#include "keyboard.h"
#include "rtc.h"
#include "system_call.h"

#define PASS 1
#define FAIL 0

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
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
#if 0
void dividebyZero() 
{
	int a = 10;
	int b = a/0;
	b = b;
}

void pageFault() 
{
	int *a = NULL;
	int b = *a;
	b = b;
}

void boundaryCheck() {
	int *a = (int *) 0xB8000;
	int b = *a;
	b = b;
	printf("Worked, no interrupt generated");
}

void videoMem() {
	int *a = (int *) 0xB9000;
	int b = *a;
	b = b;
}
#endif

#if 0
/* Checkpoint 2 tests */
void rtc_freq_test() {
	uint32_t i;
	uint32_t temp = 0;
	uint32_t buf[1];
	clear();
	set_pos(0,0);
	printf("Testing notPowerofTwo and NULL\n");
	buf[0] = 244;
	if (rtc_write(NULL, buf, NULL) == -1 && rtc_write(NULL, NULL, NULL) == -1) {
		printf("notPowerofTwo and NULL PASS\n");
	}
	rtc_open(NULL);
	for (i = 2; i <= 1024; i*=2) {
		buf[0] = i;
		temp += rtc_write(NULL, buf, NULL);
		if (temp == 0) {
			printf("Frequency %dHz working\n", i);
		}
		test_frequency_flag = 0;
	 	while (test_frequency_flag != 1){
			
		}
	}
	test_frequency_flag = -1;
	printf("rtc_close. Reset Frequenct to 2Hz\n");
	rtc_close(NULL);
	printf("TESTING rtc_read\n");
	while (1) {
		printf("rtc_read PASS\n");
		rtc_read(NULL, NULL, NULL);
	}
}

void terminal_test(uint32_t nbytes) {
	int i;
	char read_output[nbytes];
	
	while(1) {
		for (i=0;i<nbytes;i++) {
		read_output[i] = ' ';
		}
		terminal_open(NULL);
		terminal_read(0,read_output,nbytes);
		for (i=0;i<nbytes;i++) {
			if (read_output[i] == '\n') {
				shift_rows_up();
				putc(read_output[i]);
				extern int update_cursor(); // defined in lib.c
				update_cursor();
				break;
			} else {
				putc_line_runover(read_output[i]);
			}	
		}
		terminal_close(NULL);
	}
}

void read_files() 
{
	int results = file_open((uint8_t*)"counter");
	printf("counter: %d\n", results);
	results = file_open((uint8_t*)"counter"); // should fail since not closed
	printf("reopen counter: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"frame1.txt");
	printf("frame1.txt: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"pingpong");
	printf("pingpong: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"frame2.txt");
	printf("frame2.txt: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"shelle");
	printf("shelle: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"printtest");
	printf("printtest: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"verylargetextwithverylongname.txt");
	printf("verylargetextwithverylongname.txt: %d\n", results);
	file_close();

	results = file_open((uint8_t*)"verylargetextwithverylongname.tx");
	printf("verylargetextwithverylongname.tx: %d\n", results);
	file_close();

	results = file_write();
	printf("attempting to write: %d\n", results); // should be -1
}

void print_one_file() 
{
	int results = file_open((uint8_t*)"frame0.txt"); // Open frame0
	if(results == -1) // can't find
	{
		printf("Couldn't find the file!");
	}
	else
	{
		uint8_t buf[500];
		buf[499] = '\0';
		file_read(buf, 500);
		printf((int8_t*)buf);
		file_close(); // only one file can be opened right now, so just close it
	}
}

void file_read_offset_test() // reads one byte at a time from frame0.txt
{
	int results = file_open((uint8_t*)"frame0.txt"); // Open frame0
	if(results == -1) // can't find
	{
		printf("Couldn't find the file!");
	}
	else
	{
		int i;
		for(i = 0; i < 500; i++)
		{
			uint8_t buf[2];
			buf[1] = '\0';
			file_read(buf, 1);
			printf((int8_t*)buf);
		}
		file_close(); // only one file can be opened right now, so just close it
	}
}

void long_file_read_test() {
	int results = file_open((uint8_t*)"verylargetextwithverylongname.tx");
	if(results == -1) // can't find
	{
		printf("Couldn't find the file!");
	}
	else
	{
		uint8_t buf[7000];
		buf[6999] = '\0';
		file_read(buf, 7000); // Test with length longer than number of bytes (6144 < 7000)
		// Also tests reading multiple data blocks since each can only store 4k bytes
		printf((int8_t*)buf);
		file_close(); // only one file can be opened right now, so just close it
	}
	
}

void read_directory_test() 
{
	int numOfFiles = 17;
	uint32_t max_size = (numOfFiles * 33) + 1; // 32 chars per name, max of 63 dentries, add 1 for NULL
	uint8_t buf[max_size];
	buf[max_size-1] = '\0';
	int results = directory_read(buf);
	if(results == -1)
	{
		printf("Couldn't find the file");
	}
	else
	{
		// since names are zero-padded, can't just use a normal printf
		int i;
		int counter = 0;
		for(i = 0; i < numOfFiles; i++)
		{
			counter++;
			uint8_t* starting = (buf + i*33);
			printf((int8_t*)starting); // Note: if a file name is NOT NULL-terminated, 
			// we will continue until the next NULL which is likely at the end of the next file name
			printf(" "); // add space in-between file names
			if(counter % 1 == 0)
			{
				printf("\n");
			}
		}

	}
	
}

#endif 

/* Checkpoint 3 tests */
#define DO_CALL(name,number)       \
asm volatile ("                    \
.GLOBL " #name "                  ;\
" #name ":                        ;\
    PUSHL	%EBX              ;\
	MOVL	$" #number ",%EAX ;\
	MOVL	8(%ESP),%EBX      ;\
	MOVL	12(%ESP),%ECX     ;\
	MOVL	16(%ESP),%EDX     ;\
	INT	$0x80             ;\
	POPL	%EBX              ;\
	RET                        \
")

//extern int32_t __sys_write(int32_t fd, const void* buf, int32_t nbytes);

void fakeFunction() 
{
	DO_CALL(__sys_write, 4);
}

/*void test_write_function() {

}*/
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests()
{
	// Clear screen so test results can be printed cleanly
	clear();
	set_pos(0, 0);
	/** ============== FILE SYSTEM TESTS ============== **/
	// read_files(); // Shows file.open() and file.close() is working
	// print_one_file(); // Shows we are able to print a complete file
	// file_read_offset_test(); // Reads one byte of a time, showing position offset and file.read() is working.
	// long_file_read_test(); // Reads a long file (shows data blocks work)
	//read_directory_test(); // Shows directory.read() is working

	/** ============== RTC DRIVER TESTS ============== **/
	//rtc_freq_test();
	
	/** ============== TERMINAL DRIVER TESTS ============== **/
	//terminal_test(128);
	fakeFunction();
	//test_write_function();
}

