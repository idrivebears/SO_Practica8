#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesystem.h"

int main(int argc,char *argv[])
{
    // int set_inode(int num, char *filename,unsigned short atribs, int uid, int gid);

    printf("isinode 0 free:%d\n", isinodefree(0));
    set_inode(1, "EstoyAqui", 1, 2, 3);
    printf("isinode 0 free:%d\n", isinodefree(0));
    printf("search found: %d\n", search_inode("EstoyAqui"));
    //remove_inode(0);
    printf("isinode 0 free:%d\n", isinodefree(0));

    return 0;
}