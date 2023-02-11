#include "paging.h"
#include "types.h"
#include "x86_desc.h"
#include "scheduling.h"

void page_init(void)
{
    unsigned int i;
    //enables r/w for all pde's, but sets ready bit to 0
    for(i = 0; i < 1024; i++) 
        page_directory[i].rw = 1;
    // Point first entry of the page directory to the page table
    page_directory[0].present = 1;
    page_directory[0].rw = 1;
    page_directory[0].val |= (unsigned int)(first_page_table); // store address by using it as a uint

    page_directory[1].upper_address = 0x1; // PAT = 1
    page_directory[1].present = 1;
    page_directory[1].rw = 1;
    page_directory[1].pcd = 1;
    page_directory[1].page_size = 1; // 4 MB page

    //maps 4 MB
    for(i = 0; i < 1024; i++)
    {
        first_page_table[i].val = (i * 0x1000); // attributes: supervisor level, read/write
        first_page_table[i].rw = 1;
        first_page_table[i].present = 1;
    }
    first_page_table[0xB7].val = 0xB8000; // store address
    first_page_table[0xB7].present = 1;
    first_page_table[0xB7].rw = 1;
    
    /* 4 KB page for video memory */
    first_page_table[0xB8].val = 0xB8000; // store address
    first_page_table[0xB8].present = 1;
    first_page_table[0xB8].rw = 1;

    /* 4 KB page for terminal 0 */
    first_page_table[0xB9].val = 0xB9000; // store address
    first_page_table[0xB9].present = 1;
    first_page_table[0xB9].rw = 1;

    /* 4 KB page for terminal 1 */
    first_page_table[0xBA].val = 0xBA000; // store address
    first_page_table[0xBA].present = 1;
    first_page_table[0xBA].rw = 1;

    /* 4 KB page for terminal 2 */
    first_page_table[0xBB].val = 0xBB000; // store address
    first_page_table[0xBB].present = 1;
    first_page_table[0xBB].rw = 1;
}

void system_call_paging(pcb_t* pcb_pointer)
{
    // Paging for user programs: (128 MB virtual -> 8MB physical + process_number*4 MB)
    page_directory[32].upper_address = (0x2 + pcb_pointer->process_num*0x1);
    page_directory[32].present = 1;
    page_directory[32].rw = 1;
    page_directory[32].us = 1;   // user bit set to 1
    page_directory[32].pcd = 1;  // enables caching
    page_directory[32].page_size = 1; // 4 MB page

    extern void flush_tlb(void);
    flush_tlb();
    }

void scheduler_paging(int process_num)
{
    // pcb_t* temp_pcb = (pcb_t*)(EIGHT_MB - EIGHT_KB*(terminal_num+1));
    page_directory[32].upper_address = (0x2 + process_num*0x1); 
    page_directory[32].present = 1;
    page_directory[32].rw = 1;
    page_directory[32].us = 1;   // user bit set to 1
    page_directory[32].pcd = 1;  // enables caching
    page_directory[32].page_size = 1; // 4 MB page

    /*if(terminal_num == terminal_on_display)
    {
        first_page_table[0xB8].val = 0xB8000; // Normal address
        first_page_table[0xB8].present = 1;
        first_page_table[0xB8].rw = 1;
    }
    else {
        first_page_table[0xB8].val = 0xB8000 + 0x1000*(terminal_num+1);
        first_page_table[0xB8].present = 1;
        first_page_table[0xB8].rw = 1;
    }*/

    extern void flush_tlb(void);
    flush_tlb();
}

void system_call_vidmap(void) 
{
    page_directory[33].present = 1;
    page_directory[33].rw = 1;
    page_directory[33].val |= (unsigned int)(vid_page_table); // Maps 132MB - 136MB block virtual to 0MB - 4MB physical
    page_directory[33].us = 1;   // user bit set to 1
    page_directory[33].pcd = 0;  // enables caching
    page_directory[33].page_size = 0; // 4 kb page
    
    vid_page_table[0].address = 0xB8; // store address
    vid_page_table[0].present = 1;
    vid_page_table[0].rw = 1;
    vid_page_table[0].us = 1; // store address

    extern void flush_tlb(void);
    flush_tlb();
}
