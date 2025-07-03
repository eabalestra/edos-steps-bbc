//=============================================================================
// Embedded file system (efs)
//=============================================================================

#pragma once

// File types
#define EFS_FILE_PROGRAM    0
#define EFS_FILE_DATA       1

struct file {
    char            *name;
    unsigned char   type;
    unsigned char   *data;
    unsigned int    length;
};

extern struct file* efs_file(char *file_name);
