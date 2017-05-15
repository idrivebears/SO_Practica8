#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesystem.h"

int main(int argc,char *argv[])
{
     int vdcreat(char *filename,unsigned short perms);

    int fid = vdcreat("wekop", 1);
    char *mymessage = "1Este es un mensaje para todos los ninos de hoy.";

    printf("size %d\n", fid);
    
    int a = vdwrite(fid, mymessage, 1024);
    mymessage = "2Este es un mensaje para todos los ninos de hoy.";
    a = vdwrite(fid, mymessage, 1024);
    
    
    char buffer[48];
    int b = vdopen("wekop", READONLY);
    printf("pr\n");
    int c = vdread(b, buffer, 48);
    printf("Hola %d, %d, %s\n",b,c,buffer);

    return 0;
}