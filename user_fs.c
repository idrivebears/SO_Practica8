#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesystem.h"

int main(int argc,char *argv[])
{
    printf("nextfreenode: %d\n", nextfreeinode());
    assigninode(0);
    assigninode(1);
    assigninode(2);
    assigninode(3);
    printf("nextfreenode: %d\n", nextfreeinode());
    unassigninode(0);
    unassigninode(1);
    unassigninode(2);
    printf("isinode 0 free> %d", isinodefree(0));
    printf("nextfreenode: %d\n", nextfreeinode());
    return 0;
}