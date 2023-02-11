#include "scheduling.h"
#include "lib.h"
#include "rtc.h"
#include "system_call.h"
#include "i8259.h"
#include "types.h"
#include "paging.h"
#include "x86_desc.h"

#define NUM_SHELLS 3

// scheduling initialization function
void scheduling_init() {
    execute_counter = 0;
    terminal_init_flag = 0;
    terminal_on_display = 0;
    terminal_counter = 0;
    terminal_switch_flag = 0;
}

/* Function that switches between terminals
 * called upon alt + F1/F2/F3
 * INPUTS: terminal - terminal that we switch into
 * OUTPUTS: None
 */
void switch_terminal(int terminal) 
{
    // Video memory change of the terminals
    if (terminal < 0 || terminal > 2) { // invalid terminal num
        return;
    }
    int i;
    // switch the two terminal contents
    uint32_t offset1 = ((terminal_on_display+1)*FOUR_KB);
    //memcpy(VIDEO_MEMORY+offset1, VIDEO_MEMORY, FOUR_KB);

    for(i = 0; i < FOUR_KB; i++) 
    {
         *((uint8_t*)(VIDEO_MEMORY+offset1) + i) = *((uint8_t*)VIDEO_MEMORY + i);
    }
    uint32_t offset2 = ((terminal+1)*FOUR_KB);
    //memcpy(VIDEO_MEMORY, VIDEO_MEMORY+offset2, FOUR_KB);
    for(i = 0; i < FOUR_KB; i++)
    {
         *((uint8_t*)VIDEO_MEMORY + i) =  *((uint8_t*)(VIDEO_MEMORY+offset2) + i);
    }
    //cli();
    switch_terminal_cursors(terminal_on_display, terminal);
    terminal_on_display = terminal;
    #if 0
    if(execute_counter == 3 ) {
        scheduler_paging(terminal);
        extern void terminal_transition();
        terminal_transition();
    }
    #endif
}

/* Scheduler to manage all tasks
 * INPUTS: None
 * OUTPUTS: None
 */
void scheduler() {
    if(!terminal_init_flag && execute_counter < 3) { // if the switched teminal not initilized and executing process less than 3
        if(execute_counter > 0) // if not the first process to execute
        {
            switch_terminal(execute_counter);
            terminal_counter++;
        }
        execute_counter++;
        clear();
        set_pos(0, 0);
        sti();
        send_eoi(0);
        sti();
        extern void system_execute(const uint8_t* command);
        system_execute((const uint8_t*)"shell");
    }
    else if(execute_counter < 4)
    {
        switch_terminal(0);
        execute_counter++;
    }
    else // shells initialized
    {
        #if 0
        terminal_counter = (terminal_counter+1)%3;
        /* updates tss for next transition into stack(from user mode)*/
        pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_counter+1));
        int i;
        for(i = 3; i >= 0; i--) 
        {
            if(temp_pcb->active_processes[i] != 0) 
            {
                break;
            }
        }
        pcb_t* new_pcb;
        if(i != -1) {
            new_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(temp_pcb->active_processes[i]+1));
            tss.esp0 = (uint32_t)(EIGHT_MB - EIGHT_KB*temp_pcb->active_processes[i]);
        }
        else {
            new_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_counter+1));
            tss.esp0 = (uint32_t)(EIGHT_MB - EIGHT_KB*terminal_counter);
        }
        // Check for no active processes (i = 4 because never broke during loop)
        /*
        // tss -> store esp for next user to kernel
        // at end, terminal 0 should store at 8 MB, terminal 1 should store at 8MB-8KB, e.t.c.
        tss.esp0 = (i == -1) ? (uint32_t)(EIGHT_MB - EIGHT_KB*terminal_counter) : (uint32_t)(EIGHT_MB - (EIGHT_KB*temp_pcb->active_processes[i]));

        // Either use base terminal process num, or the highest prio in active_processes
        int tempProcess = (i != -1) ? temp_pcb->active_processes[i] : temp_pcb->process_num;
        */
        scheduler_paging(new_pcb->process_num);                                                                                                                                                   
        //extern void switch_processes();
        //switch_processes();
        #endif
    }
}

/* Save the registers for the current excuting process
 * INPUTS: uint32_t ebp - target/new ebp 
 *         uint32_t esp - target/new esp
 * OUTPUTS: None
 */
void save_registers_esp(uint32_t esp) {
    /* saves current ebp and esp */

    //uint32_t process = terminal_counter;
    #if 0
    if(process_number < 3) // directly use terminals
    {
        process = terminal_counter;
    }
    else
    {
        // get pcb for current process
        // get parent process number
        // get pcb for parent process
        pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(process_number+1)); // PCB for current process

        pcb_t* actual_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(temp_pcb.parent_terminal+1));
    }
    uint32_t process = (process_number < 3) ? terminal_counter : process_number;
    #endif
    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_counter+1));
    int i;
    for(i = 4; i >= 0; i--) { // display / 0 / 1 / 2
        if(temp_pcb->active_processes[i] != 0) { // if already active, break
            break;
        }
    }

    // Get highest active process for currently scheduled terminal
    pcb_t* new_pcb = (i == -1) ? (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_counter+1)) : (pcb_t*)(EIGHT_MB - EIGHT_KB*(temp_pcb->active_processes[i]+1));
    new_pcb->scheduling_esp = esp;
}

/* Get the register esp for the new process 
 * Set %eax to be new_pcb->scheduling_esp
 * INPUTS: None
 * OUTPUTS: new esp
 */
uint32_t restore_register_esp() 
{
    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_counter+1));
    int i;
    for(i = 3; i >= 0; i--) { // display / 0 / 1 / 2
        if(temp_pcb->active_processes[i] != 0) { // if already active, break
            break;
        }
    }
    // Get PCB for highest active process
    pcb_t* new_pcb = (i == -1) ? (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_counter+1)) : (pcb_t*)(EIGHT_MB - EIGHT_KB*(temp_pcb->active_processes[i]+1)); // if i == 4, first operation, else second operaion
    return new_pcb->scheduling_esp;
}

uint32_t restore_register_esp_terminal() 
{
    pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_on_display+1));
    int i;
    for(i = 3; i >= 0; i--) { // display / 0 / 1 / 2
        if(temp_pcb->active_processes[i] != 0) { // if already active, break
            break;
        }
    }
    // Get PCB for highest active process
    pcb_t* new_pcb = (i == 4) ? (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_on_display+1)) : (pcb_t*)(EIGHT_MB - EIGHT_KB*(temp_pcb->active_processes[i]+1)); // if i == 4, first operation, else second operaion
    return new_pcb->scheduling_esp;
}



