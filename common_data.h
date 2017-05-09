#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct  {
	int inuse;		// 0 cerrado, 1 abierto
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512]; //		
}OPENFILES;

typedef int VDDIR;

typedef struct  
{
	char *d_name;
} vddirent;

struct vddirent *vdreaddir(VDDIR *dirdesc);
VDDIR *vdopendir(char *path);

typedef struct{
	char drive_status;
	char chs_begin[3];
	char partition_type;
	char chs_end[3];
	int lba;
	int secs_partition;
}PARTITION; 

// Lo que vamos a escribir en el primer sector del disco
// Debe medir 512 bytes
typedef struct {
	char bootstrap_code[446];
	struct PARTITION partition[4];
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
}SECBOOTPART;

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
struct vddirent *vdreaddir(VDDIR *dirdesc);
int vdclosedir(VDDIR *dirdesc);
int vdread(int fd, char *buffer, int bytes);
int vdclose(int fd);