struct PARTITION {
	char drive_status;
	char chs_begin[3];
	char partition_type;
	char chs_end[3];
	int lba;
	int secs_partition;
} 

// Lo que vamos a escribir en el primer sector del disco
// Debe medir 512 bytes
struct MBR {
	char bootstrap_code[446];
	struct PARTITION partition[4];
	short boot_signature;
}
// printf("%d\n",sizeof(struct MBR));


// Debe medir 512 bytes
struct SECBOOTPART {
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
};
// printf("%d\n",sizeof(struct SECBOOT));


// Debe medir 64 bytes, importante es que el tamaño sea potencia de 2
struct INODE {
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
};

// printf("%d\n",sizeof(struct INODE));

struct DATE {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};
