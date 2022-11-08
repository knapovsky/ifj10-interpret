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
  * Soubor ial.h
  *
  * Hlavickovy soubor vestavenych funkci jazyka IFJ10 a tabulky symbolu 
  */

#ifndef _IAL_H
#define _IAL_H

#include "str.h"
#include "ilist.h"

#define TAB_INSERT_SUCC 0
#define TAB_INSERT_FAIL 1
#define TAB_INSERT_EMEM 2

//hlavicka funkce pro vyhledavani podretezce v retezci
int find(string *haystack, string *needle);

//hlavicka funkce pro vyhledavani podretezce v retezci
string sort(string *srcstr);

void merge_sort(char input[], char temp[], int left, int right);
void merge(char input[], char temp[], int left, int mid, int right);

typedef union {
   int iVal;
   double dVal;
   string sVal;
}  tValue;

// datova struktura pro data lokalni promenne
typedef struct {
   int varType;        // dat. typ promenne
   int varOffset;      // offset v prog. zasobniku
   tValue varValue;    // unie bude obsahovat data 
} tLData;

// datova struktura pro uzel lokalni tabulky symbolu
typedef struct tLNode {
   string key;  // klic je nazev promenne
   tLData data;
   struct tLNode *lPtr, *rPtr; 
} tLNode;

// vytvoreni typu pro lokalni tabulku symbolu
typedef tLNode tLTS;

// datova struktura pro data funkce
typedef struct {
   int argCount;        // pocet parametru fce
   int retType;         // dat. typ navratove hodnoty fce
   int varCount;        // pocet lokalnich promennych fce
   tLTS* localTS;       // ukazatel na lokani TS fce
   struct listItem* fInstr;   // ukazatel na prvni instrukci fce
   int def;             // po deklaraci fce = 0, po definici fce = 1
   int varReserved;     // po vlozeni rezervace jmena lokalni promenne = 1
   string types;        // string s typy parametru - pro kontrolu pri volani funkce
} tGData;

// datova struktura pro uzel globalni tabulky symbolu
typedef struct tGNode {
   string key;  // klic je nazev funkce
   tGData data;
   struct tGNode *lPtr, *rPtr; 
} tGNode;

// vytvoreni typu pro globalni tabulku symbolu
typedef tGNode tGTS;

// prototypy funkci pro praci s lokalni tabulkou symbolu
void lts_init(tLTS**);
int lts_insert(tLTS**, string*, int, int*);
tLNode* lts_search(tLTS*, string*);
void lts_free(tLTS**);   
int lts_get_vartype(tGNode*, string*);
tLNode* lts_get_varnode(tGNode*, string*);
tLNode* lts_set_varnode(tGTS**, tGNode*, int);
tLNode* lts_set_const(tGTS**, tGNode*, int, string*);

// prototypy funkci pro praci s globalni tabulkou symbolu
void gts_init(tGTS**);
int gts_insert(tGTS**, string*, tGNode**);
int gts_insert_var(tGTS**, string*);
tGNode* gts_search(tGTS*, string*);
void gts_free(tGTS**);
int gts_set_var(tGTS**, tGNode*, int, string*);
int gts_set_arg(tGTS**, tGNode*, int, string*);
int gts_check_arg(tGNode*, int, string*, int);
int gts_set_retval(tGNode*, int);
int gts_check_retval(tGNode*, int);
void gts_set_def(tGNode*);
int gts_check_def(tGNode*);
int gts_check_alldefs(tGTS*);
void generate_variable(string*, int);

#endif
