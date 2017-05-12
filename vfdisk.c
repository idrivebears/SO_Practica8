#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"
#include "filesystem.h"

#define LINESIZE 16
#define SECSIZE 512

int main(int argc,char *argv[])
{
    int drive = 0;
    if(argc == 2) 
    {
        drive = atoi(argv[1]);
    }

    printf("Initializing disk %d", drive);



    return 0;
}
