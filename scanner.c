/**
  * Prekladac imperativniho jazyka IFJ10
  * Projekt do predmetu IFJ a IAL
  * VUT FIT Brno, 2010
  *
  * Tym 5, varianta a/4/I
  * Resitele:
  *   Martin Knapovsky
  *   Pavel Antolik
  *   Ondrej Kratochvil
  *   Jan Myler
  *   Jiri Navratil
  *
  * Soubor scanner.c
  *
  * lexikalni analyzator
  */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "const.h"

#define STR_ERROR INT_ERR
#define ALLOC_ERR INT_ERR
#define TABLE_SIZE 32 //pocet prvku tabulky klicovych slov
#define KEYWORDS  20  //pocet klicovych slov
#define SHIFT 61  //dont touch this

const char* keywords[TABLE_SIZE] = {
  "AS"		,"DECLARE"	,"DIM",
  "DO"		,"DOUBLE"	,"ELSE",
  "END"		,"FIND"		,"FUNCTION",
  "IF"		,"INPUT"	,"INTEGER",
  "LOOP"	,"PRINT"	,"RETURN",
  "SCOPE"	,"SORT"		,"STRING",
  "THEN"	,"WHILE",
};

//promenna pro ulozeni ukazatele na zdrojovy soubor
FILE *source;
//globalni pocitadlo radku
int line = 1; 

//pomocna funkce pro nastaveni zdrojoveho souboru
void set_source_file(FILE* f)
{
  source = f;
}

/** Funkce, ktera zjisti, zda je nacitany retezec
 *  klicovym slovem jazyka ifj10
 *  @param attr ukazatel na strukturu s retezcem
 */
int is_keyword(string* attr)
{
  for(int i=0; i<KEYWORDS; i++)
  {
    if((strcmp(keywords[i], attr->str)) == 0)
      return i + SHIFT;
  }
  return 0;
}

/** Funkce, ktera reprezentuje lexikalni analyzator
 *  @param *attr ukazatel na atribut
 */
int get_next_token(string* attr)
{
  int state = S_START;  //pocatecni stav
  int c;  //promenna pro nacitani znaku
  str_clear(attr);  // pokazde musime vycistit attribut
  int x; //pomocna promenna
  
  while(1)  //nekonecny cyklus...dokud se nevrati token, bude probihat
  {
    c = getc(source); //znak cteme pouze tady
    
    //printf("%3.d, %3.d, %c, %3.d\n", line, c, c, state); //testovaci funkce

    switch(state){
      case S_START:
        if(isspace(c)){
          if(c=='\n'){
            line++;
            state = T_EOL;
            continue;
          }
          state = S_START;
        }
        else if(c=='&') state = S_NUM;  //doplneni cisla se zakladem 2, 8 a 16
        else if(c=='+') return T_ADD;
        else if(c=='-') return T_SUB;
        else if(c=='=') return T_E;
        else if(c=='(') return T_LEFT;
        else if(c==')') return T_RIGHT;
        else if(c=='*') return T_MUL;
        else if(c==';') return T_SEMI;
        else if(c==',') return T_COMMA;
        else if(c=='\\' ) return T_IDIV;
        else if(c=='\'' ) state = S_LN_CM;  //radkovy komentar
        else if(c=='\"') state = S_STR; //string...znak " neni potreba ukladat
        else if(c=='>') state = T_G;
        else if(c=='<') state = T_L;
        else if(c=='/') state = T_DIV;
        else if(c==EOF) return T_EOF; //konec souboru
        else if(c >= '0' && c <= '9'){  //integer
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_INT;
        }
        else if((c>='a' && c<='z')||(c>='A' && c<='Z')||(c=='_')){ //identifikator
          if(str_add_char(attr, toupper(c)) == STR_ERROR)
            return ALLOC_ERR;
          state = T_ID;
        }     
        else return LEX_ERR;
        break;
        
      //EOL
      case T_EOL:
        if(c == '\n'){
          line++;
        }
        else if(c == '\''){
          state = S_LN_CM;
        }
        else if(c == '/')
          state = T_DIV;
        else if(isspace(c));
				else{
          ungetc(c, source);
          return T_EOL;
        }
        break;
        
      //IDENTIFIKATOR
      case T_ID:
        x = toupper(c);
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_ID;
        }
        else if(x >= 'A' && x <= 'Z'){
          if(str_add_char(attr, x) == STR_ERROR)
            return ALLOC_ERR;
          state = T_ID;
        }
        else if(c == '_'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_ID;
        }
        else{
          ungetc(c, source);
          if((x=is_keyword(attr)) != 0)
            return x;
          else return T_ID;
        }
        break;
      
      //RADKOVY KOMENTAR
      case S_LN_CM:
        if(c==EOF)
          return T_EOF;
        else if(c=='\n'){
          line++;
          state = T_EOL;
        }
        else state = S_LN_CM;
        break;
      
      //STRING  
      case S_STR:
        if(c=='\"') return T_STR;
        else if(c=='\\') state = S_STR_ESC;
        else if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
        break;
      
      //ESCAPE SEKVENCE
      case S_STR_ESC:
        if(c=='n'){
          if(str_add_char(attr, '\n') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='\\'){
          if(str_add_char(attr, '\\') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='\''){
          if(str_add_char(attr, '\'') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='\"'){
          if(str_add_char(attr, '\"') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='0'){
          if(str_add_char(attr, '\0') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='a'){
          if(str_add_char(attr, '\a') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='b'){
          if(str_add_char(attr, '\b') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='f'){
          if(str_add_char(attr, '\f') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='r'){
          if(str_add_char(attr, '\r') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else if(c=='t'){
          if(str_add_char(attr, '\t') == STR_ERROR)
            return ALLOC_ERR;
          state = S_STR;
        }
        else return LEX_ERR;
        break;
      
      //GREATER
      case T_G:
        if(c=='=')
          return T_GE;
        else {
          ungetc(c, source);
          return T_G;
        }
        break;
      
      //LOWER
      case T_L:
        if(c=='=')
          return T_LE;
        else if(c=='>')
          return T_NE;
        else{
          ungetc(c, source);
          return T_L;
        }
        break;
      
      //REALNE DELENI
      case T_DIV:
        if(c=='\'')
          state = S_BL_CM;
        else {
          ungetc(c, source);
          return T_DIV;
        }
        break;
      
      //BLOKOVY KOMENTAR
      case S_BL_CM:
        if(c == EOF) return LEX_ERR;
        else if(c == '\n') line++;
        else if(c=='\'') state = S_BL_CM_EC;
        else state = S_BL_CM;
        break;
      
      //KONTROLA NA KONEC BLOKOVEHO KOMENTARE
      case S_BL_CM_EC:
        if(c=='/')
          state = S_START;
        else
          state = S_BL_CM;
        break;
      
      //CELE CISLO
      case T_INT:
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_INT;
        }
        else if(c=='.'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = S_NUM_DOT;
        }
        else if(toupper(c)=='E'){
          if(str_add_char(attr, toupper(c)) == STR_ERROR)
            return ALLOC_ERR;
          state = S_EXP;
        }
        else {
          ungetc(c, source);
          return T_INT;
        }
        break;
      
      //STAV OCEKAVAJICI CISLO ZA TECKOU
      case S_NUM_DOT:
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_REAL;
        }
        else return LEX_ERR;
        break;
      
      //STAV KTERY OVERUJE, ZE NASLEDUJE CISLO
      case S_NUM_EXP:
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_REAL;
        }
        else return LEX_ERR;
        break;
        
      //REALNE CISLO
      case T_REAL:
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_REAL; // zustavame ve stavu
        }
        else if(toupper(c)=='E'){
          if(str_add_char(attr, toupper(c)) == STR_ERROR)
            return ALLOC_ERR;
          state = S_EXP;
        }
        else
        {
          ungetc(c, source);
          return T_REAL;
        }
        break;
      
      //REALNE CISLO
      case T_REAL_EXP:
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == STR_ERROR)
            return ALLOC_ERR;
          state = T_REAL; // zustavame ve stavu
        }
        else
        {
          ungetc(c, source);
          return T_REAL;
        }
        break;
        
      //STAV EXPONENTU
      case S_EXP:
        if(c=='+' || c == '-'){
          if(str_add_char(attr, toupper(c)) == STR_ERROR)
            return ALLOC_ERR;
          state = S_NUM_EXP;
        }
        else if(c >= '0' && c <= '9'){
          if(str_add_char(attr, toupper(c)) == STR_ERROR)
            return ALLOC_ERR;
          state = T_REAL_EXP;
        }
        else return LEX_ERR;
        break;
      
      //EXTRA SEKCE
      case S_NUM:
        x=toupper(c);
        if(x=='H') state = S_HEXA;
        else if(x=='O') state = S_OCTAL;
        else if(x=='B') state = S_BIN;
        else return LEX_ERR;
        break;
      
      case S_HEXA:
        x=toupper(c);
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == INT_ERR)
            return INT_ERR;
          state = T_HEXA;
        }
        else if(x >= 'A' && x <= 'F'){
          if(str_add_char(attr, x) == INT_ERR)
            return INT_ERR;
          state = T_HEXA;
        }
        else return LEX_ERR;
        break;
      
      case S_OCTAL:
        if(c >= '0' && c <= '8'){
          if(str_add_char(attr, c) == INT_ERR)
            return INT_ERR;
          state = T_OCTAL;
        }
        else return LEX_ERR;
        break;
      
      case S_BIN:
        if(c == '0' || c == '1'){
          if(str_add_char(attr, c) == INT_ERR)
            return INT_ERR;
          state = T_BIN;
        }
        else return LEX_ERR;
        break;
      
      case T_HEXA:
        x=toupper(c);
        if(c >= '0' && c <= '9'){
          if(str_add_char(attr, c) == INT_ERR)
            return INT_ERR;
          state = T_HEXA;
        }
        else if(x >= 'A' && x <= 'F'){
          if(str_add_char(attr, x) == INT_ERR)
            return INT_ERR;
          state = T_HEXA;
        }
        else{
          ungetc(c, source);
          return T_HEXA;
        }   
        break;
      
      case T_OCTAL: 
        if(c >= '0' && c <= '8'){
          if(str_add_char(attr, c) == INT_ERR)
            return INT_ERR;
          state = T_OCTAL;
        }
        else{ 
          ungetc(c, source);
          return T_OCTAL;
        }
        break;
      
      case T_BIN:
        if(c == '0' || c == '1'){
          if(str_add_char(attr, c) == INT_ERR)
            return INT_ERR;
          state = T_BIN;
        }
        else{ 
          ungetc(c, source);
          return T_BIN;
        }
        break; 
    }
  }
  return 0;
}
