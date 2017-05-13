#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "vdisk.h"

typedef struct  {
	int inuse;		// 0 cerrado, 1 abierto
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512]; //		
}OpenFile;

typedef int VDDIR;

typedef struct  
{
	char *d_name;
} vddirent;

VDDIR *vdopendir(char *path);

typedef struct{
	char drive_status;
	char chs_begin[3];
	char partition_type;
	char chs_end[3];
	int lba;
	int secs_partition;
}Partition; 

// Lo que vamos a escribir en el primer sector del disco
// Debe medir 512 bytes
typedef struct {
	char bootstrap_code[446];
	Partition partition[4];
	short boot_signature;
} MBR  ;
// printf("%d\n",sizeof(struct MBR));


// Debe medir 512 bytes
typedef struct  {
	char jump[4];			
	char nombre_particion[8];
	unsigned short sec_inicpart;				// 1 sector reservado
	unsigned char sec_res;						// 1 sector reservado
	unsigned char sec_mapa_bits_area_nodos_i;	// 1 sector
	unsigned char sec_mapa_bits_bloques;		// 6 sectores
	unsigned short sec_tabla_nodos_i;			// 3 sectores
	unsigned int sec_log_particion;				// 43199 sectores
	unsigned char sec_x_bloque;					// 2 sectores por bloque
	unsigned char heads;						// 8 superficies				
	unsigned char cyls;							// 200 cilindros
	unsigned char secfis;						// 27 sectores por track
	char restante[484];							// Código de arranque
}SecBootPart;

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
} Inode;

// printf("%d\n",sizeof(struct INODE));

typedef struct {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} Date;
 
//GLOBALS
#define TOTAL_INODES 24   			 ///checar
#define BLOCKSIZE 1024
#define SECTORSIZE 512

int isinodefree(int inode);		//working
int nextfreeinode();			//working
int assigninode(int inode);		//working
int unassigninode(int inode);	//working
void load_inodemap();			//working
void write_inodemap();			//working

int set_inode(int num, char *filename,unsigned short atribs, int uid, int gid);
int search_inode(char *filename);
int remove_inode(int numinode);
void write_inode();

unsigned int datetoint(Date date);
int inttodate(Date *date,unsigned int val);
unsigned int currdatetimetoint();

void readBlockMap();
void writeBlockMap();

int vdopen(char *filename,unsigned short mode);
int vdcreat(char *filename,unsigned short perms);
int vdunlink(char *filename);
int vdseek(int fd, int offset, int whence);
int vdwrite(int fd, char *buffer, int bytes);
int isblockfree(int block);
int nextfreeblock();
int assignblock(int block);
int unassignblock(int block);
int writeblock(int block,char *buffer);
int readblock(int block,char *buffer);
unsigned short *postoptr(int fd,int pos);
unsigned short *currpostoptr(int fd);
VDDIR *vdopendir(char *path);
vddirent *vdreaddir(VDDIR *dirdesc);
int vdclosedir(VDDIR *dirdesc);
int vdread(int fd, char *buffer, int bytes);

int vdclose(int fd);


int vdwriteseclog(int seclog, char *buffer);
int vdreadseclog(int seclog, char *buffer);
