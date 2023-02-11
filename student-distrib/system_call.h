#ifndef SYSTEM_CALL_H_
#define SYSTEM_CALL_H_

#include "types.h"
#include "lib.h"

extern pcb_t* current_pcb;

void sys_calls_init(void); // initialize variables
// The halt system call terminates a process, returning the specified value to its parent process.
int32_t sys_halt (uint8_t status);
// The execute system call attempts to load and execute a new program, handing off the processor to the new program until it terminates.
int32_t sys_execute (const uint8_t* command);
// The read system call reads data from the keyboard, a file, device (RTC), or directory
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
// The write system call writes data to the terminal or to a device (RTC)
int32_t sys_write (int32_t fd, void* buf, int32_t nbytes);
// The open system call provides access to the file system.
int32_t sys_open (uint8_t* filename);
// The close system call closes the specified file descriptor and makes it available for return from later calls to open.
int32_t sys_close (int32_t fd);
// The getargs call reads the program’s command line arguments into a user-level buffer
int32_t sys_getargs (uint8_t* buf, int32_t nbytes);
// The vidmap call maps the text-mode video memory into user space at a pre-set virtual address.
int32_t sys_vidmap (uint8_t** screen_start);
int32_t sys_set_handler (int32_t signum, void* handler_address);
int32_t sys_sigreturn (void);


void start_first_shell();
void save_esp(uint32_t register_esp);
uint32_t restore_esp();
uint32_t check_process_num();
pcb_t* create_pcb(void);

uint32_t process_number;
#endif
/*** end of file ***/

