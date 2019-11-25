// Definición de constantes

#define NINGUNO -1

// Definición de estructuras


/** El sistema utiliza un arreglo de este tipo de estructura para administrar los marcos disponibles y asignados 
de la memoria principal*/
struct SYSTEMFRAMETABLE {
    int assigned;/** true: marco asignado a pagina de proceso. False: marco libre*/

    char *paddress; // No modificar, posee direccion de memoria del marco
    int shmidframe;  // No modificar
};

struct PROCESSPAGETABLE {
    int presente;     /** indica si la pagina se encuentra asignada a un marco de memoria fisica*/
    int modificado;   /** indica si la pagina ha sido modificada o no*/ 
    int framenumber; /** marco asignado de la pagina, si la pagina no tiene marco entoces deve ser -1*/
    unsigned long tarrived;
    unsigned long tlastaccess;

    int attached;   // No modificar
};

int pagefault(char *vaddress);
int countframesassigned();
int copyframe(int sframe,int dframe);
int writeblock(char *buffer, int dblock);
int readblock(char *buffer, int sblock);
int loadframe(int frame);
int saveframe(int frame);

