#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "vdisk.h"
#include "filesystem.h"

#define LINESIZE 16
#define SECSIZE 512

// int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
// int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);

/*This program initializes disk partitions*/

int vdwriteseclog(int seclog, char *buffer)
{
	int head, sector, cylinder;
	sector = (seclog % SECTORS) + 1;
	head = (seclog / SECTORS) % HEADS;
	cylinder = seclog / (SECTORS * HEADS);

	return vdwritesector(0, head, cylinder, sector, 1, buffer);
}

int vdreadseclog(int seclog, char *buffer)
{
	int head, sector, cylinder;
	sector = (seclog % SECTORS) + 1;
	head = (seclog / SECTORS) % HEADS;
	cylinder = seclog / (SECTORS * HEADS);

	return vdreadsector(0, head, cylinder, sector, 1, buffer);

}

int main(int argc,char *argv[])
{

    // Write MBR to first memory sector.
    int drive = 0;
    if(argc == 2)  {
        drive = atoi(argv[1]);
    }

    printf("Initializing disk %d\n", drive);

    //Initialize partitions
    Partition partition_entries[4];
    int i, j;
    for(i = 0; i < 4; i++) {
        partition_entries[i].drive_status = 0xFF;
        partition_entries[i].partition_type = 0xFF;
        partition_entries[i].lba = 0xFFFFFFFF;
        partition_entries[i].secs_partition = 0xFFFFFFFF;

        for(j = 0; j < 3; j++) {
            partition_entries[i].chs_begin[j] = 0xFF;
            partition_entries[i].chs_end[j] = 0xFF;
        }

    }

    // Initialize MBR values
    MBR init_mbr;
    for(i = 0; i < 446; i++) {
        init_mbr.bootstrap_code[i] = 0xFF;
    }
    init_mbr.boot_signature = 0xFFFF;
    
    for(i = 0; i < 4; i ++)
        init_mbr.partition[i] = partition_entries[i];
    
    // Copy MBR to buffer
    char * buffer = malloc((int)sizeof(MBR));
    memcpy(buffer, &init_mbr, (int) sizeof(MBR));
    //Write to memory sector 0
    int result = vdwriteseclog(0, buffer);
    printf("MRB written.\n");
    free(buffer);

    //Write SECBOOT to disk

    SecBootPart secboot;
    memset(&secboot.jump[0], 0xFFFF, sizeof(secboot.jump));
    strcpy(&secboot.nombre_particion[0], "Victoria");
    secboot.sec_inicpart = 1;
    secboot.sec_res = 1;
    secboot.sec_mapa_bits_area_nodos_i = 1;
    secboot.sec_mapa_bits_bloques = 6;
    secboot.sec_tabla_nodos_i = 3;
    secboot.sec_log_particion = 43199;
    secboot.sec_x_bloque = 2;
    secboot.heads = HEADS;
    secboot.cyls = CYLINDERS;
    secboot.secfis = SECTORS;

    buffer = malloc((int)sizeof(SecBootPart));
    memcpy(buffer, &secboot, (int)sizeof(SecBootPart));

    result = vdwriteseclog(1, buffer);
    printf("Secboot written.\n");
    free(buffer);

    return 0;
}
