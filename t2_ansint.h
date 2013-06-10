/********    Analizador Sintactico  ********/
/********    Juan Ledesma       ************/

/************** Prototipos *********************/
void comparar(int);
void lista_sentencias();
void sentencia();
void sent_asignacion();
void expresion();
void term();
void simple_exp();
void factor();
void parse();
void expresion_logica();
void esarray();
void sent_if ();
void sen_for();
void sent_declaracion();
void sent_write();

/**************** Funciones **********************/
void comparar(int tokenEsperado){
    if (t.compLex==tokenEsperado){
        sigLex();   
    }
    else{
        error(" no se esperaba el token ");
    }
}

/**********  Lista de Sentencias   ************/
void lista_sentencias(){
    sentencia();
    while ((t.compLex!=EOF) && 
           (t.compLex!=END) && 
           (t.compLex!=PR_ELSE))
    {        
        comparar(';');
        if((t.compLex!=EOF) && 
           (t.compLex!=END) && 
           (t.compLex!=PR_ELSE)){
               sentencia();                   
        } 
    }
    
}
/********* Eleccion de Sentencia ********/
void sentencia(){
    switch(t.compLex){    
    case PR_IF: sent_if();break;
    case PR_FOR: sen_for();break;
    case VAR: sent_declaracion();break;
    case ID : sent_asignacion();break;    
    case ST_WRITE: sent_write();break;
    case ST_WRITELN: sent_write();break;

    default: error(" no se esperaba ");
      sigLex();    
      break;
    }    
}
/*********** Sentencia de Asignacion */
void sent_asignacion(){
    if(t.compLex==ID){
        comparar(ID);
        esarray(); //si la variable es un array
        comparar(OPASIGNA);
        expresion();        
    }
    
}
void expresion_logica(){
     expresion();
     if(t.compLex==OPERLOGIC){
       comparar(OPERLOGIC);
       expresion();        
     }                                            
}

void expresion(){
     simple_exp();
    if (t.compLex==OPREL){
       comparar(t.compLex);
       simple_exp();       
    }   
}   
void simple_exp(){
    term();
    //OPSUMA = + | -
    while(t.compLex==OPSUMA){
        comparar(t.compLex);
        term();          
    } 
}
void term(){
     factor();
     //OPMULT=* | /
     while(t.compLex==OPMULT){
       comparar(t.compLex);
       factor();                  
     }

}
void factor(){
     switch(t.compLex){
        case NUM:
             comparar(NUM);
             break;
        case ID:
             comparar(ID);
             esarray();//nuevalinea
             break;
        case '(':
             comparar('(');
             expresion();     
             comparar(')'); 
             break;
        default :
           error(" no se esperaba ");   
           sigLex();   
           break;                                        
      }     
 }

 void parse(){
      sigLex();        
      lista_sentencias();
      
 } 
 
void esarray(){
    if (t.compLex=='['){
       comparar('[');
       simple_exp();
       comparar(']');            
    }    
}
void sent_if(){
     comparar(PR_IF);
     expresion_logica();
     comparar(THEN);
     lista_sentencias();
     if(t.compLex==PR_ELSE){
        comparar(PR_ELSE);
        lista_sentencias();           
     }
     comparar(END);
     comparar(PR_IF);
}
void sen_for(){
     comparar(PR_FOR);
     sent_asignacion();
     comparar(TO);
     factor();     
     comparar(STEP);
     factor();
     comparar(PR_DO);
     lista_sentencias();
     comparar(END);
     comparar(PR_FOR);
     
}
void sent_declaracion(){
     comparar(VAR);
     comparar(ID);
     esarray();         
     while(t.compLex==','){
        comparar(',');
        comparar(ID);
        esarray();                        
     }
} 
void sent_write(){
     comparar(t.compLex);
     comparar('(');
     if(t.compLex==LITERAL){
        comparar(LITERAL);                                           
     }
     else{
        factor();
     }
     comparar(')');
}



