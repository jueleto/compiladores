/********    Analizador Lexico  ********/
/********    Juan Ledesma  ************/

//Mensaje de error
void error(char* mensaje)
{
	printf("Lin %d: %s==>'%s'\n",numLinea,mensaje,t.pe->lexema);
    existe_error=TRUE;	
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
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id); //en la tabla hash
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				sprintf(e.lexema,id);
				e.compLex=ID;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=ID;
			}
			break;
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
							estado=6; //terminal
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.') //pascal
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
		else if (c=='<') 
		{
			//es un operador relacional, averiguar cual
			c=fgetc(archivo);
			if (c=='>'){
				t.compLex=OPREL;
				t.pe=buscar("<>");
			}
			else if (c=='='){
				t.compLex=OPREL;
				t.pe=buscar("<=");
			}
			else{
				ungetc(c,archivo);
				t.compLex=OPREL;
				t.pe=buscar("<");
			}
			break;
		}
		else if (c=='>')
		{
			//es un operador relacional, averiguar cual
				c=fgetc(archivo);
			if (c=='='){
				t.compLex=OPREL;
				t.pe=buscar(">=");
			}
			else{
				ungetc(c,archivo);
				t.compLex=OPREL;
				t.pe=buscar(">");
			}
			break;
		}
		else if (c=='=')
		{
			//puede ser un = o un operador de asignacion
			c=fgetc(archivo);
			if (c=='='){ //op de REL ==
				t.compLex=OPREL;
				t.pe=buscar("==");
			}
			else{ //es operador de asignacion
				ungetc(c,archivo);
				t.compLex=OPASIGNA;
	     		t.pe=buscar("=");
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
		else if (c=='=')
		{
			t.compLex=OPASIGNA;
			t.pe=buscar("=");
			break;
		}
		else if (c==',')
		{
			t.compLex=',';
			t.pe=buscar(",");
			break;
		}
		else if (c==';')
		{
			t.compLex=';';
			t.pe=buscar(";");
			break;
		}
		else if (c=='.')
		{
			t.compLex='.';
			t.pe=buscar(".");
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
		else if (c=='[')
		{
			t.compLex='[';
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=']';
			t.pe=buscar("]");
			break;
		}
		else if (c=='"')
		{//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='"')
				{
					c=fgetc(archivo);
					if (c=='"')
					{
						id[i]=c;
						i++;
						id[i]=c;
						i++;
					}
					else
					{
						id[i]='"';
						i++;
						break;
					}
				}
				else if(c==EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else{
					id[i]=c;
					i++;
				}
			}while(isascii(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				sprintf(e.lexema,id);
				e.compLex=LITERAL;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=e.compLex;
			}
			break;
		}
		//Comentario //
		else if (c=='/')
		{
			//elimina el comentario
		    c=fgetc(archivo);	
            if (c=='/') break;
             
			if (c==EOF)
				error("Se llego al fin de archivo sin finalizar un comentario");
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
