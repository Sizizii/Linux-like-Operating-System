#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "paging.h"

#define BOOT_BLOCK_RESERVED_SIZE    52
#define DENTRY_RESERVED_SIZE        24
#define MAXIMUM_FILENAME_LENGTH     32
#define MAXIMUM_DENTRY_NUM          63
#define TOTAL_BLOCK_NUM             1024

// directory entry structure
typedef struct Dentry {
    uint8_t     filename[MAXIMUM_FILENAME_LENGTH];
    uint32_t    filetype;
    uint32_t    num_inode;
    uint8_t     reserved[DENTRY_RESERVED_SIZE];
} dentry_t;

// boot block structure
typedef struct BootBlock {
    uint32_t    num_dentry;
    uint32_t    num_inode;
    uint32_t    num_data_block;
    uint8_t     reserved[BOOT_BLOCK_RESERVED_SIZE];
    dentry_t    dentries[MAXIMUM_DENTRY_NUM];
} boot_block_t;

// inode structure
typedef struct Inode {
    uint32_t    length;
    uint32_t    num_data_block[TOTAL_BLOCK_NUM - 1];
} inode_t;

// ! data block structure. unused
// typedef struct DataBlock {
//     uint32_t data[FOURK];
// } data_block_t;

// the pointer to the boot block
boot_block_t*   boot_block;
// the pointer to the currently open file entry
dentry_t        current_dentry;

// initialize the filesystem
void filesystem_init(uint32_t boot_block_address);


// file operation functions
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);


// directory operation functions
int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);


// helper functions
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
