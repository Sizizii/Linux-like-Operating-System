#include "filesystem.h"
#include "lib.h"
#include "system_call.h"
#define FOUR_KB     (1024*4)
// the index of the entry to be read in the next call
static int dir_readnum = 0;

/*
 * filesystem_init: init the filesystem
 *
 * Input: the address of boot block
 * Output: none
 * Return: none
 * Side effect: boot_block is set
 * 
*/
void filesystem_init(uint32_t boot_block_address) {
    printf("%d\n", boot_block_address);
    boot_block = (boot_block_t*)boot_block_address;
    return;
}

/*
 * file_open: open the file indicated by file name
 *
 * Input: file name
 * Output: none
 * Return: -1 on fail.
 *          0 on success.
 * Side effect: set current_dentry
 * 
*/
int32_t file_open(const uint8_t* filename) {
    return read_dentry_by_name(filename, &current_dentry);
}

/*
 * file_close: close the file
 *
 * Input: file descriptor
 * Output: none
 * Return: -1 on fail.
 *          0 on success.
 * Side effect: none
 * 
*/
int32_t file_close(int32_t fd) {
    if (fd == 0 || fd == 1) {
        return -1;
    }
    return 0;
}

/*
 * file_read: read the content of the file to buffer
 *
 * Input: file descriptor, buffer and length of buffer
 * Output: none
 * Return: -1 on fail.
 *          0 on success.
 * Side effect: fill the buffer with the content of the file
 * 
*/
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    file_descriptor_t* fd_pointer = &(pcb_array[curr_pid]->fd_array[fd]);
    int32_t length;
    length = read_data(fd_pointer->inode_num, fd_pointer->file_position, buf, nbytes);    
    fd_pointer->file_position=fd_pointer->file_position+length;
    return length;
}


/*
 * file_read: write the file
 *
 * Input: file descriptor, buffer and length of buffer
 * Output: none
 * Return: always -1, since this is a read-only file system
 * Side effect: none
 * 
*/
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
        return -1;
}


/*
 * dir_open: this function is to open the directory
 *
 * Input: the pointer of the file name
 * Output: none
 * Return: 0 if success, -1 if fail
 * Side effect: if success, the file will be opened
 * 
*/
int32_t dir_open(const uint8_t* filename)
{
    dentry_t temp;
    dir_readnum = 0;
    return read_dentry_by_name(filename, &temp);
}


/*
 * dir_read: this function is to loop the files information in 
 * the file directory until all files are read
 *
 * Input: buf the pointer to a buffer that we write the information of the
 * entry in the directory. fd and nbytes are ignored
 * Output: the information will be written into the buffer
 * Return: -1 if fail, 0 if success
 * Side effect: none
 * 
*/
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
    if (buf == NULL)
        return -1;
    dentry_t fetch;
    uint8_t* dentry_address;
    int len;
    // out of range
    if(dir_readnum >= boot_block->num_dentry) {
        return 0;
    }
    // first fill up the buffer
    if(0 == read_dentry_by_index(dir_readnum,&fetch)){
        dentry_address = fetch.filename;
        len = strlen((int8_t*)dentry_address);
        // copy the file name into the buffer
        strncpy((int8_t*)buf, (int8_t*)dentry_address, nbytes);
        dir_readnum++;
        return nbytes <= len ? nbytes : len;
    }

    return -1;
}



/*
 * dir_close: this function is to close the directory
 *
 * Input: the pointer of the file name
 * Output: none
 * Return: 0 if success, -1 if fail
 * Side effect: if success, the file will be opened
 * 
*/
int32_t dir_close(int32_t fd)
{
    return 0;
}


/*
 * dir_write: this function is to loop the files information in 
 * the file directory until all files are write
 *
 * Input: buf the pointer to a buffer that we write the information of the
 * entry in the directory. fd and nbytes are ignored
 * Output: the information will be written into the buffer
 * Return: -1 if fail, 0 if success
 * Side effect: none
 * 
*/
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
    // our filesystem doesn't support write
    return -1;
}

/*
 * read_dentry_by_name: this function is to read the file with the given name
 * 
 * Input: the pointer of the name of the file, a structure to fill in the information
 * of the file if it is found
 * Output: the information will be written into the pointer
 * Return: -1 if fail, 0 if success
 * Side effect: none
 * 
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
    dentry_t dentry_info; 
    int i;
    int name_length;
    uint8_t* dentry_name;
    // if the pointer is invalid or the dentry is null, return -1
    if (fname == NULL || dentry == NULL || strlen((int8_t*)fname) > MAXIMUM_FILENAME_LENGTH){
        return -1;
    }
    name_length = strlen((int8_t*)fname); 
    // loop over the directory
    for(i = 0; i < boot_block->num_dentry; i++){
        dentry_info = boot_block->dentries[i];
        dentry_name = dentry_info.filename;
        // compare the file name
        if (strncmp((int8_t*)fname,(int8_t*)dentry_name,MAXIMUM_FILENAME_LENGTH) == 0){
            *dentry = dentry_info;
            return 0;
        }

    } 
    return -1;
}

/*
 * read_dentry_by_index: this function is to read the file with the given index
 * 
 * Input: the index of the file in directory, a structure to fill in the information
 * of the file if it is found
 * Output: the information will be written into the pointer
 * Return: -1 if fail, 0 if success
 * Side effect: none
 * 
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
    dentry_t dentry_info;
    // invalid index
    if(index < 0 || index >= boot_block->num_dentry || dentry == NULL){
        return -1;
    }

    dentry_info = boot_block->dentries[index];
    *dentry = dentry_info;
    return 0;
}
    

/*
 * read_data: read the data to the buffer
 * 
 * Input: inode index, offset, buffer and length of the read data
 * Output: none
 * Return: successfully read length. If -1, read failed
 * Side effect: fill the buffer with the read data
 * 
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    if (buf == NULL || inode < 0 || inode >= boot_block->num_inode) {
        // check the input. If buf pointer is NULL or inode is not within
        // range, return 0.
        return 0;
    }
    // pointer to the inode
    inode_t* current_inode_ptr = (inode_t*)boot_block + inode + 1;
    // if no content should be read, return 0
    if (offset > current_inode_ptr->length) {
        return 0;
    }
    // if the length is too long, trucat it.
    if (length + offset > current_inode_ptr->length) {
        length = current_inode_ptr->length - offset;
    }
    // calculate the total block num
    const int32_t block_num = current_inode_ptr->length / FOUR_KB + (int32_t)((current_inode_ptr->length % FOUR_KB) > 0);
    // calculate the start block id
    const int32_t start_block_id = offset / FOUR_KB;
    // calculate the offset inside the start block
    const int32_t start_block_offset = offset % FOUR_KB;
    int i;
    // if any block is invalid, return -1
    for (i = 0; i < block_num; i++) {
        if (current_inode_ptr->num_data_block[i] >= boot_block->num_data_block) {
            return -1;
        }
    }

    uint8_t* dest = buf;
    uint32_t remain_length = length;
    // read the data inside one black at a time
    for (i = start_block_id; i < block_num && remain_length > 0; i++) {
        const int32_t block_offset = (i == start_block_id) ? start_block_offset : 0;
        uint32_t n = (remain_length >= FOUR_KB || (remain_length < FOUR_KB && block_offset + remain_length > FOUR_KB)) ? FOUR_KB - block_offset : remain_length;
        uint8_t* src = (uint8_t*)boot_block + (boot_block->num_inode + 1 + current_inode_ptr->num_data_block[i]) * FOUR_KB + block_offset;
        // use memcpy to copy the whole desired data inside the block to the buffer
        memcpy(dest, src, n);
        dest += n;
        remain_length -= n;
    }
    return length;
}
