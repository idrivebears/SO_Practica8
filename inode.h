#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "common_data.h"

// Debe medir 64 bytes, importante es que el tamaño sea potencia de 2
typedef struct  {
	char name[18];
	unsigned int datetimecreat;			// 32 bits
	unsigned int datetimemodif;			// 32 bits
	unsigned int datetimelaacc; 		// 32 bits
	unsigned short uid;					// 16 bits
	unsigned short gid;					// 16 bits
	unsigned short perms;				// 16 bits
	unsigned int size;					// 32 bits
	unsigned short direct_blocks[10];	// 10 x 16 bits = 20 bytes
	unsigned short indirect;			// 16 bits
	unsigned short indirect2;			// 16 bits
} INODE;

// printf("%d\n",sizeof(struct INODE));

typedef struct {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} DATE;

int isinodefree(int inode);
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid);
int searchinode(char *filename);
int removeinode(int numinode);
int nextfreeinode();
int assigninode(int inode);
int unassigninode(int inode);
unsigned int datetoint(struct DATE date);
int inttodate(struct DATE *date,unsigned int val);
unsigned int currdatetimetoint();