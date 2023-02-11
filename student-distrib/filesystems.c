#include "filesystems.h"
#include "lib.h"
#include "system_call.h"

#define FILE_NAME_LEN 32

static uint32_t* boot_block_start; // Pointer to the first 4kB box in the file system
static uint32_t* inode_block_start; // Pointer to the first inode block
static uint32_t* data_block_start; // Pointer to the first data block

void fileSystem_init(uint32_t * fs_start) 
{
    boot_block_start = fs_start;
    inode_block_start = fs_start + BLOCK_OFFSET; // 1024 * 4 = 4 KB, inodes are one block after boot block
    int num_of_inodes = *(boot_block_start+1);
    data_block_start = inode_block_start + (num_of_inodes*BLOCK_OFFSET); // skip over all the inodes to get to the first data block
}

//param: pointer to a valid 4 byte string
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
    if(fname == NULL || dentry == NULL) // null check for bad pointers  
    {
        return -1;
    }
    uint8_t* temp = (uint8_t*) fname;
    uint8_t counter = 0;
    while(*(temp) != NULL)
    {
        temp++;
        counter++;
    }
    if(counter > 32) // fname is longer than 32 characters
    {
        return -1;
    }
    uint32_t *curr_addr = boot_block_start;    //4 byte-addressable pointer
    int num_of_entries = *curr_addr;   //number of dir entries
    int i;
    curr_addr += 16;    //curr_addr points to start of dir entries
    int found = 0; // update flag if we find the file
    for(i = 0; i < num_of_entries; i++) {
        if(strncmp((int8_t*)fname, (int8_t*)curr_addr, 31) == 0) {
            found = 1;
            break;
        }
        curr_addr += 16; // 16 * 4 = 64 B, skip to next dentry
    }
    if(!found) // File does not exist
    {
        return -1;
    }

    dentry_t* dentry_pointer = (dentry_t*)(curr_addr);
    for(i = 0; i < CHARS_IN_FNAME; i++)
    {
        dentry->file_name[i] = dentry_pointer->file_name[i];
    }
    dentry->file_type = dentry_pointer->file_type;
    dentry->inode_num = dentry_pointer->inode_num;
    for(i = 0; i < RESERVED_BYTES; i++)
    {
        dentry->reserved[i] = dentry_pointer->reserved[i];
    }
    return 0;
}


int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
    int numOfEntries = *boot_block_start;
    if((index < 0 || index >= numOfEntries) || dentry == NULL)// invalid index or dentry
    {
        return -1;
    }

    // Get to first directory, offset by index
    dentry_t* dentry_pointer = (dentry_t*)((boot_block_start + 16) + (16 * index));
    int i;
    for(i = 0; i < CHARS_IN_FNAME; i++)
    {
        dentry->file_name[i] = dentry_pointer->file_name[i];
    }
    dentry->file_type = dentry_pointer->file_type;
    dentry->inode_num = dentry_pointer->inode_num;
    for(i = 0; i < RESERVED_BYTES; i++)
    {
        dentry->reserved[i] = dentry_pointer->reserved[i];
    }
    return 0;
}


int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) 
{
    uint32_t num_of_inodes = *(boot_block_start + 1);
    if(buf == NULL || inode < 0 || offset < 0 || length < 0 || inode >= num_of_inodes) // invalid inode num
    {
        return -1;
    }
    uint32_t* inode_addr = inode_block_start + (inode*BLOCK_OFFSET);  // curr_addr points to start of correct inode
    uint32_t length_of_data = *inode_addr;  // length of file data
    uint32_t* inode_block_ptr = inode_addr + 1 + (offset/BLOCK_SIZE); // Points to data block # for the starting data block
    uint32_t block_num = *(inode_block_ptr); // number of the first block we read from
    uint8_t* data_block_ptr = (uint8_t*)(data_block_start + (block_num*BLOCK_OFFSET)); 
    uint32_t currentByte = offset % BLOCK_SIZE; 
    uint32_t bytesRead = 0; // counter 
    while(bytesRead < length)
    {
        if(offset+bytesRead >= length_of_data) {
            break;
        }
        buf[bytesRead] = data_block_ptr[currentByte]; // Read byte
        bytesRead++; // increment counters
        currentByte++;
        // Check if we reached the end of a data block
        if(currentByte >= BLOCK_SIZE)
        {
            // Increment inode_block_ptr to point to next data block
            inode_block_ptr++;
            block_num = *(inode_block_ptr);
            // update data_block_ptr
            data_block_ptr = (uint8_t*)(data_block_start + (block_num*BLOCK_OFFSET)); 
            currentByte = 0;
        }
    }
    return bytesRead;
}

/* File Driver Functions */
int file_open(uint8_t *filename) 
{
    dentry_t tempDent;

    if(filename == NULL) // bad file name
    {
        return -1;
    }

    int results = read_dentry_by_name(filename, &tempDent);
    if(results == -1) // File not found
    {
        return -1;
    }

    int i;
    for(i = 0; i < 8; i++) 
    {
        extern pcb_t* current_pcb;
        if(current_pcb->file_array[i].flags == 0) 
        {
            // open spot, store in here and mark as used
            current_pcb->file_array[i].flags = 1; // claim spot
            current_pcb->file_array[i].inode = tempDent.inode_num;
            current_pcb->file_array[i].position = 0; // start at the first byte
            return i; // i is the file descriptor
        }
    }
    return -1;

}
int file_close(int32_t fd)
{
    // Invalid FD
    if(fd < 0 || fd > 7)
    {
        return -1;
    }
    if(current_pcb->file_array[fd].flags == 1) // If file is open
    {
        current_pcb->file_array[fd].inode = 0;
        current_pcb->file_array[fd].position = 0; // start at the first byte
        current_pcb->file_array[fd].flags = 0; // mark it as available
        return 0;
    }
    return -1; // file was not open, nothing to close
}

int file_read(int32_t fd, void* buf, int32_t nbytes)
{
    // Invalid buffer or fd
    if(buf == NULL || fd < 0 || fd > 7)
    {
        return -1;
    }
    int results = read_data(current_pcb->file_array[fd].inode, current_pcb->file_array[fd].position, (uint8_t*)buf, nbytes);
    if(results == -1)
    {
        return -1;
    }
    else if(results == 0)
    {
        current_pcb->file_array[fd].position = *(inode_block_start + (current_pcb->file_array[fd].inode *1024)); // Length of file in B
    }
    else
    {
        current_pcb->file_array[fd].position += nbytes; // increment position
    }
    return results;
}

// Do nothing, return -1. Should never be called.
int file_write(int32_t fd, void* buf, int32_t nbytes) 
{ 
    return -1;
}

/* Directory Driver Functions */
int directory_open(uint8_t* file_name)
{
    dentry_t tempDentry;
    int results = read_dentry_by_name((uint8_t*)"/", &tempDentry);
    return results;
}


int directory_read(int32_t fd, void* buf, int32_t nbytes)
{
    if((uint8_t*)buf == NULL || fd < 0 || fd > 7)
    {
        return -1;
    }
    int num_bytes;
    if(nbytes > FILE_NAME_LEN) // Max is maximum file name length
    {
        num_bytes = FILE_NAME_LEN;
    }
    else
    {
        num_bytes = nbytes;
    }
    dentry_t tempDentry;
    int currentFileIndex = (current_pcb->file_array[fd].position / FILE_NAME_LEN); 
    int numDentries = *(boot_block_start);
    if(currentFileIndex == numDentries)
    {
        return 0;
    }
    int results = read_dentry_by_index(currentFileIndex, &tempDentry); // Read next dentry
    if(results == -1) // bad dentry
    {
        return -1;
    }
    int bytes_read = 0;
    int i;
    for(i = 0; i < num_bytes; i++) {
        ((int8_t*)buf)[i] = tempDentry.file_name[i];
        if(tempDentry.file_name[i] != NULL)
        {
            bytes_read++;
        }
    }
    current_pcb->file_array[fd].position += num_bytes; 
    return bytes_read;
}

int directory_close(int32_t fd)
{
    if(fd < 0 || fd > 7)
    {
        return -1;
    }
    current_pcb->file_array[fd].position = 0; // reset position
    current_pcb->file_array[fd].flags = 0; // no longer in use
    return 0;
}

int directory_write(int32_t fd, void* buf, int32_t nbytes)
{
    return -1;
}


