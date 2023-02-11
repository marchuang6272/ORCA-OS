/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0
#define FILE_ARRAY_LEN (8)
#define first_magic_num 0x7f
#define second_magic_num 0x45
#define third_magic_num 0x4c
#define fourth_magic_num 0x46
#define EIGHT_MB 0x800000
#define EIGHT_KB 0x2000
#define FOUR_MB  0x400000
#define FOUR_KB  0x1000
#define _128_MB  0x8000000

#define VIDEO_MEMORY 0xB8000

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

typedef struct __attribute__ ((packed)) fd_t
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*open)(uint8_t* file_name);
    int32_t (*close)(int32_t fd);
    uint32_t inode; // Should be 0 for devices
    uint32_t position; // Current number of bytes into file
    uint32_t flags; // 1 = in use, 0 = free to be overwritten
} fd_t;

// PCB Block -> Represents a process
typedef struct __attribute__ ((packed)) pcb_t
{
    uint32_t process_num;
    uint32_t parent_terminal;
    uint32_t parent_terminal_number;
    uint32_t scheduling_esp;
    uint32_t active_processes[4];
    uint32_t terminal_displays[4];
    uint32_t executed_processes[4];
    fd_t file_array[FILE_ARRAY_LEN];
    uint32_t esp;
    uint8_t arguments[128];
    uint32_t arg_position;
} pcb_t;

#endif /* ASM */

#endif /* _TYPES_H */
