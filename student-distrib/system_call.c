#include "types.h"
#include "system_call.h"
#include "filesystems.h"
#include "paging.h"
#include "rtc.h"
#include "keyboard.h"
#include "x86_desc.h"
#include "scheduling.h"

#define FILE_IMAGE 0x08048000
#define MAX_COMMAND_LEN 32

pcb_t* current_pcb; // current process control block

/* The init_sys_calls helper function initializes the process_number and current_pcb for first execute
 * INPUTS: none
 * OUTPUTS: none
 */
void sys_calls_init(void)
{
    process_number = 0; // start at 0
    current_pcb = NULL;
}

/* The halt system call terminates a process, returning the specified value to its parent process.
 * INPUTS: uint8_t status - the status
 * OUTPUTS: 0 if success
 */
int32_t sys_halt(uint8_t status) 
{
    if(process_number <= 2) // do not allow first shells to close
    {
        return -1;
    }

    /* Close relevant FD's */
    int i;
    for(i = 0; i < FILE_ARRAY_LEN; i++) 
    {
        // Close any open files (check if it was initialized with a pointer)
        if(current_pcb->file_array[i].flags == 1 && current_pcb->file_array[i].close != NULL)
        {
            current_pcb->file_array[i].close(i);
        }
        current_pcb->file_array[i].flags = 0;
    }
    /* Restore parent data */
    // OLD: process_number--;
    
    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(current_pcb->parent_terminal+1));
    // Get new process number -> check terminal's active processes or use terminal's process
    // terminal_counter stores current terminal #


    for(i = 3; i >= 0; i--) // start from highest priority
    {
        if(temp_pcb->active_processes[i] != 0) 
        {
            process_number = temp_pcb->active_processes[i];
            temp_pcb->active_processes[i] = 0;
            temp_pcb->terminal_displays[i] = 0;
            break;
        }
    } 
    if(i == -1) // no active processes; use base terminal
    {
        process_number = 3; // terminal 0, 1, 2
    }

    current_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(process_number));
    tss.esp0 = (uint32_t)(EIGHT_MB - (EIGHT_KB*current_pcb->process_num));
    tss.ss0 = KERNEL_DS;
    /* Restore parent paging */
    system_call_paging(current_pcb);
    return 0;
}

// read do nothing
int32_t no_read(int32_t fd, void* buf, int32_t nbytes)
{
    return -1;
}

// write do noting
int32_t no_write(int32_t fd, void* buf, int32_t nbytes)
{
    return -1;
}

/* The ge_pcb helper function return the current pcb
 * INPUTS: none
 * OUTPUTS: pointer to the current_pcb
 */
pcb_t* get_pcb() 
{
    return current_pcb;
}

/* The create_pcb helper function creates a new pcb and updates process number
 * INPUTS: none
 * OUTPUTS: pointer to a new pcb
 */
pcb_t* create_pcb(void) 
{   
    int i;
    pcb_t* pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(process_number+1)); // pcb addr = 8MB - 8KB * (process_number+1)
    pcb->process_num = process_number;
    pcb->arg_position = 0;
    for(i = 0; i < 127; i++) { // intialize all 128 argrments (buffer) in one pcb
        pcb->arguments[i] = 0x00;
    }
    // pcb file descriptor [0] -> stdin
    pcb->file_array[0].read = terminal_read; // stdin_read
    pcb->file_array[0].write = no_write; // do nothing
    pcb->file_array[0].open = terminal_open; // stdin_open
    pcb->file_array[0].close = terminal_close; // stdin_close
    pcb->file_array[0].flags = 1; // file descriptor[0] in use
    
    // pcb file descriptor [1] -> stdout
    pcb->file_array[1].read = no_read; // do nothing
    pcb->file_array[1].write = terminal_write; // std_write
    pcb->file_array[1].open = terminal_open; // std_open
    pcb->file_array[1].close = terminal_close; // std_close
    pcb->file_array[1].flags = 1; // file descriptor[1] in use

    /* updates running processes for terminals */
    if(pcb->process_num > 2) {
        pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_on_display+1));
        for(i = 0; i < 4; i++) {
            if(temp_pcb->active_processes[i] == 0) {
                temp_pcb->active_processes[i] = pcb->process_num;
                //temp_pcb->terminal_displays[i] = terminal_on_display+1;
                break;
            }
        }
    }
    pcb->parent_terminal = terminal_on_display;
    process_number++;
    return pcb;
}

/* The execute system call attempts to load and execute a new program, handing off the processor to the new program until it terminates.
 * INPUTS: uint8_t* command - the input command string
 * OUTPUTS: 0 if success, -1 if failure
 * SIDE EFFECTS: switch processer to user mode
 */
int32_t sys_execute(const uint8_t* command) 
{
    /*
    while(1) {
        if(terminal_on_display == terminal_counter) {
            cli();
            break;
        }
    }*/
    /* parse argument */
    if(command == NULL || *command == NULL || process_number >= 6) // bad pointer or too many programs
    {
        return -1;
    }
    //Stores file name inside a buffer (need to check for invalid file names)
    int i = 0;
    uint8_t process_name[MAX_COMMAND_LEN];
    process_name[MAX_COMMAND_LEN-1] = 0x00;
    while((command[i] != ' ' && command[i] != NULL) && i < MAX_COMMAND_LEN-1) // go to first space or end of string if only one word
    {
        process_name[i] = command[i];
        i++;
    }
    process_name[i] = 0x00;
    
    current_pcb = create_pcb();
    int fd = file_open(process_name);
    // add check if FD is valid
    uint8_t temp_buffer[40];
    int8_t magic_num[4] = {first_magic_num, second_magic_num, third_magic_num, fourth_magic_num};
    file_read(fd, temp_buffer, 40);
    if(strncmp((int8_t*)temp_buffer, magic_num, 4) != 0)  {
        // Not an executable, return -1
        file_close(fd);
        process_number--;
        current_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(process_number));
        return -1;
    }
    i++; // skip last character
    /* Copy argument into pcb buffer */
    while(command[i] == ' ') 
    {
        i++;
    }
    int offset = i;
    while(command[i] != 0x00) 
    {
        current_pcb->arguments[i-offset] = command[i];
        i++;
    }
    cli();
    /* Enable paging */
    system_call_paging(current_pcb);

    uint32_t *memory = (uint32_t *)FILE_IMAGE;
    for(i = 0; i < 40; i++) // copy temp buffer into memory
    {
        *((uint8_t*)memory + i) = temp_buffer[i];
    }
    /* Load file into memory */
    file_read(fd, memory+10, FOUR_MB);
    file_close(fd); // Close after reading from the program

    /* Prepare for context switch*/
    uint32_t register_eip = ((temp_buffer[27]<<24)|(temp_buffer[26]<<16))|((temp_buffer[25]<<8)|temp_buffer[24]);
    // Set TSS (subtract 1 entry because paging)
    tss.esp0 = (uint32_t)(EIGHT_MB - (EIGHT_KB*current_pcb->process_num));
    tss.ss0 = KERNEL_DS;

    if(recover_num > process_number)
    {
        process_number = recover_num; // if recovering from shutting down first shell
    }

    sti();
    return register_eip;
}

/* The save_esp helper functions saves the esp into the current pcb
 * INPUTS: value of current esp
 * OUTPUTS: none
 */
void save_esp(uint32_t register_esp) {
    current_pcb->esp = register_esp+4;
}

/* The restore_esp helper function initializes the process_number and current_pcb for first execute
 * INPUTS: none
 * OUTPUTS: the value of the esp saved in current_pcb
 */
uint32_t restore_esp() {
    return current_pcb->esp;
}

/* The read system call reads data from the keyboard, a file, device (RTC), or directory
 * INPUTS: int32_t fd - file descriptor
 *         void* buf - buffer 
 *         int32_t nbytes - how many characters read
 * OUTPUTS: number of characters read if success, -1 if failure
 */
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes) 
{
    if (buf == NULL || fd < 0 || fd >= FILE_ARRAY_LEN || current_pcb->file_array[fd].flags == 0) 
    {
        return -1;
    }
    return (int32_t)current_pcb->file_array[fd].read(fd, buf, nbytes);
}

/* The write system call writes data to the terminal or to a device (RTC)
 * INPUTS: int32_t fd - file descriptor
 *         void* buf - buffer 
 *         int32_t nbytes - how many characters written
 * OUTPUTS: number of characters written if success, -1 if failure
 */
int32_t sys_write (int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL || fd < 0 || fd >= FILE_ARRAY_LEN || current_pcb->file_array[fd].flags == 0) 
    {
        return -1;
    }
    return (int32_t)current_pcb->file_array[fd].write(fd, buf, nbytes);
}

/* The open system call provides access to the file system.
 * INPUTS: uint8_t* filename - file name
 * OUTPUTS: file index if success, -1 if failure
 */
int32_t sys_open (uint8_t* filename) {
    dentry_t dentry;
    int i;

    // Invalid file name
    if (filename == NULL || read_dentry_by_name(filename, &dentry) == -1) {
        return -1;
    }

    pcb_t* pcb = get_pcb();

    for (i = 2; i < FILE_ARRAY_LEN; i++) {
        if (pcb->file_array[i].flags == 0) // find the first unoccupied file descriptor
        {
            pcb->file_array[i].inode = dentry.inode_num;
            pcb->file_array[i].position = 0;
            pcb->file_array[i].flags = 1;

            switch(dentry.file_type) {
            case 0:
                pcb->file_array[i].read = rtc_read;
                pcb->file_array[i].write = rtc_write;
                pcb->file_array[i].open = rtc_open;
                pcb->file_array[i].close = rtc_close;
                rtc_open(filename); // Initialize the RTC
                break;
            case 1:
                pcb->file_array[i].read = directory_read;
                pcb->file_array[i].write = directory_write;
                pcb->file_array[i].open = directory_open;
                pcb->file_array[i].close = directory_close;
                break;
            case 2:
                pcb->file_array[i].read = file_read;
                pcb->file_array[i].write = file_write;
                pcb->file_array[i].open = file_open;
                pcb->file_array[i].close = file_close;
                break;
            default:
                break;
            }
            return i;
        }
    }
    return -1;
}

/* The close system call closes the specified file descriptor and make it available for return from later calls to open.
 * INPUTS: int32_t fd - file descriptor
 * OUTPUTS: 0 if success, -1 if failure
 */
int32_t sys_close (int32_t fd) {
    pcb_t* pcb = get_pcb();
    if (fd < 2 || fd > FILE_ARRAY_LEN || pcb->file_array[fd].flags == 0) {
        return -1;
    }
    return (int32_t)pcb->file_array[fd].close(fd);
}

/* The getargs call reads the program’s command line arguments into a user-level buffer
 * INPUTS: uint8_t* buf - buffer
 *         int32_t nbytes - how many characters read
 * OUTPUTS: 0 if success, -1 if failure
 */
int32_t sys_getargs(uint8_t* buf, int32_t nbytes)
{
    int bytes_read = 0;
    while(bytes_read < nbytes && (current_pcb->arguments[current_pcb->arg_position + bytes_read] != 0x00)) 
    {
        buf[bytes_read] = current_pcb->arguments[current_pcb->arg_position + bytes_read];
        bytes_read++;
    }
    buf[bytes_read] = 0x00;
    current_pcb->arg_position += bytes_read;
    if(bytes_read == 0)
    {
        return -1;
    }
    return 0;
}

/* The vidmap call maps the text-mode video memory into user space at a pre-set virtual address.
 * INPUTS: uint** screen_start - double pointer to user address space
 * OUTPUTS: the address if success, -1 if failure
 */
int32_t sys_vidmap(uint8_t** screen_start)
{
    if (screen_start == NULL || (uint32_t)screen_start < _128_MB || (uint32_t)screen_start > _128_MB + FOUR_MB) {
        return -1;
    }
    system_call_vidmap();
    /* remember to undo mapping in halt */
    *screen_start = (uint8_t*)(_128_MB + FOUR_MB); //connects the purple 4kb page in virtual to physical 4kb vide
    return _128_MB + FOUR_MB;
}


int32_t sys_set_handler (int32_t signum, void* handler_address)
{
    return -1;
}

int32_t sys_sigreturn (void)
{
    return -1;   
}

/* The check_process_num helper function returns the current process number
 * INPUTS: none
 * OUTPUTS: value of current process number
 */
uint32_t check_process_num() 
{
    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_on_display+1)); // get current terminals PCB
    int i;
    for(i = 3; i >= 0; i--)
    {
        if(temp_pcb->active_processes[i] != 0)
        {
            break; 
        }
    }

    if(i != -1)
    {
        // return temp_pcb->active_processes[i];
        return process_number;
    }
    else
    {
        return terminal_on_display;
    }


    // return process_number;
}

/* The start_first_shell helper function helps execute the first shell
 * INPUTS: none
 * OUTPUTS: none
 */
void start_first_shell()
{
    // sys_calls_init();
    current_pcb = NULL;
    recover_num = process_number; // save process num
    process_number = terminal_on_display; // we need to restart the terminal
    extern void system_execute(const uint8_t* command);
    system_execute((const uint8_t*) "shell");
}

