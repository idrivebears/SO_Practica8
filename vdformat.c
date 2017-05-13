#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesystem.h"

int main(int argc,char *argv[])
{   

    //Escribir mapa de nodos i
    char buffer[512];
    memset(&buffer[0], 0x00, sizeof(buffer));

    memcpy(buffer, &buffer, sizeof(buffer));
    int re = vdwriteseclog(2, buffer);

    re += vdwriteseclog(3, buffer);
    re += vdwriteseclog(4, buffer);
    re += vdwriteseclog(5, buffer);
    if(re == 4)
        printf("Disk format completed successfully.\n");
    else
        printf("DISK FORMAT ERROR.\n [%d]\n", re);

    
    printf(" ");
    
    return 0;
}