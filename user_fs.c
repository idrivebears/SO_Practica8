#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesystem.h"

int main(int argc,char *argv[])
{
    // int vdcreat(char *filename,unsigned short perms);

    int fid = vdcreat("Weko", 1);
    char *mymessage = malloc(2048);
    mymessage = "Este es un mensaje para todos los ninos de hoy.";
    int a = vdwrite(fid, mymessage, sizeof(mymessage));
    printf("Hola %d\n",a);

    return 0;
}