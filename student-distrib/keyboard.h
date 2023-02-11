/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"

#define KEYBOARD_IRQ_NUM    1
#define KEYBOARD_DATA_PORT  0x60
#define KEYBOARD_CMD_PORT   0x64

#define ESC_SCAN_CODE             0x01

// released
#define SHIFT_L_SCAN_CODE_R       0xAA
#define SHIFT_R_SCAN_CODE_R       0xB6
#define CONTROL_L_SCAN_CODE_R     0x9D
#define CONTROL_R_SCAN_CODE_R

// Number
#define ONE_SCAN_CODE             0x02
#define TWO_SCAN_CODE             0x03
#define THREE_SCAN_CODE           0x04
#define FOUR_SCAN_CODE            0x05
#define FIVE_SCAN_CODE            0x06
#define SIX_SCAN_CODE             0x07
#define SEVEN_SCAN_CODE           0x08
#define EIGHT_SCAN_CODE           0x09
#define NINE_SCAN_CODE            0x0A
#define ZERO_SCAN_CODE            0x0B

#define SUB_SCAN_CODE             0x0C
#define EQUAL_SCAN_CODE           0x0D
#define BACKSPACE_SCAN_CODE       0x0E
#define TAB_SCAN_CODE             0x0F
#define TAB_SCAN_CODE_R           0x8F

// letter
#define Q_SCAN_CODE               0x10
#define W_SCAN_CODE               0x11
#define E_SCAN_CODE               0x12
#define R_SCAN_CODE               0x13
#define T_SCAN_CODE               0x14
#define Y_SCAN_CODE               0x15
#define U_SCAN_CODE               0x16
#define I_SCAN_CODE               0x17
#define O_SCAN_CODE               0x18
#define P_SCAN_CODE               0x19

#define S_BRACKET_L_SCAN_CODE     0x1A
#define S_BRACKET_R_SCAN_CODE     0x1B
#define ENTER_SCAN_CODE           0x1C
#define CONTROL_L_SCAN_CODE       0x1D

// letter
#define A_SCAN_CODE               0x1E
#define S_SCAN_CODE               0x1F
#define D_SCAN_CODE               0x20
#define F_SCAN_CODE               0x21
#define G_SCAN_CODE               0x22
#define H_SCAN_CODE               0x23
#define J_SCAN_CODE               0x24
#define K_SCAN_CODE               0x25
#define L_SCAN_CODE               0x26

#define SEMICOLON_SCAN_CODE       0x27
#define SINGLE_QUOTE_SCAN_CODE    0x28
#define BACK_TICK_SCAN_CODE       0x29
#define SHIFT_L_SCAN_CODE         0x2A
#define SLASH_R_SCAN_CODE         0x2B

// letter
#define Z_SCAN_CODE               0x2C
#define X_SCAN_CODE               0x2D
#define C_SCAN_CODE               0x2E
#define V_SCAN_CODE               0x2F
#define B_SCAN_CODE               0x30
#define N_SCAN_CODE               0x31
#define M_SCAN_CODE               0x32

#define COMMA_SCAN_CODE           0x33
#define DOT_SCAN_CODE             0x34
#define SLASH_L_SCAN_CODE         0x35
#define SHIFT_R_SCAN_CODE         0x36
#define KEYPAD_STAR_SCAN_CODE     0x37
#define ALT_L_SCAN_CODE           0x38
#define ALT_L_SCAN_CODE_R         0xB8
#define SPACE_SCAN_CODE           0x39
#define CAPS_LOCK_SCAN_CODE       0x3A

#define F1_SCAN_CODE              0x3B
#define F2_SCAN_CODE              0x3C
#define F3_SCAN_CODE              0x3D
#define F4_SCAN_CODE              0x3E
#define F5_SCAN_CODE              0x3F
#define F6_SCAN_CODE              0x40
#define F7_SCAN_CODE              0x41
#define F8_SCAN_CODE              0x42
#define F9_SCAN_CODE              0x43
#define F10_SCAN_CODE             0x44

#define NUMBER_LOCK_SCAN_CODE     0x45
#define SCROLL_LOCK_SCAN_CODE     0x46

#define KEYPAD_SEVEN_SCAN_CODE    0x47
#define KEYPAD_EIGHT_SCAN_CODE    0x48
#define KEYPAD_NINE_SCAN_CODE     0x49
#define KEYPAD_SUB_SCAN_CODE      0x4A
#define KEYPAD_FOUR_SCAN_CODE     0x4B
#define KEYPAD_FIVE_SCAN_CODE     0x4C
#define KEYPAD_SIX_SCAN_CODE      0x4D
#define KEYPAD_ADD_SCAN_CODE      0x4E
#define KEYPAD_ONE_SCAN_CODE      0x4F
#define KEYPAD_TWO_SCAN_CODE      0x50
#define KEYPAD_THREE_SCAN_CODE    0x51
#define KEYPAD_ZERO_SCAN_CODE     0x52
#define KEYPAD_DOT_SCAN_CODE      0x53
#define F11_SCAN_CODE             0x57
#define F12_SCAN_CODE             0x58

#define ESC_ASCII             0x01
#define SPACE_ASCII           0x20

// Number
#define ONE_ASCII             0x31
#define TWO_ASCII             0x32
#define THREE_ASCII           0x33
#define FOUR_ASCII            0x34
#define FIVE_ASCII            0x35
#define SIX_ASCII             0x36
#define SEVEN_ASCII           0x37
#define EIGHT_ASCII           0x38
#define NINE_ASCII            0x39
#define ZERO_ASCII            0x30

#define SUB_ASCII             0x2D
#define EQUAL_ASCII           0x3D

// letter
#define Q_ASCII               0x71
#define W_ASCII               0x77
#define E_ASCII               0x65
#define R_ASCII               0x72
#define T_ASCII               0x74
#define Y_ASCII               0x79
#define U_ASCII               0x75
#define I_ASCII               0x69
#define O_ASCII               0x6F
#define P_ASCII               0x70

#define S_BRACKET_L_ASCII     0x5B
#define S_BRACKET_R_ASCII     0x5D

// letter
#define A_ASCII               0x61
#define S_ASCII               0x73
#define D_ASCII               0x64
#define F_ASCII               0x66
#define G_ASCII               0x67
#define H_ASCII               0x68
#define J_ASCII               0x6A
#define K_ASCII               0x6B
#define L_ASCII               0x6C

#define SEMICOLON_ASCII       0x3B
#define SINGLE_QUOTE_ASCII    0x27
#define BACK_TICK_ASCII       0x60
#define SLASH_R_ASCII         0x5C

// letter
#define Z_ASCII               0x7A
#define X_ASCII               0x78
#define C_ASCII               0x63
#define V_ASCII               0x76
#define B_ASCII               0x62
#define N_ASCII               0x6E
#define M_ASCII               0x6D

#define COMMA_ASCII           0x2C
#define DOT_ASCII             0x2E
#define SLASH_L_ASCII         0x2F

#define EXCLAMATORY_ASCII     0x21
#define AT_ASCII              0x40
#define NUMBER_SIGN_ASCII     0x23
#define DOLLAR_ASCII          0x24
#define PERCENT_ASCII         0x25
#define CARET_ASCII           0x5E
#define AND_ASCII             0x26
#define STAR_ASCII            0x2A
#define PARENTHESES_L_ASCII   0x28
#define PARENTHESES_R_ASCII   0x29
#define UNDERLINE_ASCII       0x5F
#define ADD_ASCII             0x2B
#define A_BRACKET_L_ASCII     0x7B
#define A_BRACKER_R_ASCII     0x7D
#define BAR_ASCII             0x7C
#define COLON_ASCII           0x3A
#define DOUBLE_QUOTE_ASCII    0x22
#define LESS_ASCII            0x3C
#define GREATER_ASCII         0x3E
#define QUESTION_ASCII        0x3F

#define TILDE_ASCII           0x7E

/*
unsigned char terminal_buffer[127];
unsigned int terminal_buffer_idx;
unsigned char terminal_buffer1[127];
unsigned int terminal_buffer_idx1;
unsigned char terminal_buffer2[127];
unsigned int terminal_buffer_idx2;*/

// plug in terminal number to get the right buffer and idx
unsigned char terminal_buffer[127][3];
unsigned int terminal_buffer_idxs[3];

void keyboard_init();
void keyboard_handler();
int32_t terminal_open(uint8_t* file_name);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes);

#endif /* _KEYBOARD_H */
