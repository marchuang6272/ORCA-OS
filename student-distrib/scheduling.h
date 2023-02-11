#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "types.h"
#include "lib.h"

int terminal_init_flag; // initialized to 0, set to 1 after all terminals are initialized
volatile int terminal_counter; // what terminal is currently being processed by the scheduler
volatile int terminal_on_display; // What terminal is currently being displayed
int execute_counter; // What terminal is currently being displayed
int flag;
int terminal_switch_flag;

int recover_num; // stores a recovery value if we detect the base shells were terminated

// scheduling initialization function
void scheduling_init();
// Function that switches between terminals called upon alt + F1/F2/F3
void switch_terminal(int terminal);
// Scheduler to manage all tasks
void scheduler();
// Save the registers for the current excuting process
void save_registers_esp(uint32_t esp);
// Get the register esp for the new process 
uint32_t restore_register_esp();
uint32_t restore_register_esp_terminal();

#endif

