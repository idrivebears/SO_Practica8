#include "common_data.h"
#include "inode.h"

//Funciones para el manejo de los nodos i, usan el mapa de bits del área de nodos i para determinar si un nodo i está libre, ocupado, y también hay funciones para asignar y liberar nodos i
// *************************************************************************
// Para el mapa de bits del área de nodos i
// *************************************************************************

// Usando el mapa de bits, determinar si un nodo i, está libre u ocupado.

#define TOTAL_NODOS_I 24   			 ///checar
INODE inode[24];					 ///checar
SECBOOTPART secboot; 				 ///checar
int secboot_en_memoria = 1;		     ///checar	
char mapa_bits_nodos_i[3] = {0,0,0}; ///checar

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
		result=vdreadseclog(inicio_nodos_i, mapa_bits_nodos_i);
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



unsigned int datetoint(struct DATE date)
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

int inttodate(struct DATE *date,unsigned int val)
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
	
	struct DATE now;

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
