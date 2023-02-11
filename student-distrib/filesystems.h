#ifndef _H_FILE_SYSTEMS_
#define _H_FILE_SYSTEMS_

#include "types.h"

// 32 characters in a file name (zero-padded so just always use 32)
#define CHARS_IN_FNAME (32)
// 64 bytes per dentry
#define DENTRY_SIZE (64)
// 4 bytes in each 32 bit entry
#define NUM_OF_BYTES (4) 

#define RESERVED_BYTES (24)

// 4 KB boxes
#define BLOCK_SIZE (4 * 1024)

// 4096 = 4 KB / 4 = 1024
#define BLOCK_OFFSET (1024)


/**
 * A structure that represents a directory entry (dentry).
 *
*/
typedef struct dentry_t
{
    int8_t file_name[CHARS_IN_FNAME]; // Array for file name (chars)
    uint32_t file_type; // 0 = device like RTC, 1 = directory, 2 = normal file
    uint32_t inode_num; // Which inode is associated with this file? 0 for devices. (4B)
    int8_t reserved[RESERVED_BYTES];
} __attribute__ ((packed)) dentry_t;

// File Descriptor Block -> Represents an open file
typedef struct file_desc_block_t
{ 
    uint32_t inode; // Should be 0 for devices
    uint32_t position; // Current number of bytes into file
    uint32_t flags; // 1 = in use, 0 = free to be overwritten
} file_desc_block_t;

/** 
 * @brief Initializes the file system pointers
 * 
*/
void fileSystem_init(uint32_t * fs_start);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

// File Driver Functions
/**
 * Prepares a file for reading.
 * @input file_name
 * Outputs: A file descriptor integer (0-7), -1 means file not found
*/
int file_open(uint8_t* file_name);

int file_close(int32_t fd);

/**
 * 
*/
int file_read(int32_t fd, void* buf, int32_t nbytes);

/**
 * Does nothing and returns -1; here for posterity.
*/
int file_write(int32_t fd, void* buf, int32_t nbytes);

// Directory Driver Functions
int directory_open(uint8_t* file_name);
int directory_close(int32_t fd);
int directory_read(int32_t fd, void* buf, int32_t nbytes);

/**
 * Does nothing and returns -1; here for posterity.
*/
int directory_write(int32_t fd, void* buf, int32_t nbytes);

#endif
/*** end of file ***/


