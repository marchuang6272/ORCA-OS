/* keyboard.c - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "idt.h"
#include "scheduling.h"

/* Global variables */
uint8_t shift = 0;
uint8_t shift_l = 0;
uint8_t shift_r = 0;
uint8_t caps_lock = 0;
uint8_t ctrl = 0;
uint8_t alt = 0;
uint8_t enter = 0;
uint8_t line_run_over_flag = 0;
char *string = "12312312";
char *string2 = "78978797";

extern int backspace_on_screen();
extern int backspace_to_prev();
extern int update_cursor();

/* Initialize the keyboard */
void keyboard_init() {
    cli();
    extern void keyboard_linkage(void);
    SET_IDT_ENTRY(idt[0x21], keyboard_linkage);
    set_idt(0x21);
    enable_irq(KEYBOARD_IRQ_NUM);
    //outb(0xF4, KEYBOARD_CMD_PORT); // enable scanning
}

int32_t terminal_open(uint8_t* file_name) {
    int i;
    for (i=0;i<127;i++) {
        terminal_buffer[i][terminal_on_display] = 0x00;
    }
    terminal_buffer_idxs[terminal_on_display] = 0;
    //Todo: correct return value
    return 0;
}

int32_t terminal_close(int32_t fd) {
    int i;
    for (i=0;i<127;i++) {
        terminal_buffer[i][terminal_on_display] = 0x00;
    }
    terminal_buffer_idxs[terminal_on_display] = 0;
    //to do: figure out the correct return value
    return 0;
}

int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    sti();
    enter = 0;
    while(!enter) {  
        line_run_over_flag = 1;  
    }
    line_run_over_flag = 0;
    cli();
    int i;
    for (i = 0; i<nbytes-1; i++) {
        if (i >= terminal_buffer_idxs[terminal_on_display]) {
            break;
        } else {
            ((char*) buf)[i] = terminal_buffer[i][terminal_on_display];
        }
    }
    ((char*) buf)[i] = 0x00;

    // reset buffer every enter
    int j;
    for(j = i; j < nbytes; j++)
    {
        ((char*) buf)[j] = 0x00;
    }
    terminal_buffer_idxs[terminal_on_display] = 0; // "Clear" buffer
    nbytes = 0;
    sti();

   //Todo: Return value
   return i+1;
}


int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes) 
{
    cli();
    return write_string_to_screen((int8_t*) buf, nbytes);
    sti();
}


// use the scan_code to find the corresponding ASCII
int8_t scan_code_to_ascii(uint32_t scan_code) {
    uint8_t i;
    switch(scan_code) {
        case(F1_SCAN_CODE):
            return F1_SCAN_CODE;
        case(F2_SCAN_CODE): 
            return F2_SCAN_CODE;
        case(F3_SCAN_CODE): 
            return F3_SCAN_CODE;
        case(SHIFT_L_SCAN_CODE):
            if (!shift_r) {
                shift = 1;
                shift_l = 1;
            }
            return 0;
        case(SHIFT_L_SCAN_CODE_R):
            if (shift_l) {
                shift = 0;
                shift_l = 0;
            }
            return 0;
        case(SHIFT_R_SCAN_CODE):
            if (!shift_l) {
                shift = 1;
                shift_r = 1;
            }
            return 0;
        case(SHIFT_R_SCAN_CODE_R):
            if (shift_r) {    
                shift = 0;
                shift_r = 0;
            }
            return 0;
        case(TAB_SCAN_CODE):
            if (line_run_over_flag) {
                for (i = 0; i < 5; i++) {
                    if (terminal_buffer_idxs[terminal_on_display] < 127) {
                        putc_line_runover(SPACE_ASCII); // five ' '
                        terminal_buffer[terminal_buffer_idxs[terminal_on_display]][terminal_on_display] = SPACE_ASCII;
                        terminal_buffer_idxs[terminal_on_display]++;
                    }
                }
            } else {
                for (i = 0; i < 5; i++) {
                    if (terminal_buffer_idxs[terminal_on_display] < 127) {
                        putc(SPACE_ASCII); // five ' '
                        terminal_buffer[terminal_buffer_idxs[terminal_on_display]][terminal_on_display] = SPACE_ASCII;
                        terminal_buffer_idxs[terminal_on_display]++;
                    }
                }
            }
            update_cursor();
            return 0;
        case(CAPS_LOCK_SCAN_CODE):
            caps_lock = !caps_lock;
            return 0;
        case(CONTROL_L_SCAN_CODE):
            ctrl = 1;
            return 0;
        case(CONTROL_L_SCAN_CODE_R):
            ctrl = 0;
            return 0;
        case(BACKSPACE_SCAN_CODE):
            if (terminal_buffer_idxs[terminal_on_display] == 0) {
                return 0;
            } 
            else if (line_run_over_flag && terminal_buffer_idxs[terminal_on_display] > 0) {
                terminal_buffer_idxs[terminal_on_display]--;
                backspace_to_prev();
            } 
            else if (!line_run_over_flag) {
                backspace_to_prev();
            }
            return 0;
        case(ENTER_SCAN_CODE):
            shift_rows_up();
            if (line_run_over_flag) {
                putc_line_runover('\n');
            } else {
                putc('\n');
            }
            update_cursor();
            enter = 1;
            return 0;
        case(ALT_L_SCAN_CODE):
            alt = 1;
            return 0;
        case(ALT_L_SCAN_CODE_R):
            alt = 0;
            return 0;

        // number row
        case(ONE_SCAN_CODE):
            if (shift) {
                return EXCLAMATORY_ASCII;
            }
            return ONE_ASCII;
        case(TWO_SCAN_CODE):
            if (shift) {
                return AT_ASCII;
            }
            return TWO_ASCII;
        case(THREE_SCAN_CODE):
            if (shift) {
                return NUMBER_SIGN_ASCII;
            }
            return THREE_ASCII;
        case(FOUR_SCAN_CODE):
            if (shift) {
                return DOLLAR_ASCII;
            }
            return FOUR_ASCII;
        case(FIVE_SCAN_CODE):
            if (shift) {
                return PERCENT_ASCII;
            }
            return FIVE_ASCII;
        case(SIX_SCAN_CODE):
            if (shift) {
                return CARET_ASCII;
            }
            return SIX_ASCII;
        case(SEVEN_SCAN_CODE):
            if (shift) {
                return AND_ASCII;
            }
            return SEVEN_ASCII;
        case(EIGHT_SCAN_CODE):
            if (shift) {
                return STAR_ASCII;
            }
            return EIGHT_ASCII;
        case(NINE_SCAN_CODE):
            if (shift) {
                return PARENTHESES_L_ASCII;
            }
            return NINE_ASCII;
        case(ZERO_SCAN_CODE):
            if (shift) {
                return PARENTHESES_R_ASCII;
            }
            return ZERO_ASCII;

        // first row
        case(Q_SCAN_CODE):
            return Q_ASCII;
        case(W_SCAN_CODE):
            return W_ASCII;
        case(E_SCAN_CODE):
            return E_ASCII;
        case(R_SCAN_CODE):
            return R_ASCII;
        case(T_SCAN_CODE):
            return T_ASCII;
        case(Y_SCAN_CODE):
            return Y_ASCII;
        case(U_SCAN_CODE):
            return U_ASCII;
        case(I_SCAN_CODE):
            return I_ASCII;
        case(O_SCAN_CODE):
            return O_ASCII;
        case(P_SCAN_CODE):
            return P_ASCII;

        // second row
        case(A_SCAN_CODE):
            return A_ASCII;
        case(S_SCAN_CODE):
            return S_ASCII;
        case(D_SCAN_CODE):
            return D_ASCII;
        case(F_SCAN_CODE):
            return F_ASCII;
        case(G_SCAN_CODE):
            return G_ASCII;
        case(H_SCAN_CODE):
            return H_ASCII;
        case(J_SCAN_CODE):
            return J_ASCII;
        case(K_SCAN_CODE):
            return K_ASCII;
        case(L_SCAN_CODE):
            return L_ASCII;

        // third row
        case(Z_SCAN_CODE):
            return Z_ASCII;
        case(X_SCAN_CODE):
            return X_ASCII;
        case(C_SCAN_CODE):
            return C_ASCII;
        case(V_SCAN_CODE):
            return V_ASCII;
        case(B_SCAN_CODE):
            return B_ASCII;
        case(N_SCAN_CODE):
            return N_ASCII;
        case(M_SCAN_CODE):
            return M_ASCII;

        // symbol
        case(SUB_SCAN_CODE):
            if (shift) {
                return UNDERLINE_ASCII;
            }
            return SUB_ASCII;
        case(EQUAL_SCAN_CODE):
            if (shift) {
                return ADD_ASCII;
            }
            return EQUAL_ASCII;
        case(S_BRACKET_L_SCAN_CODE):
            if (shift) {
                return A_BRACKET_L_ASCII;
            }
            return S_BRACKET_L_ASCII;
        case(S_BRACKET_R_SCAN_CODE):
            if (shift) {
                return A_BRACKER_R_ASCII;
            }
            return S_BRACKET_R_ASCII;
        case(SLASH_R_SCAN_CODE):
            if (shift) {
                return BAR_ASCII;
            }
            return SLASH_R_ASCII;
        case(SEMICOLON_SCAN_CODE):
            if (shift) {
                return COLON_ASCII;
            }
            return SEMICOLON_ASCII;
        case(SINGLE_QUOTE_SCAN_CODE):
            if (shift) {
                return DOUBLE_QUOTE_ASCII;
            }
            return SINGLE_QUOTE_ASCII;
        case(COMMA_SCAN_CODE):
            if (shift) {
                return LESS_ASCII;
            }
            return COMMA_ASCII;
        case(DOT_SCAN_CODE):
            if (shift) {
                return GREATER_ASCII;
            }
            return DOT_ASCII;
        case(SLASH_L_SCAN_CODE):
            if (shift) {
                return QUESTION_ASCII;
            }
            return SLASH_L_ASCII;
        case(BACK_TICK_SCAN_CODE):
            if (shift) {
                return TILDE_ASCII;
            }
            return BACK_TICK_ASCII;
        case(SPACE_SCAN_CODE):
            return SPACE_ASCII;

        default:
            return 0; // NULL ASCII
    }
}

void keyboard_handler() 
{
    uint8_t key = scan_code_to_ascii(inb(KEYBOARD_DATA_PORT));
    if (ctrl) {
        if (key == L_ASCII) {
            clear();
            set_pos(0,0);
            update_cursor();
            terminal_open(NULL);
        }   
    }

    if (alt) {
        if (key == F1_SCAN_CODE) {
            switch_terminal(0);
        }
        else if (key == F2_SCAN_CODE) {
            switch_terminal(1);
        }
        else if (key == F3_SCAN_CODE) {
            switch_terminal(2);
        }
    }
    
    if (line_run_over_flag && !alt) {
        if (((key >= 0x61) & (key <= 0x7A)) & (shift ^ caps_lock)) { // if alphabet and capital
            key = key - 0x20;
        }
        if ((key != 0) & (!ctrl)) { //print if key is something other than null
            if (terminal_buffer_idxs[terminal_on_display]<127) {
                putc_line_runover(key);
                terminal_buffer[(terminal_buffer_idxs[terminal_on_display])][terminal_on_display] = (char)key;
                terminal_buffer_idxs[terminal_on_display]++;
            }
            update_cursor();
        }
    }
    send_eoi(KEYBOARD_IRQ_NUM);
}

