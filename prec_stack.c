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
 * Soubor prec_stack.c
 *
 * Soubor se zasobnikem pro precedencni analyzu.
 */
#include <stdlib.h>
#include <stdio.h>
#include "prec_stack.h"
#include "scanner.h"
#include "prec_analyse.h"
#include "str.h"

void Error(void)	{


/*
** Vytiskne upozorneni na to, zda doslo k chybe.
**/
    printf ("*ERROR* Chyba pøi práci se seznamem.\n"); //TODO vychytat chyby
                    
}

void InitList (tStack *L)	{
  // inicializace
  L->First = NULL;
  L->Act = NULL;
}

void DisposeList (tStack *L)	{
/*
 Zrusi vsechny prvky seznamu L a uvede seznam L do stavu, v jakem se nachazel
 po inicializaci. Vsechny prvky seznamu L budou korektne uvolneny volanim
 operace free.
*/
  while (L->First != NULL){
    if(L->First->ptr!= NULL){   //kontrola zda jde o poslední prvek z dùodu pøiøazování pointru
      tElem *tmp;
      tmp = L->First;
      L->First =L->First->ptr;
      str_free(&(tmp->attr));
      free(tmp);
    }
    else{
      tElem *tmp;
      tmp = L->First;
      str_free(&(tmp->attr));
      free(tmp);
      L->First = NULL;
      L->Act = NULL;
    }
  }
}

void InsertFirst (tStack *L, int val, string atribut)	{
// vytvoreni a alokace prvku a nasledne prirazeni na prvni misto seznamu a to pomoci prohazeni pointru
  tElem *newItem;

  if ((newItem = (tElem*)malloc (sizeof (struct tElem))) == NULL){
  
  Error();
  }
  else{
    newItem->data = val;
    str_init(&(newItem->attr));
    str_copy_string(&(newItem->attr), &atribut);
    newItem->ptr = L->First;
    newItem->ptl = NULL;
    if(L->First!=NULL){
      L->First->ptl = newItem; 
    }
    
    L->First = newItem;
  }
}

void First (tStack *L)		{
  L->Act = L->First;  // oznaceni prvniho prvku jako aktivniho
}

int CopyFirst (tStack *L)	{
/*
** Vrati hodnotu prvniho prvku seznamu L.
** Pokud je seznam L prazdny, vola funkci Error().
**/
  if (L->First != NULL){
    return L->First->data;
  }
  else{
 
    Error();
  }
return 0;
}

void DeleteFirst (tStack *L)	{
/*
** Rusi prvni prvek seznamu L. Pokud byl ruseny prvek aktivni, aktivita seznamu
** se ztraci. Pokud byl seznam L prazdny, nic se nedeje!
**/
  if(L->First==L->Act){ //kontrola zda je prvek aktivni pokud ano zrusi se aktivita
    L->Act = NULL;
  }
    tElem *tmp;
    tmp = L->First;
    L->First =L->First->ptr;
    str_free(&(tmp->attr));
    free(tmp);
}

void PostInsert (tStack *L, int val, string attribute)	{
/*
** Vlozi prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedeje!
** V pripade, ze neni dostatek pameti pro novy prvek pri operaci malloc,
** vola funkci Error().
**/
  if(Active(L)){
    tElem *newItem;
    if ((newItem = (tElem*)malloc (sizeof (struct tElem))) == NULL){
      
    Error();
    }
    else{
      newItem->data = val;
      str_init(&(newItem->attr));
      str_copy_string(&(newItem->attr), &attribute);
      newItem->ptr = L->Act->ptr;
      L->Act->ptr = newItem;
    }
  }
}

void PreInsert (tStack *L, int val, string attribute)	{
/*
** Vlozi prvek s hodnotou val pred aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedeje!
** V pripade, ze neni dostatek pameti pro novy prvek pri operaci malloc,
** vola funkci Error().
**/
  if(Active(L)){
    tElem *newItem;
    if ((newItem = (tElem*)malloc (sizeof (struct tElem))) == NULL){
      
    Error();
    }
    else{
      newItem->data = val;
      str_init(&(newItem->attr));
      str_copy_string(&(newItem->attr), &attribute);
      newItem->ptr = L->Act;
      newItem->ptl = L->Act->ptl;
      if(L->Act->ptl!=NULL){
        L->Act->ptl->ptr = newItem;   

       }
       else {
        L->First=newItem;
       }
        

      L->Act->ptl = newItem;


    }
  }
}

int Copy (tStack *L){
/*
** Vrati hodnotu aktivniho prvku seznamu L.
** Pokud seznam neni aktivni, vola se funkce Error().
**/
  if(Active(L)){
    return L->Act->data;
  }
  else{
 
    Error(); // výpis chyby
  }
 return 0;

}

string * CopyPtrAttr (tStack *L){
/*
** Vrati atribut aktivniho prvku seznamu L.
** Pokud seznam neni aktivni, vola se funkce Error().
**/
  if(Active(L)){
    return &(L->Act->attr);
  }
  else{
 
    Error(); // výpis chyby
  }
 return &(L->Act->attr);

}

string CopyAttr (tStack *L){
/*
** Vrati atribut aktivniho prvku seznamu L.
** Pokud seznam neni aktivni, vola se funkce Error().
**/
  if(Active(L)){
    return L->Act->attr;
  }
  else{
 
    Error(); // výpis chyby
  }
 return L->Act->attr;

}

void Actualize (tStack *L, int val)	{
/*
** Prepise data aktivniho prvku seznamu L.
** Pokud seznam L neni aktivni, nedela nic!
**/

  if(Active(L)){
    L->Act->data = val;
  }
}

void Succ (tStack *L)	{
/*
 Posune aktivitu na nasledujici prvek seznamu L.
 Vsimnete si, že touto operaci se muze aktivni seznam stat neaktivnim.
 Pokud seznam L neni aktivni, nedela nic!
*/
  if(Active(L)){
    if(L->Act->ptr != NULL){
    L->Act = L->Act->ptr;
    }
    else{
      L->Act = NULL;
        }

  }
}

int Active (tStack *L) 	{

 return (L->Act==NULL)?0:1;

}

void FirstTerminal(tStack *L){
  
  First(L);
while(!IsTerminal(Copy(L))){  
	Succ(L);
	}
}

int FirstLower(tStack *L){
  int x=0;
  First(L);

  //Succ(L);

  while(Copy(L)!=FLAG){  
	Succ(L);
	x++;
if(x==10){return 4;}

	}
return x;
}

int IsTerminal(int token){
   switch(token){
    case T_E:
    case T_ID:
    case T_EOL:
    case T_THEN:
    case T_LEFT:
    case T_RIGHT:
    case T_ADD:
    case T_SUB:
    case T_MUL:
    case T_IDIV:
    case T_DIV:
    case T_G:
    case T_GE:
    case T_L:
    case T_LE:
    case T_NE:
    case T_INT:                
    case T_REAL:           
    case T_REAL_EXP: 
    case T_HEXA:            
    case T_OCTAL:     
    case T_BIN:
    case T_STR:
      return 1; 
    default: 
      return 0; 
   }
}

void DelettoLower (tStack *L){
  while(L->First->data != FLAG){
    DeleteFirst(L);
  }
    DeleteFirst(L);
  }


