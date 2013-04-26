/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Tarea 1
 *	Descripcion:
 * Implementar una calculadora de expresiones aritméticas, con manejo de
 * números reales, enteros o en notación científica. Recibe un archivo fuente
 * con expresiones aritméticas simples por línea (opcionalmente pueden
 * contener paréntesis, no es obligatorio considerar), debe realizar un análisis
 * léxico reconociendo tokens, y luego por línea debe evaluar la expresión
 * aritmética e imprimir en pantalla el resultado de la evaluación. Se debe
 * respetar la prioridad de los operadores aritméticos. En caso que una línea
 * posea un error léxico deberá imprimir un mensaje de error y continuar con
 * la siguiente línea del archivo fuente. Además los tokens deben ser
 * introducidos en una tabla de símbolos (se sugiere una representación en
 * forma de tabla hash).
 *	
 */
 
 
 
 /*********** LIbrerias utilizadas **************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/***************** MACROS **********************/

//Codigos
#define NUM			284
#define OPSUMA		291
#define OPMULT		292
#define OPASIGNA	293

// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/************* Definiciones ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...
	
} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token;


typedef struct 
{
char tok[50];
} g_token;



/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente pascal
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea


// Variables agregadas para infija, postifija y validacion
char *pila[100];
char *elemento;
int cima=0;



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

//Infija a Postfija y Evaluacion de postfija ***************************


void apilar (char cad[50])
{
	pila[cima]=cad;
	elemento=cad;
	cima++;
}

void desapilar()
{
	pila[cima]='\0';
	cima--;
	elemento=pila[cima];
	
}

void vaciar ()
{
	pila[0]="V";
	cima=1;
}

int prioridad(char* op1, char* op2)
{
	int retornar=0;
	if((strcmp(op1,"*")==0||strcmp(op1,"/")==0) && (strcmp(op2,"+")==0||strcmp(op2,"-")==0)){
		//prioridad  
		retornar=1;
	}else {
		//prioridad menor o igual
		retornar=2;
	}
	return retornar;
	
}


void pasarPostfija(char *linea[50],int longitud){
	
	char *postfija[50];	
	//indice de postfija
	int x=0;
	//indice de infija
	int k=0;
	
	//********Imprimir Infija
	//printf("InFija: ");
	for(k;k<longitud;k++){
		printf("%s",linea[k]);	
	}
//	printf("\n");
	
	
	k=0;
	for(k;k<longitud;k++){
		
		if(strcmp(linea[k],"+")==0 || strcmp(linea[k],"-")==0 ||strcmp(linea[k],"*")==0 ||strcmp(linea[k],"/")==0){	
			//apilar(linea[k]);
		
			//printf("\nel:%s_pi1:%s_pi2:%s_ci:%d\n",elemento,pila[cima],pila[cima-1],cima);
			//printf("prio entre %s y %s es:%d\n",linea[k],pila[cima-1],prioridad(linea[k],pila[cima-1]));
			if(strcmp(pila[cima-1],"V")==0 || (prioridad(linea[k],pila[cima-1]))==1){
				apilar(linea[k]);
			}else {
				postfija[x]=pila[cima-1];
				desapilar();
				apilar(linea[k]);
				x++;
			}
			
		}else{ //Es un operando
			postfija[x]=linea[k];
			x++;
		}			
		
	}
	
	
	//Desapilar lo que queda
	while(elemento!="V"){
		desapilar();
		postfija[x]=elemento;
		x++;
	}
	x--;
	
	evaluarPostfija(postfija,x);
	
}


void evaluarPostfija(char *linea[50],int longitud){
	
	double resultado;
	double op1;
	double op2;
	resultado=0.0;
	op1=0.0;
	op2=0.0;

	//indice pila
	int x=0;
	//indice postfija
	int k=0;
	
	/*****Imprimir Postfija
	printf("\nPostFija:_");
	for(k;k<longitud;k++){
		printf("%s_",linea[k]);	
	}
	printf("\n");
	*/
	
	//Vaciamos la pila para reutilizarlo
	vaciar();
	
	k=0;
	for(k;k<longitud;k++){
		if(strcmp(linea[k],"+")==0 || strcmp(linea[k],"-")==0 ||strcmp(linea[k],"*")==0 ||strcmp(linea[k],"/")==0){	
		
			
			if (elemento!="V"){
				desapilar();
				op2=atof(elemento);
			
				desapilar();
				op1=atof(elemento);
			
			
			
			//Comparar operaciones
			if(strcmp(linea[k],"+")==0){
				resultado=op1+op2;
			}else if (strcmp(linea[k],"-")==0){ 
				resultado=op1-op2;
			}else if (strcmp(linea[k],"*")==0){ 
				resultado=op1*op2;
			}else if (strcmp(linea[k],"/")==0){ 
				resultado=op1/op2;				
			}
			
			//printf("op1: %.5f %s op2: %.5f = %.5f\n",op1,linea[k], op2,resultado);
			
			char toString[50];
			sprintf(toString,"%.5f",resultado);
			
			//printf("cad %s\n",toString);
			apilar(toString);
			
			resultado=0.0;
			op1=0.0;
			op2=0.0;	
			}

		}else{
			apilar(linea[k]);
		}
	}
	
	//Imprimir Resultado
	if(pila[1]=='\0'){
		printf(" = Error Sintactico\n");
	}else{
		printf(" = %s\n",pila[1]);
	}
	/*/Imrpimir Pila
	for(x;x<cima;x++){
		printf("%s_",pila[x]);
	}*/	
}

/*********************HASH************************/
entrada *tabla;				//declarar la tabla de simbolos
int tamTabla=TAMHASH;		//utilizado para cuando se debe hacer rehash
int elems=0;				//utilizado para cuando se debe hacer rehash

int h(const char* k, int m)
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
	for (;!esprimo(n);n+=2); //n+=2 es lo mismo que n=n+2

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
entrada* buscar(const char *clave)
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

void insertTablaSimbolos(const char *s, int n)
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
	const char *vector[]={
		"program",
		"type",
		"var",
		"array",
		"begin",
		"end",
		"do",
		"to",
		"downto",
		"then",
		"of",
		"function",
		"procedure", 
		"integer", 
		"real", 
		"boolean", 
		"char", 
		"for", 
		"if", 
		"else", 
		"while", 
		"repeat", 
		"until", 
		"case", 
		"record", 
		"writeln",
		"write",
		"const"
	};
 	for (i=0;i<28;i++)
	{
		insertTablaSimbolos(vector[i],i+256);
	}
	insertTablaSimbolos("+",OPSUMA);
	insertTablaSimbolos("-",OPSUMA);
	insertTablaSimbolos("*",OPMULT);
	insertTablaSimbolos("/",OPMULT);
	
}

// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex()
{
	int i=0, longid=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{	
		if (c==' ' || c=='\t')
			continue;	//eliminar espacios en blanco
		else if(c=='\n')
		{
			//incrementar el numero de linea
			numLinea++;
			continue;
		}
	
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->compLex==-1)
						{
							sprintf(e.lexema,id);
							e.compLex=NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=NUM;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c=='+')
		{
			t.compLex=OPSUMA;
			t.pe=buscar("+");
			break;
		}
		else if (c=='-')
		{
			t.compLex=OPSUMA;
			t.pe=buscar("-");
			break;
		}
		else if (c=='*')
		{
			t.compLex=OPMULT;
			t.pe=buscar("*");
			break;
		}
		else if (c=='/')
		{
			t.compLex=OPMULT;
			t.pe=buscar("/");
			break;
		}
		else if (c=='(')
		{
			if ((c=fgetc(archivo))=='*')
			{//es un comentario
				while(c!=EOF)
				{
					c=fgetc(archivo);
					if (c=='*')
					{
						if ((c=fgetc(archivo))==')')
						{
							break;
						}
						ungetc(c,archivo);
					}
				}
				if (c==EOF)
					error("Se llego al fin de archivo sin finalizar un comentario");
				continue;
			}
			else
			{
				ungetc(c,archivo);
				t.compLex='(';
				t.pe=buscar("(");
			}
			break;
		}
		else if (c==')')
		{
			t.compLex=')';
			t.pe=buscar(")");
			break;
		}
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

int main(int argc,char* args[])
{
	// inicializar analizador lexico
	int complex=0;

	initTabla();
	initTablaSimbolos();
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		int g_nro_linea=1;
		char linea[100];
		linea[0]='\0';
		
		
		int j=0;
		char *token_linea[50];
		
		
		while (t.compLex!=EOF){
			sigLex();
			
			if(g_nro_linea==numLinea){
				strcat(linea, t.pe->lexema);
				token_linea[j]=t.pe->lexema;
				j++;
			}else{
			//	printf("%s\n",t.pe->lexema);
			//	printf("Linea: %d\n",g_nro_linea);
			//	printf("%s\n",linea);
				linea[0]='\0';
				strcat(linea, t.pe->lexema);
				
				//Vaciar pila y utilizarlo
				vaciar();
				pasarPostfija(token_linea,j);
				
				/*
				int k=0;
				printf("Infija cant. Caracteres j: %d\n",j);
				for(k;k<j;k++){
					printf("%s",token_linea[k]);	
				}
				printf("\n");
				
				*/
				
				
				j=0;
				token_linea[j]=t.pe->lexema;
				j++;
				g_nro_linea=numLinea;
				
				
			}
		
			
			//printf("Lin %d: %s -> %d\n",numLinea,t.pe->lexema,t.compLex);
		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
