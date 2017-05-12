#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesystem.h"

int main(int argc,char *argv[])
{
    char *buffer;
    int re = vdwriteseclog(0, buffer);
    printf("done");

    return 0;
}