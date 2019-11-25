#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mmu.h>

#define NUMPROCS 4
#define PAGESIZE 4096
#define PHISICALMEMORY 12*PAGESIZE 
#define TOTFRAMES PHISICALMEMORY/PAGESIZE
#define RESIDENTSETSIZE PHISICALMEMORY/(PAGESIZE*NUMPROCS)
#define SIZE_OF_NON_V_MEMORY 12
#define TRUE 1
#define FALSE 0
#define ERROR -1

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;


int getfreeframe();
int searchvirtualframe();
int getfifo();
int get_free_virtual_frame();
int get_free_physical_frame();
int find_page_to_push();

int pagefault(char *vaddress)
{
    int virtual_frame;
    int in_virtual_mem = FALSE;
    
    
    int i;
    int frame, vframe; 
    long pag_a_expulsar;
    int fd;
    char buffer[PAGESIZE];
    int pag_del_proceso;

    // A partir de la dirección que provocó el fallo, calculamos la página
    pag_del_proceso=(long) vaddress>>12;

    // Si la página del proceso está en un marco virtual del disco
    if(ptbr[pag_del_proceso].framenumber >= framesbegin+SIZE_OF_NON_V_MEMORY)
    {
        /* Se relaciona con la parte de abajo, esta parte indica si la pagina estaba en memoria secundaria */
        in_virtual_mem = TRUE;
        
        vframe = ptbr[pag_del_proceso].framenumber;
		// Lee el marco virtual al buffer
        readblock(buffer, vframe);
        // Libera el frame virtual
        systemframetable[vframe].assigned = FALSE;
    }

    // Cuenta los marcos asignados al proceso
    i=countframesassigned();

    // Si ya ocupó todos sus marcos, expulsa una página
    if(i>=RESIDENTSETSIZE)
    {
		// Buscar una página a expulsar
		pag_a_expulsar = (long) find_page_to_push();
		// Poner el bit de presente en 0 en la tabla de páginas
        ptbr[pag_a_expulsar].presente = FALSE;
        // Si la página ya fue modificada, grábala en disco
        if(ptbr[pag_a_expulsar].modificado == TRUE)
        {
			// Escribe el frame de la página en el archivo de respaldo y pon en 0 el bit de modificado
            saveframe(ptbr[pag_a_expulsar].framenumber);
            ptbr[pag_a_expulsar].modificado = FALSE;
        }
		
        // Busca un frame virtual en memoria secundaria
        vframe = get_free_virtual_frame();
		// Si no hay frames virtuales en memoria secundaria regresa error
        if(vframe == ERROR)
		{
            return(-1);
        }
        // Copia el frame a memoria secundaria
        copyframe(ptbr[pag_a_expulsar].framenumber, vframe);
        //libera el marco de la memoria principal
        ptbr[pag_a_expulsar].presente = FALSE;
        ptbr[pag_a_expulsar].framenumber = vframe;
        //Actualiza la tabla de páginas 
        systemframetable[vframe].assigned = TRUE;
        systemframetable[frame].assigned = FALSE;
    }

    // Busca un marco físico libre en el sistema
    frame = get_free_physical_frame();
    
	// Si no hay marcos físicos libres en el sistema regresa error
    if(frame == ERROR)
    {
        return(-1); // Regresar indicando error de memoria insuficiente
    }

    // Si la página estaba en memoria secundaria
    if(in_virtual_mem)
    {
        // Cópialo al frame libre encontrado en memoria principal 
        writeblock(buffer, frame);
        //y transfiérelo a la memoria física
        loadframe(frame);        
    }
   
	// Poner el bit de presente en 1 en la tabla de páginas
    systemframetable[frame].assigned = TRUE;
    //y el frame 
    ptbr[pag_del_proceso].presente = TRUE;
    ptbr[pag_del_proceso].framenumber = frame;

    return(1); // Regresar todo bien
}

int get_free_physical_frame()
{
    /* Retorna el primer indice de marco fisico disponible que se encuentre */
    int end_of_physical_mem = framesbegin + TOTFRAMES;
    for(int count = framesbegin; count < end_of_physical_mem; count++)
    {
        if(systemframetable[count].assigned == FALSE)
            return count;
    }
    /* Si la funcion llega hasta aqui, es porque no hubo ningun marco disponible */
    return ERROR;
}

int get_free_virtual_frame()
{
    /* Retorna el primer indice de marco virtual disponible que se encuentre */
    int start_of_virtual_mem = framesbegin + TOTFRAMES;
    //Debido a que la memoria virtual se encuentra inmediatamente despues de la fisica
    int end_of_virtual_mem = start_of_virtual_mem + TOTFRAMES;
    for(int count = start_of_virtual_mem; count < end_of_virtual_mem; count++)
    {
        if(systemframetable[count].assigned == FALSE)
            return count;
    }
    /* Si la funcion llega hasta aqui, es porque no hubo ningun marco disponible */
    return ERROR;
}

int find_page_to_push()
{
    /* Determina, mediante el algoritmo _ cual pagina sale */
}






