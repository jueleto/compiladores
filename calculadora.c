/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Tarea 1
 *  Responsable: Juan Emilio Ledesma
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
char *pila[100]; //para apilar en el momento de conversion a postfija y evaluacion del mismo.
char *elemento; //elemento tope de la pila
int cima=0; // cantidad de elementos que tiene la pila

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

//Agregar elemento a la pila
void desapilar()
{
	pila[cima]='\0';
	cima--;
	elemento=pila[cima];
	
}
//Vaciar pila 
void vaciar ()
{
	pila[0]="V";
	cima=1;
}

//Retorna 1 si prioridad de op1 es mayor a op2
//Retorna 2 si prioridad de op1 es menor o igual a op2
int prioridad(char* op1, char* op2)
{
	int retornar=0;
	if((strcmp(op1,"*")==0||strcmp(op1,"/")==0) && (strcmp(op2,"+")==0||strcmp(op2,"-")==0)){
		retornar=1;
	}else {
		//prioridad menor o igual
		retornar=2;
	}
	return retornar;
}

//Recibe un vector de tokens y lo convierte a notacion postfija
void pasarPostfija(char *linea[50],int longitud){
	
	//En este vector se almacenar la notacion postfija o polaca
	char *postfija[50];	
	//indice de postfija
	int x=0;
	//indice de infija
	int k=0;
	
	//********Imprimir Infija
	printf("Linea %d--> ",(numLinea-1));
	for(k;k<longitud;k++){
		printf("%s",linea[k]);	
	}

	k=0;
	//Iteramos hasta consumir todos los elementos de la infija
	for(k;k<longitud;k++){
		
		//si es un operador
		if(strcmp(linea[k],"+")==0 || strcmp(linea[k],"-")==0 ||strcmp(linea[k],"*")==0 ||strcmp(linea[k],"/")==0){	
			
			//si la prioridad es mayor que el ultimo elemento de la pila o la pila esta vacia
			//se apila
			if(strcmp(pila[cima-1],"V")==0 || (prioridad(linea[k],pila[cima-1]))==1){
				apilar(linea[k]);
			} //si prioridad es menor o igual se desapila
			else {
				postfija[x]=pila[cima-1];
				desapilar();
				apilar(linea[k]);
				x++;
			}
			
		}else{ //Es un operando se pasa a la notacion postfija
			postfija[x]=linea[k];
			x++;
		}			
		
	}
	
	
	//Desapilar los elementos que queda
	while(elemento!="V"){
		desapilar();
		postfija[x]=elemento;
		x++;
	}
	x--;
	
	//le pasamos la notacion postfija a un evaluador de notacion postfija
	evaluarPostfija(postfija,x);
	
}

//Evalua la notacion postfija y calcula la operacion
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
	
	//Vaciamos la pila para reutilizarla
	vaciar();
	
	k=0;
	//recorremos la notacion postfija
	for(k;k<longitud;k++){
		
		//si es un operador
		if(strcmp(linea[k],"+")==0 || strcmp(linea[k],"-")==0 ||strcmp(linea[k],"*")==0 ||strcmp(linea[k],"/")==0){	
			
			//Si la pila NO esta vacia
			if (elemento!="V"){
				//obtenemos los ultimos 2 elementos de la pila y lo convertimos double
				desapilar();
				op2=atof(elemento);
			
				desapilar();
				op1=atof(elemento);
			
			
			//Comparar operaciones y realizamos la operacion segun corresponda
			if(strcmp(linea[k],"+")==0){
				resultado=op1+op2;
			}else if (strcmp(linea[k],"-")==0){ 
				resultado=op1-op2;
			}else if (strcmp(linea[k],"*")==0){ 
				resultado=op1*op2;
			}else if (strcmp(linea[k],"/")==0){ 
				resultado=op1/op2;				
			}
			
			//El resultado del calculo anterior lo convertimos a cadena y lo apilamos
			char toString[50];
			sprintf(toString,"%.5f",resultado);
			
			apilar(toString);
			
			resultado=0.0;
			op1=0.0;
			op2=0.0;	
			}

		} //si es un operando lo apilamos
		else{
			apilar(linea[k]);
		}
	}
	
	//Imprimir Resultado, si el resultado es null, significa que hay un errod sintactico
	if(pila[1]=='\0'){
		printf("  (Error Sintactico)\n");
	}else{
		printf(" = %s\n",pila[1]);
	}
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
						else{
							estado=6;
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
		   //comprobamos si es comentario
            if ((c=fgetc(archivo))=='/') //es comentario
            {
					//ignorar hasta que sea fin de la linea
               while(c!=EOF)
			   {
                     c=fgetc(archivo);  
                     if(c=='\n'){
                       ungetc(c,archivo);    
                       break; 
                     }        
                }    
                            
            } // no es comentario
            else{ 
               ungetc(c,archivo); 	
               t.compLex=OPMULT;
			   t.pe=buscar("/");
               break;
			   
			}

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
			
			//Cortamos cada vez que cambie numero de linea
			if(g_nro_linea==numLinea){
				strcat(linea, t.pe->lexema);
				token_linea[j]=t.pe->lexema;
				j++;
			}else{
				linea[0]='\0';
				strcat(linea, t.pe->lexema);
				
				//Vaciar pila y utilizarlo
				vaciar();
				//Convertimos a postfija la expresion y posteriormente lo evaluamos
				pasarPostfija(token_linea,j);
				
				j=0;
				token_linea[j]=t.pe->lexema;
				j++;
				g_nro_linea=numLinea;
			}
		
		//	printf("Lin %d: %s -> %d\n",numLinea,t.pe->lexema,t.compLex);
		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
