#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"
#include "filesystem.h"

#define SECSIZE 512
#define LINESIZE 16

int main(int argc,char *argv[])
{
	unsigned char buffer[SECSIZE];
	int offset;
	int i,j,r;
    int seclog;
	unsigned char c;

	if(argc==2)
	{	
        seclog=atoi(argv[1]);
		printf("Desplegando de disco 0.vd"); 
		
	}
	else	
	{
		fprintf(stderr,"Error en los argumentos\n");
		exit(1);
	}

	if(vdreadseclog(seclog, buffer)==-1)
	{
		fprintf(stderr,"Error al abrir disco virtual\n");
		exit(1);
	}

	for(i=0;i<SECSIZE/LINESIZE;i++)
	{
		printf("\n %3X -->",i*LINESIZE);
		for(j=0;j<LINESIZE;j++)
		{
			c=buffer[i*LINESIZE+j];
			printf("%2X ",c);
		}
		printf("  |  ");
		for(j=0;j<LINESIZE;j++)
		{
			c=buffer[i*LINESIZE+j]%256;
			if(c>0x1F && c<127)
				printf("%c",c);
			else
				printf(".");
		}
	}
	printf("\n");
}

