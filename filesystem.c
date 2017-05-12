#include "filesystem.h"

//Funciones del Sistema de Archivos
int openfiles_inicializada = 0;
<<<<<<< HEAD
Openfiles openfiles[24];
=======
OPENFILES openfiles[24];
Inode inode[24];					 ///checar
SecBootPart secboot; 				 ///checar
int secboot_en_memoria = 1;		     ///checar	
char mapa_bits_nodos_i[3] = {0,0,0}; ///checar

>>>>>>> d0c04a5bc1eb1c89374388101c89c63e817b505c
int vdopen(char *filename,unsigned short mode)
{
	// Les toca hacerla a ustedes
}


// Esta función se va a usar para crear una función en el 
// sistema de archivos
int vdcreat(char *filename,unsigned short perms)
{
	int numinode;
	int i;

	// Ver si ya existe el archivo
	numinode=searchinode(filename);
	if(numinode==-1) // Si el archivo no existe
	{
		// Buscar un inodo en blanco en el mapa de bits (nodos i)
		numinode=nextfreeinode(); // Recorrer la tabla 
									// de nodos i buscando
									// un inodo que esté
									// libre
		if(numinode==-1) // La tabla de nodos-i está llena
		{
			return(-1); // No hay espacio para más archivos
		}
	} else	// Si el archivo ya existe elimina el inodo
		removeinode(numinode);


	// Escribir el archivo en el inodo encontrado
// En un inodo de la tabla, escribe los datos del archivo
	setninode(numinode,filename,perms,getuid(),getgid());
		
	// assigninode(numinode);

	// Establecer el archivo como abierto
	if(!openfiles_inicializada)
	{
		// La primera vez que abrimos un archivo, necesitamos
		// inicializar la tabla de archivos abiertos
		for(i=3;i<16;i++)
		{
			openfiles[i].inuse=0;
			openfiles[i].currbloqueenmemoria=-1;
		}
		openfiles_inicializada=1;
	}

	// Buscar si hay lugar en la tabla de archivos abiertos
	// Si no hay lugar, regresa -1
	i=3;
	while(openfiles[i].inuse && i<16)
		i++;

	if(i>=16)		// Llegamos al final y no hay lugar
		return(-1);

	openfiles[i].inuse=1;	// Poner el archivo en uso
	openfiles[i].inode=numinode;  // Indicar que inodo es el
							// del archivo abierto
	openfiles[i].currpos=0; // Y la posición inicial
// del archivo es 0
	return(i);
}


// Borrar un archivo del sistema de archivos
int vdunlink(char *filename)
{
	int numinode;
	int i;

	// Busca el inodo del archivo
	numinode=searchinode(filename);
	if(numinode==-1)
		return(-1); // No existe

	removeinode(numinode);
}

// Mover el puntero del archivo a la posición indicada
// a partir de: el inicio si whence es 0, de la posición 
// actual si whence es 1, o a partir del final si whence es 2
int vdseek(int fd, int offset, int whence)
{
	unsigned short oldblock,newblock;

	// Si no está abierto regresa error
	if(openfiles[fd].inuse==0)
		return(-1);

	// La función currpostoptr es una función que a partir de 
	// la posición actual del apuntador del archivo, 
// me regresa la dirección de memoria en el nodo i o en 
	// en el bloque de apuntadores que contiene el bloque
	// donde está el puntero del archivo
	oldblock=*currpostoptr(fd);
		
	if(whence==0) // A partir del inicio
	{
		if(offset<0 || 
		   openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size)
			return(-1);
		openfiles[fd].currpos=offset;

	} else if(whence==1) // A partir de la posición actual
	{
		if(openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size ||
		   openfiles[fd].currpos+offset<0)
			return(-1);
		openfiles[fd].currpos+=offset;

	} else if(whence==2) // A partir del final
	{
		if(offset>inode[openfiles[fd].inode].size ||
		   openfiles[fd].currpos-offset<0)
			return(-1);
		openfiles[fd].currpos=inode[openfiles[fd].inode].size-offset;
	} else
		return(-1);

	// Verificamos si la nueva posición del puntero es un
	// bloque diferente al que estábamos,si es así hay que
	// cargar ese bloque a memoria
	newblock=*currpostoptr(fd);
	
	// Después de mover el puntero, ahora me cambié a otro
	// bloque?
	if(newblock!=oldblock)
	{
		// Escribir el bloque viejo
		writeblock(oldblock,openfiles[fd].buffer);
		// Leer el bloque nuevo
		readblock(newblock,openfiles[fd].buffer);
		// Indicar el nuevo bloque como bloque actual
		// en la tabla de archivos abiertos
		openfiles[fd].currbloqueenmemoria=newblock;
	}

	// Regresamos la posición actual del archivo
	return(openfiles[fd].currpos);
}

// Esta es la función más difícil, 
int vdwrite(int fd, char *buffer, int bytes)
{
	int currblock;
	int currinode;
	int cont=0;
	int sector;
	int i;
	int result;
	unsigned short *currptr;

	// Si no está abierto, regresa error
	if(openfiles[fd].inuse==0)
		return(-1);

	currinode=openfiles[fd].inode;

	// Copiar byte por byte del buffer que recibo como 
// argumento al buffer del archivo
	while(cont<bytes)
	{
		// Obtener la dirección de donde está el bloque que corresponde
		// a la posición actual
		currptr=currpostoptr(fd);
		if(currptr==NULL)
			return(-1);
	
		currblock=*currptr;

		// Si el bloque está en blanco, dale uno
		if(currblock==0)
		{
			currblock=nextfreeblock();
			// El bloque encontrado ponerlo en donde
			// apunta el apuntador al bloque actual
			*currptr=currblock;
			assignblock(currblock);	// Asignarlo en el mapa de bits
			
			// Escribir el sector de la tabla de nodos i
			// En el disco
			sector=(currinode/4)*4;
			result=vdwriteseclog(inicio_nodos_i+sector,&inode[sector*8]);
		}

		// Si el bloque de la posición actual no está en memoria
		// Lee el bloque al buffer del archivo
		if(openfiles[fd].currbloqueenmemoria!=currblock)
		{
			// Leer el bloque actual hacia el buffer que
			// está en la tabla de archivos abiertos
			readblock(currblock,openfiles[fd].buffer);			
			// Actualizar en la tabla de archivps abiertos
			// el bloque actual
			openfiles[fd].currbloqueenmemoria=currblock;
		}

		// Copia el caracter al buffer
		openfiles[fd].buffer[openfiles[fd].currpos%TAMBLOQUE]=buffer[cont];

		// Incrementa posición actual del actual
		openfiles[fd].currpos++;

		// Si la posición es mayor que el tamaño, modifica el tamaño
		if(openfiles[fd].currpos>inode[currinode].size)
			inode[openfiles[fd].inode].size=openfiles[fd].currpos;

		// Incrementa el contador
		cont++;

		// Si se llena el buffer, escríbelo
		if(openfiles[fd].currpos%TAMBLOQUE==0)
			writeblock(currblock,openfiles[fd].buffer);
	}
	return(cont);
} 


// ******************************************************************************
// Para el mapa de bits del área de de datos
// ******************************************************************************

// Usando la información del mapa de bits del area de datos, saber si un bloque está libre o no
int isblockfree(int block)
{
	int offset=block/8; // Número de byte en el mapa
	int shift=block%8; // Número de bit en el byte
	int result;
	int i;

	// Determinar si tenemos el sector de boot de la partición en memoria
	if(!secboot_en_memoria)
	{
		result=vdreadseclog(1, (char *) &secboot);
		secboot_en_memoria=1;
	}

	// Calcular el sector lógico donde está el mapa de bits de los bloques
	mapa_bits_bloques= secboot.sec_inicpart+secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
	
	// Verificar si ya está en memoria, si no, cargarlo
	if(!blocksmap_en_memoria)
	{
		// Cargar todos los sectores que corresponden al 
		// mapa de bits
		for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	}

	if(blocksmap[offset] & (1<<shift))
		return(0);	// Si el bit está en 1, regresar 0 (no está libre)
	else
		return(1);	// Si el bit está en 0, regresar 1 (si está libre)
}	

// Usando el mapa de bits, buscar el siguiente bloque libre
int nextfreeblock()
{
	int i,j;
	int result;

	// Determinar si tenemos el sector de boot de la partición en memoria
	if(!secboot_en_memoria)
	{
		result=vdreadseclog(1, (char *) &secboot);
		secboot_en_memoria=1;
	}

	// Calcular el sector lógico donde está el mapa de bits de los bloques
	mapa_bits_bloques= secboot.sec_inicpart+secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
	
	// Verificar si ya está en memoria, si no, cargarlo
	if(!blocksmap_en_memoria)
	{
		// Cargar todos los sectores que corresponden al 
		// mapa de bits
		for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	}

	i=0;
	while(blocksmap[i]==0xFF && i<secboot.sec_mapa_bits_bloques*512)
		i++;

	if(i<secboot.sec_mapa_bits_bloques*512)
	{
		j=0;
		while(blocksmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j);
	}
	else
		return(-1);

		
}
// Asignar un bloque en 1 en el mapa de bits, lo cual significa que estaría ya ocupado.
int assignblock(int block)
{
	int offset=block/8;
	int shift=block%8;
	int result;
	int i;
	int sector;

	// Determinar si tenemos el sector de boot de la partición en memoria
	if(!secboot_en_memoria)
	{
		result=vdreadseclog(1, (char *) &secboot);
		secboot_en_memoria=1;
	}

	// Calcular el sector lógico donde está el mapa de bits de los bloques
	mapa_bits_bloques= secboot.sec_inicpart+secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
	
	// Verificar si ya está en memoria, si no, cargarlo
	if(!blocksmap_en_memoria)
	{
		// Cargar todos los sectores que corresponden al 
		// mapa de bits
		for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	}

	blocksmap[offset]|=(1<<shift);

	sector=(offset/512)*512;
	vdwriteseclog(mapa_bits_bloques+sector,blocksmap+sector*512);
	//for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}

// Poner en 0 el bit que corresponde a un bloque en el mapa de bits, esto equivale a decir que el bloque está libre
int unassignblock(int block)
{
	int offset=block/8;
	int shift=block%8;
	int result;
	char mask;
	int sector;
	int i;

	// Determinar si tenemos el sector de boot de la partición en memoria
	if(!secboot_en_memoria)
	{
		result=vdreadseclog(1, (char *) &secboot);
		secboot_en_memoria=1;
	}

	// Calcular el sector lógico donde está el mapa de bits de los bloques
	mapa_bits_bloques= secboot.sec_inicpart+secboot.sec_res+secboot.sec_mapa_bits_nodos_i;
	
	// Verificar si ya está en memoria, si no, cargarlo
	if(!blocksmap_en_memoria)
	{
		// Cargar todos los sectores que corresponden al 
		// mapa de bits
		for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
			result=vdreadseclog(mapa_bits_bloques+i,blocksmap+i*512);
		blocksmap_en_memoria=1;
	}

	blocksmap[offset]&=(char) ~(1<<shift);

	sector=(offset/512)*512;
	vdwriteseclog(mapa_bits_bloques+sector,blocksmap+sector*512);
	// for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}
	

// **********************************************************************************
// Lectura y escritura de bloques
// **********************************************************************************

int writeblock(int block,char *buffer)
{
	int result;
	int i;

	// Determinar si el sector de boot de la partición está en memoria, si no está en memoria, cargarlo
	if(!secboot_en_memoria)
	{
		// Leer el sector lógico 1, donde está
		// el sector de boot de la partición
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}

	// Inicio area de datos =  setor de inicio de la partición +
	// 							sectores reservados +
    //							sectores mapa de bits area de nodos i+	
	//							sectores mapa de bits area de datos+	
	//							sectores area nodos i (dr)
	inicio_area_datos=secboot.sec_inicpart+secboot.sec_res++secboot.sec_mapa_bits_nodosi +secboot.sec_mapa_bits_bloques+secboot.sec_tabla_nodos_i;

	// Escribir todos los sectores que corresponden al 
	// bloque
	for(i=0;i<secboot.sec_x_bloque;i++)
		vdwriteseclog(inicio_area_datos+(block-1)*secboot.sec_x_bloque+i,buffer+512*i);
	return(1);	
}

int readblock(int block,char *buffer)
{
	int result;
	int i;

	// Determinar si el sector de boot de la partición está en memoria, si no está en memoria, cargarlo
	if(!secboot_en_memoria)
	{
		// Leer el sector lógico 1, donde está
		// el sector de boot de la partición
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}

	// Inicio area de datos =  setor de inicio de la partición +
	// 							sectores reservados +
    //							sectores mapa de bits area de nodos i+	
	//							sectores mapa de bits area de datos+	
	//							sectores area nodos i (dr)
	inicio_area_datos=secboot.sec_inicpart+secboot.sec_res++secboot.sec_mapa_bits_nodosi +secboot.sec_mapa_bits_bloques+secboot.sec_tabla_nodos_i;

	for(i=0;i<secboot.sec_x_bloque;i++)
		vdreadseclog(inicio_area_datos+(block-1)*secboot.sec_x_bloque+i,buffer+512*i);
	return(1);	
}



/* Esta función se utilizará con las funciones seek, write y read, con estas operaciones el nodo i 
del archivo debe estar cargado en memoria RAM. Por ejemplo si escribimos información en el archivo 
se le empiezan a asignar bloques, ¿cuáles?, los que encontremos disponibles, y esos bloques 
disponibles es necesario que los escribamos en el nodo i que está en memoria, y con esta función 
sabemos cuál es la dirección donde debe ir ese apuntador*/

unsigned short *postoptr(int fd,int pos)
{
	int currinode;
	unsigned short *currptr;
	unsigned short indirect1;

	currinode=openfiles[fd].inode;

	// Está en los primeros 10 K
	if((pos/1024)<10)
		// Está entre los 10 apuntadores directos
		currptr=&inode[currinode].blocks[pos/1024];
	else if((pos/1024)<522)
	{
		// Si el indirecto está vacío, asígnale un bloque
		if(inode[currinode].indirect==0)
		{
			// El primer bloque disponible
			indirect1=nextfreeblock();
			assignblock(indirect1); // Asígnalo
			inode[currinode].indirect=indirect1;
		} 
		currptr=&openfiles[fd].buffindirect[pos/1024-10];
	}
	else
		return(NULL);

	return(currptr);
}



unsigned short *currpostoptr(int fd)
{
	unsigned short *currptr;

	currptr=postoptr(fd,openfiles[fd].currpos);

	return(currptr);
}

//Funciones para el manejo de directores (sólo directorio ráiz)

VDDIR dirs[2]={-1,-1};
struct vddirent current;


VDDIR *vdopendir(char *path)
{
	int i=0;
	int result;

	if(!secboot_en_memoria)
	{
		result=vdreadseclog(0,(char *) &secboot);
		secboot_en_memoria=1;
	}

//Aquí se debe calcular la variable inicio_nodos_i con los datos que están en el sector de boot de la partición

	// Determinar si la tabla de nodos i está en memoria
// si no está en memoria, hay que cargarlos
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(inicio_nodos_i+i,&inode[i*8]);

		nodos_i_en_memoria=1;
	}

	if(strcmp(path,".")!=0)
		return(NULL);

	i=0;
	while(dirs[i]!=-1 && i<2)
		i++;

	if(i==2)
		return(NULL);

	dirs[i]=0;

	return(&dirs[i]);	
}


// Lee la siguiente entrada del directorio abierto
struct vddirent *vdreaddir(VDDIR *dirdesc)
{
	int i;

	int result;
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(inicio_nodos_i+i,&inode[i*8]);

		nodos_i_en_memoria=1;
	}

	// Mientras no haya nodo i, avanza
	while(isinodefree(*dirdesc) && *dirdesc<4096)
		(*dirdesc)++;


	// Apunta a donde está el nombre en el inodo	
	current.d_name=inode[*dirdesc].name;

	(*dirdesc)++;

	if(*dirdesc>=24)
		return(NULL);
	return( &current);	
}



int vdclosedir(VDDIR *dirdesc)
{
	(*dirdesc)=-1;
}


int vdread(int fd, char *buffer, int bytes)
{
	// Ustedes la hacen
}

int vdclose(int fd)
{
	// Ustedes la hacen
}

// **********************************************************
// Lectura y escritura de bloques
// **********************************************************

/*int writeblock(int num_block,char *buffer)
{
	// Les toca hacerla
	// Un bloque es un grupo de sectores lógicos
	// Determinar cuál es el primer sector lógico que
	// le corresponde a ese número de bloque, para eso
	// se necesitan los datos que están en el sector de
	// boot.
	// La operación de escritura se hace sobre el sector
	// lógico calculado y los siguientes sectores lógicos
	// que corresponden a ese bloque.
}

int readblock(int num_block,char *buffer)
{
	// Les toca hacerla
	// Un bloque es un grupo de sectores lógicos
	// Determinar cuál es el primer sector lógico que
	// le corresponde a ese número de bloque, para eso
	// se necesitan los datos que están en el sector de
	// boot.
	// La operación de lectura se hace sobre el sector
	// lógico calculado y los siguientes sectores lógicos
	// que corresponden a ese bloque.

}
*/


//**********************************************************************
// INODES funcs
//**********************************************************************
int isinodefree(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;

	int inodesmap_en_memoria = 0; 		///checar
	unsigned short inicio_nodos_i;		///checar


	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}
	inicio_nodos_i = secboot.sec_inicpart +secboot.sec_res; 	
//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,&inode);		///checar
		inodesmap_en_memoria=1;
	}


	if(mapa_bits_nodos_i[offset] & (1<<shift))
		return(0); // El nodo i ya está ocupado
	else
		return(1); // El nodo i está libre
}	
// Escribir en la tabla de nodos-I del directorio raíz, los datos de un archivo
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid)
{
	int i;

	int result;

	int nodos_i_en_memoria = 0;									///checar
	int inicio_nodos_i = 0;										///checar
	inicio_nodos_i = secboot.sec_inicpart +secboot.sec_res; 	///checar

	//Antes de continuar debe cargarse en memoria el sector lógico 1 que es el sector de boot de la partición.
	//Con los datos que están ahí, calcular:
	//El sector lógico donde empieza la tabla de nodos-i
	//También vamos a usar el número de sectores que tiene la tabla de nodos-i


	// Si la tabla de nodos-i no está en memoria, 
// hay que cargarla a memoria
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(inicio_nodos_i+i,&inode[i*4]);

		nodos_i_en_memoria=1;
	}

	// Copiar el nombre del archivo en el nodo i
	strncpy(inode[num].name,filename,18);

	// Asegurando que el último caracter es el terminador (cero)
	if(strlen(inode[num].name)>17)
	 	inode[num].name[17]='\0';

	// Poner en el nodo I las fechas y horas de creación
	// con las fecha y hora actual
	inode[num].datetimecreat=currdatetimetoint();
	inode[num].datetimemodif=currdatetimetoint();
	// Información sobre el dueño, grupo dueño y atributos
	inode[num].uid=uid;
	inode[num].gid=gid;
	inode[num].perms=atribs;
	inode[num].size=0;	// Tamaño del archivo en 0
	
	// Establecer los apuntadores a bloques directos en 0
	for(i=0;i<10;i++)
		inode[num].direct_blocks[i]=0;			///checar cambio blocks -> direct_blocks

	// Establecer los apuntadores indirectos en 0
	inode[num].indirect=0;
	inode[num].indirect2=0;

	// Optimizar la escritura escribiendo solo el sector lógico que
	// corresponde al inodo que estamos asignando.
	// i=num/4;
	// result=vdwriteseclog(inicio_nodos_i+i,&inode[i*4]);
	for(i=0;i<secboot.sec_tabla_nodos_i;i++)
		result=vdwriteseclog(inicio_nodos_i+i,&inode[i*4]);

	return(num);
}

// Buscar en la tabla de nodos I, el nodo I que contiene el 
// nombre del archivo indicado en el apuntador a la cadena
// Regresa el número de nodo i encontrado
// Si no lo encuentra, regresa -1
int searchinode(char *filename)
{
	int i;
	int free;
	int result;

	int nodos_i_en_memoria = 0;	///checar
	int inicio_nodos_i = 0;		///checar
	inicio_nodos_i = secboot.sec_inicpart +secboot.sec_res; 	///checar
	//Antes de continuar debe cargarse en memoria el sector lógico 1 que es el sector de boot de la partición.

	//Con los datos que están ahí, calcular:
	//El sector lógico donde empieza la tabla de nodos-i
	//También vamos a usar el número de sectores que tiene la tabla de nodos-i

	// Traer los sectores lógicos de los nodos I a memoria
	if(!nodos_i_en_memoria)
	{
		for(i=0;i<secboot.sec_tabla_nodos_i;i++)
			result=vdreadseclog(inicio_nodos_i+i,&inode[i*4]);

		nodos_i_en_memoria=1;
	}
	
	// El nombre del archivo no debe medir más de 18 bytes
	if(strlen(filename)>17)
	  	filename[17]='\0';

	// Recorrer la tabla de nodos I que ya tengo en memoria
// desde el principio hasta el final buscando el archivo.
	i=0;
	while(strcmp(inode[i].name,filename) && i<TOTAL_NODOS_I)
		i++;

	if(i>= TOTAL_NODOS_I)
		return(-1);		// No se encuentra el archivo
	else
		return(i);		// La posición donde fue encontrado 
}

// Eliminar un nodo I de la tabla de nodos I, y establecerlo 
// como disponible
int removeinode(int numinode)
{
	int i;

	unsigned short temp[512]; // 1024 bytes
// Desasignar los bloques directos en el mapa de bits que 
// corresponden al archivo

//Antes de continuar debe cargarse en memoria el sector lógico 1 que es el sector de boot de la partición.

//Con los datos que están ahí, calcular:
//El sector lógico donde empieza la tabla de nodos-i
//También vamos a usar el número de sectores que tiene la tabla de nodos-i

//Asegurar que los sectores de la tabla nodos-I están en memoria, si no están en memoria, cargarlos.

// Recorrer los apuntadores directos del nodo i
	for(i=0;i<10;i++)
		if(inode[numinode].direct_blocks[i]!=0) // Si es dif de cero
										// Si está asignado
		{
			unassignblock(inode[numinode].direct_blocks[i]);
			inode[numinode].direct_blocks[i]=0;					///checar blocks -> direct_blocks //all three
		}

	// Si el bloque indirecto, ya está asignado
	if(inode[numinode].indirect!=0)
	{
		// Leer el bloque que contiene los apuntadores
// a memoria
		readblock(inode[numinode].indirect,(char *) temp);
		// Recorrer todos los apuntadores del bloque para
		// desasignarlos 
		for(i=0;i<512;i++)
			if(temp[i]!=0)
				unassignblock(temp[i]);
		// Desasignar el bloque que contiene los apuntadores
		unassignblock(inode[numinode].indirect);
		inode[numinode].indirect=0;
	}

	// Poner en cero el bit que corresponde al inodo en el mapa
	// de bits de nodos-i
	unassigninode(numinode);
	return(1);
}
// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode()
{
	int i,j;
	int result;
	int inodesmap_en_memoria = 0; 		///checar

	unsigned short inicio_nodos_i;		///checar
	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}
	inicio_nodos_i = secboot.sec_inicpart +secboot.sec_res; 	///checar
//Usamos la información del sector de boot de la partición para 
						//determinar en que sector inicia el 
						// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(inicio_nodos_i, &inode);
		inodesmap_en_memoria=1;
	}

	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	i=0;
	while(mapa_bits_nodos_i[i]==0xFF && i<3)
		i++;

	if(i<3)
	{
		// Recorrer los bits del byte, para encontrar el bit
		// que está en cero
		j=0;
		while(mapa_bits_nodos_i[i] & (1<<j) && j<8)
			j++;

		return(i*8+j); // Regresar el bit del mapa encontrado en cero
	}
	else // Todos los bits del mapa de nodos i están en 1
		return(-1); // -1 significa que no hay nodos i disponibles

		
}


// Poner en 1, el bit que corresponde al número de inodo indicado
int assigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;

	int inodesmap_en_memoria = 0; 		///checar
	unsigned short inicio_nodos_i;		///checar

	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}
	inicio_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
		//Usamos la información del sector de boot de la partición para 
		//determinar en que sector inicia el 
		// mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(inicio_nodos_i,&inode);
		inodesmap_en_memoria=1;
	}

	mapa_bits_nodos_i[offset]|=(1<<shift); // Poner en 1 el bit indicado
	vdwriteseclog(inicio_nodos_i,&inode);
	return(1);
}

// Poner en 0, el bit que corresponde al número de inodo indicado
int unassigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	int inodesmap_en_memoria = 0; 		///checar
	unsigned short inicio_nodos_i;		///checar
	
	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria)
	{
		// Si no está en memoria, cárgalo
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}
	inicio_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
		//Usamos la información del sector de boot de la partición para 
		//determinar en que sector inicia el 
		// mapa de bits de nodos i 	
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria)
	{
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(inicio_nodos_i,&inode);	////checar
		inodesmap_en_memoria=1;
	}


	mapa_bits_nodos_i[offset]&=(char) ~(1<<shift); // Poner en cero el bit que corresponde al inodo indicado
	vdwriteseclog(inicio_nodos_i,inode);
	return(1);
}	


// ***********************************************************************************
// Funciones para el manejo de inodos
// ***********************************************************************************



unsigned int datetoint(struct Date date)
{
	unsigned int val=0;

	val=date.year-1970;
	val<<=4;
	val|=date.month;
	val<<=5;
	val|=date.day;
	val<<=5;
	val|=date.hour;
	val<<=6;
	val|=date.min;
	val<<=6;
	val|=date.sec;
	
	return(val);
}

int inttodate(struct Date *date,unsigned int val)
{
	date->sec=val&0x3F;
	val>>=6;
	date->min=val&0x3F;
	val>>=6;
	date->hour=val&0x1F;
	val>>=5;
	date->day=val&0x1F;
	val>>=5;
	date->month=val&0x0F;
	val>>=4;
	date->year=(val&0x3F) + 1970;

	return(1);
}

unsigned int currdatetimetoint()
{
	struct tm *tm_ptr;
	time_t the_time;
	
	struct Date now;

	(void) time(&the_time);
	tm_ptr=gmtime(&the_time);

	now.year=tm_ptr->tm_year-70;
	now.month=tm_ptr->tm_mon+1;
	now.day=tm_ptr->tm_mday;
	now.hour=tm_ptr->tm_hour;
	now.min=tm_ptr->tm_min;
	now.sec=tm_ptr->tm_sec;

	return(datetoint(now));
}
