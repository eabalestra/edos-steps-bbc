//=============================================================================
// EFS: An embedded (in kernel image) filesystem
//=============================================================================
#include "efs.h"
#include "klib.h"

// defined 
extern struct file efs_files_table[];

// return bytes read
struct file* efs_file(char *file_name)
{
    for (int i=0; efs_files_table[i].name; i++) {
        if (strcmp(efs_files_table[i].name, file_name) == 0)
            return efs_files_table + i;
    }
    return 0;
}
