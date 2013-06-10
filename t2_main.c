/*********** LIbrerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/***************** MACROS **********************/
//Codigos
#define VAR			256  //var 

/*if then else */
#define PR_IF		257 // if  
#define THEN		258 //then 
#define PR_ELSE		259 //else 

/*for to step do */ 
#define PR_FOR		260 //for 
#define TO			261 //to 
#define STEP		262 //st   
#define PR_DO		263 //do   

/*end , que sera usado asi 'end if' o 'end for'*/
#define END			264 //end 

/*do write */
#define ST_WRITE    265 //WRITE  

/*token de conjuntos de caracteres*/
#define NUM			266 //numero
#define ID			267 //identificador
#define LITERAL		268 //literal "as"

/*operador de asignacion*/
#define OPASIGNA	269

/*tokens simples operadores */
#define OPREL		270 //<=, >=, ==
#define OPSUMA		271 //+,- 
#define OPMULT		272 //* /
#define OPERLOGIC   273 // or and
#define ST_WRITELN  274 //WRITE  

// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/************* Definiciones ********************/
typedef enum {TRUE, FALSE} Error;
typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	//es lo que apare en elcodigo 
	struct entrada *tipoDato; // null puede representar variable no declarada, otra entrada la tabla de simbolos , no sera usado, por solo analizaremos lexicamente
	// aqui irian mas atributos para funciones y procedimientos...
	
} entrada;

typedef struct {
	int compLex;
	entrada *pe; //tabla de simbolos
} token;


typedef enum 
    /* book-keeping tokens */
   {ENDFILE,ERROR,
    /* reserved words */
    VAR_,IF,THEN_,ELSE_,END_,FOR_,TO_,STEP_,DO_,WRITE,
    /* multicharacter tokens */
    ID_,NUM_,
    /* special symbols */
    ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI
   } TokenType;
typedef enum {Stmtk,ExpK} NodeKind; //enumeración es un conjunto de constantes enteras
typedef enum {IfK, RepeatK, AssignK, WriteK} StmtKind;
typedef enum {OpK, ConstK, IdK} ExpKind;
typedef enum {Void, Integer, Boolean} ExpType;//será utiliza para la verificación de tipos
#define MAXCHILDREN 3

typedef struct treeNode
{
    struct treeNode * child [MAXCHILDREN];
    struct treeNode * sibling;
    
    int lineno;
    NodeKind nodekind;
    
    //Union: Pueden albergar diferentes tipos de datos, pero solo uno, de entre todos los posibles, al mismo tiempo
    //El valor almacenado es sobreeescrito cada vez que se asigna un valor al mismo miembro o a un miembro diferente, aquí radica la diferencia con las estructuras.
    union {StmtKind stmt; ExpKind exp;}kind;
    union {TokenType op; int val; char * name; }attr;
    ExpType type; 
} TreeNode;

/************* Variables globales **************/
int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico
FILE *archivo;			
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
Error existe_error= FALSE;

/************** Prototipos *********************/

void sigLex();		// Del analizador Lexico	

/**************** Funciones **********************/
int stricmp(const char* cad,const char* cad2) 
{
	int i;
	char c1[strlen(cad)];
	char c2[strlen(cad2)];
	
	strcpy(c1,cad);
	strcpy(c2,cad2);
	for(i=0;i<strlen(c1);i++)
		c1[i]=tolower(c1[i]);

	for(i=0;i<strlen(c2);i++)
		c2[i]=tolower(c2[i]);

	return strcmp(c1,c2);
}

/*********************HASH************************/
entrada *tabla;				//declarar la tabla de simbolos
int tamTabla=TAMHASH;		//utilizado para cuando se debe hacer rehash
int elems=0;				//utilizado para cuando se debe hacer rehash

int h(char* k, int m)
{
	unsigned h=0,g;
	int i;
	for (i=0;i<strlen(k);i++)
	{
		h=(h << 4) + k[i];
		if (g=h&0xf0000000){
			h=h^(g>>24);
			h=h^g;
		}
	}
	return h%m;
}
void insertar(entrada e);

void initTabla()
{	
	int i=0;
	
	tabla=(entrada*)malloc(tamTabla*sizeof(entrada));
	for(i=0;i<tamTabla;i++)
	{
		tabla[i].compLex=-1;
	}
}

int esprimo(int n)
{
	int i;
	for(i=3;i*i<=n;i+=2)
		if (n%i==0)
			return 0;
	return 1;
}

int siguiente_primo(int n)
{
	if (n%2==0)
		n++;
	for (;!esprimo(n);n+=2);

	return n;
}

//en caso de que la tabla llegue al limite, duplicar el tamaño
void rehash()
{
	entrada *vieja;
	int i;
	vieja=tabla;
	tamTabla=siguiente_primo(2*tamTabla);
	initTabla();
	for (i=0;i<tamTabla/2;i++)
	{
		if(vieja[i].compLex!=-1)
			insertar(vieja[i]);
	}		
	free(vieja);
}

//insertar una entrada en la tabla
void insertar(entrada e)
{
	int pos;
	if (++elems>=tamTabla/2)
		rehash();
	pos=h(e.lexema,tamTabla);
	while (tabla[pos].compLex!=-1)
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	tabla[pos]=e;

}
//busca una clave en la tabla, si no existe devuelve NULL, posicion en caso contrario
entrada* buscar(char *clave)
{
	int pos;
	entrada *e;
	pos=h(clave,tamTabla);
	while(tabla[pos].compLex!=-1 && stricmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	return &tabla[pos];
}

void insertTablaSimbolos(char *s, int n)
{
	entrada e;
	sprintf(e.lexema,s);
	e.compLex=n;
	insertar(e);
}

void initTablaSimbolos()
{
	int i;
	entrada pr,*e;
	char *vector[]={
	"var",
    "if", 
    "then", 
    "else",
	"for", 
    "to", 
    "step", 
    "do",
    "end",
    "write"	
  	};
  	//rango de 0 a 9 , para tomar solo las palabras reservadas
 	for (i=0;i<10;i++)
	{
		insertTablaSimbolos(vector[i],i+256);
//			printf("\n %s ->%d",vector[i],i+256);
	}
	//notese que el numero correpondiente a los parentesis y corchetes
	//es su mismo numero de ascii, aprovechando que son caracteres unitarios
    insertTablaSimbolos(",",',');
	insertTablaSimbolos(".",'.');
	insertTablaSimbolos(";",';');
	insertTablaSimbolos("(",'(');
	insertTablaSimbolos(")",')');
	insertTablaSimbolos("[",'[');
	insertTablaSimbolos("]",']');
	insertTablaSimbolos("<",OPREL);
	insertTablaSimbolos("<=",OPREL);
	insertTablaSimbolos("<>",OPREL);
	insertTablaSimbolos("==",OPREL);
	insertTablaSimbolos(">",OPREL);
	insertTablaSimbolos(">=",OPREL);
	insertTablaSimbolos("=",OPASIGNA);
	insertTablaSimbolos("+",OPSUMA);
	insertTablaSimbolos("-",OPSUMA);
	insertTablaSimbolos("or",OPSUMA);
	insertTablaSimbolos("*",OPMULT);
	insertTablaSimbolos("/",OPMULT);
	insertTablaSimbolos("and",OPERLOGIC);
	insertTablaSimbolos("AND",OPERLOGIC);
	insertTablaSimbolos("or",OPERLOGIC);
	insertTablaSimbolos("OR",OPERLOGIC);
	insertTablaSimbolos("WRITE",ST_WRITE);
 	insertTablaSimbolos("writeln",ST_WRITELN);
 	insertTablaSimbolos("WRITELN",ST_WRITELN);
 	insertTablaSimbolos("IF",PR_IF);
    insertTablaSimbolos("VAR",VAR);
    insertTablaSimbolos("THEN",THEN);
    insertTablaSimbolos("ELSE",PR_ELSE);
    insertTablaSimbolos("FOR",PR_FOR);
   	insertTablaSimbolos("TO",TO);
   	insertTablaSimbolos("STEP",STEP);
   	insertTablaSimbolos("DO",PR_DO);
}

//librerias propias
#include "tp2_anlex.h"
#include "tp2_ansint.h"

/********* MAIN *************/

int main(int argc,char* args[])
{
    
    
	int complex=0;             //iniciar el analizador lexico
	initTabla();
	initTablaSimbolos();

         if(argc > 1)
	     {
    		 if (!(archivo=fopen(args[1],"rt")))
    		 {
    			printf("\tNo se ha encontrado el Archivo..");
    			exit(1);
    		 }
         
         parse();
            
         if(existe_error==FALSE){
             printf("\n ---ANALISIS EXITOSO. NO SE ENCONTRARON ERRORES---\n\n");                                  
         }
        
         fclose(archivo);

       	
         }
     
         else
         {
    		 printf("Debe pasar como parametro el path del archivo fuente.");
    		 exit(1);
	    } 
	    return 0;   
}
