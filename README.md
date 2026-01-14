## Features

Implemented:
* Mocked disk driver, being a region in ram treated as a ramdisk 
* Inode allocation and deallocation using bitmaps
* Data Block allocation and deallocation using bitmaps
* Root directory initialisation
* Pathname parsing
* Pathname to inode number

WIP:
* File allocation
* Directory Allocation
* File Reading
* File Writting

## File System Organisation

SuperBlock | Inode Bitmap | Inode Zone | Data Block Bitmap | Data Block Zone

A zone is a contigous region on disk containing a collection of a data structure. The inode zone contains inodes, the data block zone contains disk data blocks.
