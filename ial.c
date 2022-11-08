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
  * Soubor ial.c
  *
  * Soubor s definicemi vestavenych funkci find() a sort() pro imperativni jazyk IFJ2010
  * Soubor s definici tabulky symbolu implementovane pomoci BVS  
  */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "ial.h"
#include "const.h"

// telo vyhledavaci funkce
int find(string *haystack, string *needle){

  // vytah vyhledavaneho retezce a jeho delky z prislusne struktury
  char *needle_str = str_get_str(needle);
  int needle_l = str_get_length(needle);

  int fail[needle_l];   // deklarace fail vektoru
  
  // inicializace pomocnych promennych a nastaveni uvodnich hodnot do vektoru
  int pos = 2;
  int cnd = 0;
  fail[0] = -1;
  fail[1] = 0;
  
  // konstrukce fail vektoru  
  while(pos < needle_l){
    if(needle_str[pos-1] == needle_str[cnd]){
      cnd++;
      fail[pos] = cnd;
      pos++;
    }else if(cnd > 0){
      cnd = fail[cnd];
    }else{
      fail[pos] = 0;
      pos++;
    }
  }  

  // vytah vyhledavaneho retezce a jeho delky z prislusne struktury  
  char *haystack_str = str_get_str(haystack);
  int haystack_l = str_get_length(haystack);
  
  // inicializace pomocnych promennych pro cyklus vyhledavani
  int m = 0;
  int i = 0;
  
  // samotne vyhledavani podretezce
  while(m+i < haystack_l){
    if(needle_str[i] == haystack_str[m+i]){
      // podminka ukonceni funkce
      if(i == (needle_l-1)){
        // je-li nalezen cely retezec, vratime jeho pozici
        return m;
      }
      i++;
    }else{
      m = m + i - fail[i]; // hodnota -1 na prvnim znaku fail-vektoru zde zarucuje posun vpred
      if(fail[i] > -1){
        i = fail[i];
      }else{
        i = 0;
      }
    }
  }
  // navrat - nenalezeno
  return -1;
}

// merge(..) - funkce pro slevani mnozin, volana funkci merge_sort(..)
void merge(char input[], char temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;
 
  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;
 
  while((left <= left_end) && (mid <= right)){
    if(input[left] <= input[mid]){
      temp[tmp_pos] = input[left];
      tmp_pos++;
      left++;
    }else{
      temp[tmp_pos] = input[mid];
      tmp_pos++;
      mid++;
    }
  }
 
  while(left <= left_end){
    temp[tmp_pos] = input[left];
    left++;
    tmp_pos++;
  }
  while(mid <= right){
    temp[tmp_pos] = input[mid];
    mid++;
    tmp_pos++;
  }
  for (i=0; i <= num_elements; i++){
    input[right] = temp[right];
    right--;
  }
}

// telo funkce merge_sort(..)
void merge_sort(char input[], char temp[], int left, int right){

  int mid;
 
 // rozdeleni a nasledne rekurzivni setrizovani
  if (right > left){
    mid = (right + left) / 2;
    merge_sort(input, temp, left, mid);
    merge_sort(input, temp, mid+1, right);
    
    // spojeni dvou setridenych mnozin
    merge(input, temp, left, mid+1, right);
  }
}


string sort(string *srcstr){
  // vytahnuti retezce ktery budeme radit z prislusne struktury a zjisteni jeho delky
  char *unsorted = str_get_str(srcstr);      
  int unsorted_l = str_get_length(srcstr); 

  // deklarace pomocneho a vysledneho pole
  char temp[unsorted_l];
  
  // serazeni retezce
  merge_sort(unsorted, temp, 0, unsorted_l-1);  
  
  // prekopirovani serazeneho retezce zpet do struktury string
  str_clear(srcstr);
  for(int aa = 0; aa < unsorted_l; aa++){       
       str_add_char(srcstr, temp[aa]);
  }
    return *srcstr;
}

/* ############################################## */
/* ##### Sekce implementace tabulek symbolu ##### */
/* ############################################## */

/**
 * Funkce inicializuje lokalni tabulku symbolu implementovanou pomoci BVS.
 * @param **rootPtr ukazatel na koren BVS (tabulku symbolu)
 */      
inline void lts_init(tLTS** rootPtr) {
   *rootPtr = NULL;
}

/**
 * Funkce vlozi do lokalni tabulky symbolu novy identifikator. 
 * Rekurzivne se hleda umisteni pro novy identifikator v ramci
 * BVS stromu.  
 * @param **rootPtr ukazatel na koren BVS (tabulku symbolu)
 * @param *key ukazatel na klic (jmeno identifikatoru)
 * @param type typ identifikatoru (integer, double, string)
 * @param *i index v ramci programoveho zasobniku
 * @return vraci TAB_INSERT_SUCC pokud se povedlo novy identifikator vlozit,
 *         vraci TAB_INSERT_FAIL jestlize identifikator byl uz vlozen,
 *         vraci TAB_INSERT_EMEM pokud nastane chyba pri praci s pameti  
 */      
int lts_insert(tLTS** rootPtr, string* key, int type, int* i) {
   // pokud je tabulka symbolu prazdna, vytvor novy uzel
   if(*rootPtr == NULL) {
      tLNode* nodePtr = (tLNode*) malloc(sizeof(struct tLNode));
      if(nodePtr == NULL) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c lts_insert(): Nepodarilo se alokovat pamet pro novou polozku lokalni tabulky symbolu.\n");
         #endif
         return TAB_INSERT_EMEM;
      }        
        
      // inicializace klice
      if(str_init(&(nodePtr->key)) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c lts_insert(): Nepodarilo se inicializovat polozku klice.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }
        
      // vlozeni hodnoty klice
      if(str_copy_string(&(nodePtr->key), key) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c lts_insert(): Nepodarilo se zkopirovat klic (jmeno identif.) do tabulky symbolu.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }
        
      // vlozeni datoveho typu promenne
      nodePtr->data.varType = type;
        
      // nastaveni indexu
      nodePtr->data.varOffset = *i;
      *i += 1;

      // inicializace unie? podle typu promenne na hodnotu???
      switch(type) {
         case T_INTEGER:
            nodePtr->data.varValue.iVal = 0;
            break;
         case T_DOUBLE:
            nodePtr->data.varValue.dVal = 0.0;
            break;
         case T_STRING:
            str_init(&(nodePtr->data.varValue.sVal));
            break;
      }

      // inicializace ukazatelu na podstromy
      nodePtr->lPtr = NULL;
      nodePtr->rPtr = NULL;
        
      // navazani uzlu do stromu
      *rootPtr = nodePtr;
   }    
   else if (str_cmp_string(key, &((*rootPtr)->key)) < 0) {
      // pokracovani v levem podstromu
      return lts_insert(&((*rootPtr)->lPtr), key, type, i);        
   }
   else if (str_cmp_string(key, &((*rootPtr)->key)) > 0) {
      // pokracovani v pravem podstromu
      return lts_insert(&((*rootPtr)->rPtr), key, type, i);
   }
   else {
      // promenna je uz vlozena
      return TAB_INSERT_FAIL;
   }
    
   return TAB_INSERT_SUCC;
}

/**
 * Funkce vyhleda identifikator v lokalni tabulce symbolu.
 * @param *rootPtr ukazatel na BVS (tabulku symbolu)
 * @param *key ukazatel na klic (jmeno identifikatoru)
 * @return ukazatel na data pro dany identifikator, NULL pokud nenalezne ID    
 */ 
tLNode* lts_search(tLTS* rootPtr, string* key) {
   if(rootPtr == NULL) {
      return NULL;
   }
   else {
      if(str_cmp_string(key, &(rootPtr->key)) == 0) {
         // nalezen klic
         return rootPtr;
      }
      else {
         // nenalezen klic, porovnani klicu a pokracovani v levem ci pravem podstromu
         if(str_cmp_string(key, &(rootPtr->key)) < 0) {
            return lts_search(rootPtr->lPtr, key);
         }
         else {
            return lts_search(rootPtr->rPtr, key);
         }
      }
   }
}

/**
 * Funkce zrusi lokalni tabulku symbolu a uvolni pamet.
 * @param **rootPtr ukazatel na BVS (tabulku symbolu)
 */   
void lts_free(tLTS** rootPtr) {
   if(*rootPtr != NULL) {
      // rekurzivni zanoreni do leveho a potom praveho podstromu
      lts_free(&((*rootPtr)->lPtr));
      lts_free(&((*rootPtr)->rPtr));
        
      // uvolneni pameti klice
      str_free(&((*rootPtr)->key));
      
      // pokud je v unii string
      if((*rootPtr)->data.varType == T_STRING) {
         str_free(&((*rootPtr)->data.varValue.sVal));
      }

      // uvolneni pameti uzlu a nastaveni na NULL
      free(*rootPtr);
      *rootPtr = NULL;
   }
}


/**
 * Funkce vyhleda identifikator v lokalni TS a vrati jeho datovy typ.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param *key nazev lokalni promenne
 * @return vraci datovy promenne pri nalezeni
 *         vraci SEM_ERR pokud nenalezne promennou v lokalni TS
 *         vraci INT_ERR pokud je ukazatel na uzel GTS NULL
 */
inline int lts_get_vartype(tGNode* recPtr, string* key) {
   if(recPtr != NULL) {
      tLNode* localTS = lts_search(recPtr->data.localTS, key);
      if(localTS != NULL)
         return localTS->data.varType;
      else 
         return SEM_ERR;
   }
   else {
      return INT_ERR;
   }
}

/** 
 * Funkce vyhleda identifikator v lokalni TS a vrati ukazatel na jeho uzel.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param *key nazev lokalni promenne
 * @return vraci ukazatel na tLNode
 */
tLNode* lts_get_varnode(tGNode* recPtr, string* key) {
   if(recPtr != NULL) {
      tLNode* localTS = lts_search(recPtr->data.localTS, key);
      return localTS;
   }
   else {
      return NULL;
   }
}

/**
 * Funkce vytvori novou vnitrni promennou pro danou funkci.
 * @param *rootPtr ukazatel na BVS (tabulka symbolu)
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param type datovy typ promenne
 * @return vraci ukazatel na vytvoreny tLNode
 */
tLNode* lts_set_varnode(tGTS** rootPtr, tGNode* recPtr, int type) {
   string new;
   str_init(&new);
   str_add_char(&new, '$');
   int i = recPtr->data.argCount + recPtr->data.varCount;
   
   // vygenerovani unikatniho nazvu v ramci jedne funkce
   while(i != 0) {
      str_add_char(&new, (char) (i%10 + '0'));
      i = i/10;
   }
  
   // vlozeni noveho zaznamu o promenne do lokalni tabulky symbolu dane funkce
   gts_set_var(rootPtr, recPtr, type, &new);
   
   tLNode* result = lts_get_varnode(recPtr, &new);

   // EDIT: uvolneni str?
   str_free(&new);
   
   // vraceni ukazatele na nove vytvoreny uzel
   return result;
}

/**
 * Pridani dat do LTS, konverze BASE rozsireni na int.
 */
tLNode* lts_set_const(tGTS** rootPtr, tGNode* recPtr, int type, string* attr) {
   int tmpType;
   string new;
   tLNode* tmpNode;
   str_init(&new);
   str_add_char(&new, '$');
   int i = recPtr->data.argCount + recPtr->data.varCount;

   // DEBUG
   // printf("set_const: typ[%d], attr[%s]\n", type, str_get_str(attr));
   // DEBUG

   // vygenerovani unikatniho nazvu v ramci jedne funkce
   while(i != 0) {
      str_add_char(&new, (char) (i%10 + '0'));
      i = i/10;
   }

   // nastaveni typu T_INTEGER pro T_HEXA, T_OCTAL a T_BIN
   switch(type) {
      case T_HEXA:
      case T_OCTAL:
      case T_BIN:
      case T_INT:
         tmpType = T_INTEGER;
         break;
      case T_REAL:
      case T_REAL_EXP:
         tmpType = T_DOUBLE;
         break;
      case T_STR:
         tmpType = T_STRING;
         break;
   }

   // vlozeni noveho zaznamu o promenne do lokalni tabulky symbolu
   gts_set_var(rootPtr, recPtr, tmpType, &new);
   
   tmpNode = lts_get_varnode(recPtr, &new);
   
   char* tmpChar = str_get_str(attr);
   // pridani hodnoty
   switch(type) {
      case T_HEXA:
         tmpNode->data.varValue.iVal = (int) strtol(tmpChar, NULL, 16);
         break;
      case T_OCTAL:
         tmpNode->data.varValue.iVal = (int) strtol(tmpChar, NULL, 8);
         break;
      case T_BIN:
         tmpNode->data.varValue.iVal = (int) strtol(tmpChar, NULL, 2);
         break;
      case T_INT:
         tmpNode->data.varValue.iVal = (int) strtol(tmpChar, NULL, 10);
         break;
      case T_REAL:
      case T_REAL_EXP:
         tmpNode->data.varValue.dVal = strtod(str_get_str(attr), NULL);
         break;
      case T_STR:
         str_copy_string(&(tmpNode->data.varValue.sVal), attr);
         break;
   }
   
   // DEBUG
   /*
   switch(type) {
      case T_HEXA:
      case T_OCTAL:
      case T_BIN:
      case T_INT:
         printf("hodnota T_INTEGER: %d\n", tmpNode->data.varValue.iVal);
         break;
      case T_REAL:
      case T_REAL_EXP:
         printf("hodnota T_DOUBLE: %f\n", tmpNode->data.varValue.dVal);
         break;
      case T_STR:
         printf("hodnota T_STRING: %s\n", str_get_str(&(tmpNode->data.varValue.sVal)));
         break;
   }
   */
   // DEBUG

   str_free(&new);

   return tmpNode;
}

/**
 * Funkce inicializuje globalni tabulku symbolu implementovanou pomoci BVS.
 * @param **rootPtr ukazatel na koren BVS (tabulku symbolu)
 */      
inline void gts_init(tGTS** rootPtr) {
   *rootPtr = NULL;
}

/**
 * Funkce vlozi do globalni tabulky symbolu novy zaznam funkce.
 * Rekurzivne se hleda umisteni pro novy zaznam v ramci
 * BVS stromu.  
 * @param **rootPtr ukazatel na koren BVS (tabulku symbolu)
 * @param *key ukazatel na klic (jmeno funkce)
 * @param *recPtr fce zde vraci uk. na polozku TS (pro vkladani dodatecnych informaci)
 * @return vraci TAB_INSERT_SUCC pokud se povedlo novy identifikator vlozit,
 *         vraci TAB_INSERT_FAIL jestlize identifikator byl uz vlozen,
 *         vraci TAB_INSERT_EMEM pokud nastane chyba pri praci s pameti  
 */      
int gts_insert(tGTS** rootPtr, string* key, tGNode** recPtr) {
   // pokud je tabulka symbolu prazdna, vytvor novy uzel
   if(*rootPtr == NULL) {
      tGNode* nodePtr = (tGNode*) malloc(sizeof(struct tGNode));
      if(nodePtr == NULL) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert(): Nepodarilo se alokovat pamet pro novou polozku globalni tabulky symbolu.\n");
         #endif
         return TAB_INSERT_EMEM;
      }        
        
      // inicializace klice
      if(str_init(&(nodePtr->key)) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert(): Nepodarilo se inicializovat polozku klice.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }
        
      // vlozeni hodnoty klice
      if(str_copy_string(&(nodePtr->key), key) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert(): Nepodarilo se zkopirovat klic (jmeno identif.) do tabulky symbolu.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }
        
      // inicializace ukazatelu na podstromy
      nodePtr->lPtr = NULL;
      nodePtr->rPtr = NULL;
      
      // inicializace datove struktury
      nodePtr->data.argCount = 0;
      nodePtr->data.retType = 0;
      nodePtr->data.def = 0;
      nodePtr->data.localTS = NULL;
      nodePtr->data.varCount = 0;
      nodePtr->data.varReserved = 0;
      nodePtr->data.fInstr = NULL;
      
      // inicializace klice
      if(str_init(&(nodePtr->data.types)) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert(): Nepodarilo se inicializovat polozku pro typy parametru funkce.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }

      // navazani uzlu do stromu
      *rootPtr = nodePtr;
      
      // vraceni uk. na tento uzel
      *recPtr = nodePtr;      
   }   
   else if (str_cmp_string(key, &((*rootPtr)->key)) < 0) {
      // pokracovani v levem podstromu
      return gts_insert(&((*rootPtr)->lPtr), key, recPtr);        
   }
   else if (str_cmp_string(key, &((*rootPtr)->key)) > 0) {
      // pokracovani v pravem podstromu
      return gts_insert(&((*rootPtr)->rPtr), key, recPtr);
   }
   else {
      // funkce je uz vlozena
      return TAB_INSERT_FAIL;
   }
    
   return TAB_INSERT_SUCC;
}

/**
 * Funkce vlozi do globalni tabulky symbolu novy zaznam rezervace identifikatoru promenne.
 * Rekurzivne se hleda umisteni pro novy zaznam v ramci BVS stromu.  
 * @param **rootPtr ukazatel na koren BVS (tabulku symbolu)
 * @param *key ukazatel na klic (jmeno parametru/lokalni promenne)
 * @return vraci TAB_INSERT_SUCC pokud se povedlo novy identifikator vlozit,
 *         vraci TAB_INSERT_FAIL jestlize identifikator byl uz vlozen,
 *         vraci TAB_INSERT_EMEM pokud nastane chyba pri praci s pameti  
 */      
int gts_insert_var(tGTS** rootPtr, string* key) {
   // pokud je tabulka symbolu prazdna, vytvor novy uzel
   if(*rootPtr == NULL) {
      tGNode* nodePtr = (tGNode*) malloc(sizeof(struct tGNode));
      if(nodePtr == NULL) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert_var(): Nepodarilo se alokovat pamet pro novou polozku globalni tabulky symbolu.\n");
         #endif
         return TAB_INSERT_EMEM;
      }        
        
      // inicializace klice
      if(str_init(&(nodePtr->key)) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert_var(): Nepodarilo se inicializovat polozku klice.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }
        
      // vlozeni hodnoty klice
      if(str_copy_string(&(nodePtr->key), key) == INT_ERR) {
         #ifdef DEBUG
            fprintf(stderr, "ial.c gts_insert_var(): Nepodarilo se zkopirovat klic (jmeno identif.) do tabulky symbolu.\n");
         #endif
         free(nodePtr);
         return TAB_INSERT_EMEM;
      }
        
      // inicializace ukazatelu na podstromy
      nodePtr->lPtr = NULL;
      nodePtr->rPtr = NULL;
      
      // inicializace datove struktury
      nodePtr->data.argCount = 0;
      nodePtr->data.retType = 0;
      nodePtr->data.def = 0;
      nodePtr->data.localTS = NULL;
      nodePtr->data.varCount = 0;
      nodePtr->data.varReserved = 1;
      nodePtr->data.fInstr = NULL;
      
      // navazani uzlu do stromu
      *rootPtr = nodePtr;
   }   
   else if (str_cmp_string(key, &((*rootPtr)->key)) < 0) {
      // pokracovani v levem podstromu
      return gts_insert_var(&((*rootPtr)->lPtr), key);        
   }
   else if (str_cmp_string(key, &((*rootPtr)->key)) > 0) {
      // pokracovani v pravem podstromu
      return gts_insert_var(&((*rootPtr)->rPtr), key);
   }
   else {
      // identifikator je jiz vlozen, kdyz je to ID promenne, je vse ok 
      // (promenne muzou byt ve vice funkcich stejneho jmena)
      if((*rootPtr)->data.varReserved) {
         return TAB_INSERT_SUCC;      
      }
      return TAB_INSERT_FAIL;
   }
    
   return TAB_INSERT_SUCC;
}

/**
 * Funkce vyhleda zaznam funkce v globalni tabulce symbolu. 
 * Pozor!! Funkce nerozlisuje, zda se jedna o uzel funkce ci rezervace jmena 
 * parametru/lokalni promenne. Toto je treba explicitne kontrolovat.
 * @param *rootPtr ukazatel na BVS (tabulku symbolu)
 * @param *key ukazatel na klic (jmeno funkce)
 * @return ukazatel na data pro danou funkci, NULL pokud nenalezne ID    
 */ 
tGNode* gts_search(tGTS* rootPtr, string* key) {
   if(rootPtr == NULL) {
      return NULL;
   }
   else {
      if(str_cmp_string(key, &(rootPtr->key)) == 0) {
         // nalezen klic
         return rootPtr;
      }
      else {
         // nenalezen klic, porovnani klicu a pokracovani v levem ci pravem podstromu
         if(str_cmp_string(key, &(rootPtr->key)) < 0) {
            return gts_search(rootPtr->lPtr, key);
         }
         else {
            return gts_search(rootPtr->rPtr, key);
         }
      }
   }
}

/**
 * Funkce zrusi globalni tabulku symbolu a uvolni pamet.
 * @param **rootPtr ukazatel na BVS (tabulku symbolu)
 */   
void gts_free(tGTS** rootPtr) {
   if(*rootPtr != NULL) {
      // rekurzivni zanoreni do leveho a potom praveho podstromu
      gts_free(&((*rootPtr)->lPtr));
      gts_free(&((*rootPtr)->rPtr));
      
      // uvolneni pameti klice
      str_free(&((*rootPtr)->key)); 
      
      // uvolneni pameti typu parametru, pokud se jedna o funkci
      if((*rootPtr)->data.varReserved == 0) {
         str_free(&((*rootPtr)->data.types));
      }

      // uvolneni pameti lokalni tabulky symbolu
      lts_free(&((*rootPtr)->data.localTS)); 
      
      // uvolneni pameti uzlu a nastaveni na NULL
      free(*rootPtr);
      *rootPtr = NULL;
   }
} 

/**
 * Funkce prida informaci o datovem typu parametru do globalni
 * tabulky symbolu. Pri uspesnem pridani zvysi pocitadlo parametru.
 * @param *rootPtr ukazatel na BVS (tabulka symbolu)
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param argType datovy typ parametru (ocekava konstanty T_INTEGER, T_DOUBLE, T_STRING)
 * @param *key nazev parametru (klic pro polozku lokalni TS)
 * @return pri uspesnem pridani vraci SUCCESS, pri neuspechu vraci SEM_ERR nebo INT_ERR
 */
inline int gts_set_arg(tGTS** rootPtr, tGNode* recPtr, int argType, string* key) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_set_arg(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return INT_ERR;
   }
   
   int err;   
   
   // kontrola jestli muze byt identifikator parametru pouzit (neni jiz rezervovan pro identif. fce)
   // pokus o vlozeni/rezervaci identif. v globalni TS, pri nezdaru - neni mozne ID pouzit => SEM_ERR
   if((err = gts_insert_var(rootPtr, key)) != TAB_INSERT_SUCC) {
      if(err == TAB_INSERT_EMEM) {
         return INT_ERR;      
      }
      else {
         return SEM_ERR;
      }
   }
   
   // pridani informace o parametru do lokalni tabulky symbolu + navyseni citace parametru
   if((err = lts_insert(&(recPtr->data.localTS), key, argType, &(recPtr->data.argCount))) == TAB_INSERT_SUCC) {
      // pridani informace o typu
      switch(argType) {
         case T_INTEGER:
            str_add_char(&(recPtr->data.types), 'i');
            break;
         case T_DOUBLE:
            str_add_char(&(recPtr->data.types), 'd');
            break;
         case T_STRING:
            str_add_char(&(recPtr->data.types), 's');
            break;
      }
      return SUCCESS;  
   }
   // pokud uz dana promenna/parametr je v lokalni tabulce symbolu
   else if(err == TAB_INSERT_FAIL) {
      return SEM_ERR;
   }
   // chyby pameti pri vkladani
   else {
      return INT_ERR;
   }
}

/**
 * Funkce prida informaci o lokalni promenne do lokalni tabulky symbolu dane fce.
 * Zaroven je kontrolovano, zda jiz dany ID promenne nebyl pouzit jako jmeno nektere
 * z fci. Po uspesnem pridani je navysen citac promennych funkce.
 * @param *rootPtr ukazatel na BVS (tabulka symbolu)
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param argType datovy typ promenne (ocekava konstanty T_INTEGER, T_DOUBLE, T_STRING)
 * @param *key nazev parametru (klic pro polozku lokalni TS)
 * @return pri uspesnem pridani vraci SUCCESS, pri neuspechu vraci SEM_ERR nebo INT_ERR
 */
inline int gts_set_var(tGTS** rootPtr, tGNode* recPtr, int argType, string* key) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_set_var(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return INT_ERR;
   }
   
   int err;   
   
   // kontrola jestli muze byt identifikator parametru pouzit (neni jiz rezervovan pro identif. fce)
   // pokus o vlozeni/rezervaci identif. v globalni TS, pri nezdaru - neni mozne ID pouzit => SEM_ERR
   if((err = gts_insert_var(rootPtr, key)) != TAB_INSERT_SUCC) {
      if(err == TAB_INSERT_EMEM) {
         return INT_ERR;      
      }
      else {
         return SEM_ERR;
      }
   }
   
   int offset = recPtr->data.argCount + recPtr->data.varCount;
   
   // pridani informace o promenne do lokalni tabulky symbolu + navyseni citace parametru
   if((err = lts_insert(&(recPtr->data.localTS), key, argType, &offset)) == TAB_INSERT_SUCC) {
      recPtr->data.varCount += 1;
      return SUCCESS;  
   }
   // pokud uz dana promenna/parametr je v lokalni tabulce symbolu
   else if(err == TAB_INSERT_FAIL) {
      return SEM_ERR;
   }
   // chyby pameti pri vkladani
   else {
      return INT_ERR;
   }
}

/**
 * Funkce nastavi navratovy typ dane funkce, pokud nebyl nastaven. Pokud byla funkce deklarovana,
 * musi se pri definici navratove typy shodovat. Jinak chyba.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param rt datovy typ navratove hodnoty (ocekava konstanty T_INTEGER, T_DOUBLE, T_STRING)
 * @return SUCCESS, INT_ERR
 */
inline int gts_set_retval(tGNode* recPtr, int rt) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_set_retval(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return INT_ERR;
   }

   recPtr->data.retType = rt;
   return SUCCESS;
}

/**
 * Funkce kontroluje, zda se je spravny navratovy typ funkce, ktera jiz
 * ma zaznam v globalni tabulce symbolu.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @param rt datovy typ navratove hodnoty (ocekava konstanty T_INTEGER, T_DOUBLE, T_STRING)
 * @return SUCCESS, INT_ERR, SEM_ERR
 */
inline int gts_check_retval(tGNode* recPtr, int rt) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_check_retval(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return INT_ERR;
   }
  
   if(recPtr->data.retType == rt)
      return SUCCESS;
   else
      return SEM_ERR;
}

/**
 * Porovnani jmen, datovych typu a poradi parametru u jiz deklarovane
 * funkce s definici.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS) 
 * @param argType datovy typ parametru (ocekava konstanty T_INTEGER, T_DOUBLE, T_STRING)
 * @param *key nazev parametru (klic pro polozku lokalni TS)
 * @param i poradi parametru (offset v prog. zasobniku)
 * @return SUCCESS, SEM_ERR, INT_ERR
 */ 
inline int gts_check_arg(tGNode* recPtr, int argType, string* key, int i) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_check_arg(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return INT_ERR;
   }
    
   tLNode* tmp = lts_search(recPtr->data.localTS, key);
   
   if(tmp == NULL || tmp->data.varType != argType || tmp->data.varOffset != i) {
      return SEM_ERR;
   }
   else {
      return SUCCESS;
   }
}

/**
 * Nastavi do globalni TS priznak, ze dana funkce jiz byla definovana.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 */
inline void gts_set_def(tGNode* recPtr) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_set_def(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return;  // !! jen vyskoci, funkce je void
   }
   
   recPtr->data.def = 1;
}

/**
 * Zkontroluje, zda ma dana funkce nastaven priznak, ze jiz byla definovana.
 * @param *recPtr ukazatel na zaznam v globalni tabulce symbolu (uzel GTS)
 * @return 0 - nebyla definovana, 1 - byla definovana, INT_ERR - chyba
 */
inline int gts_check_def(tGNode* recPtr) {
   if(recPtr == NULL) {
      #ifdef DEBUG
         fprintf(stderr, "ial.c gts_set_def(): Ukazatel na uzel glob. tabulky symbolu je NULL.\n");
      #endif
      return INT_ERR;
   }
   
   return recPtr->data.def;
}

/**
 * Rekurzivne projde celou globalni tabulku a kontroluje, zda jsou vsechny
 * vlozene funkce definovane.
 * @param *rootPtr ukazatel na BVS (tabulku symbolu)
 * @return SUCCESS pokud jsou vsechny funkce definovane
 *         SEM_ERR pokud jedna nebo vice funkci definovanych neni
 */
int gts_check_alldefs(tGTS* rootPtr) {
   int result;
   if(rootPtr != NULL) {
      // rekurzivni zanoreni do leveho a potom praveho podstromu
      if((result = gts_check_alldefs(rootPtr->lPtr)) != SUCCESS)
         return result;
      if((result = gts_check_alldefs(rootPtr->rPtr)) != SUCCESS)
         return result;
      
      // pokud se jedna o zaznam funkce a neni nastaven priznak definice funkce
      if(rootPtr->data.varReserved == 0 && rootPtr->data.def == 0) {
         return SEM_ERR;
      }      
          
      return SUCCESS;
   }

   return SUCCESS;
}
