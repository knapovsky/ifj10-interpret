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
  * Soubor interpret.h
  *
  * Implementace seznamu
  */


#include "str.h"
#include "const.h"
#include "dllist.h"
#include "scanner.h"
#include "ial.h"
#define BASE_POINTER 99

void init_list(t_list *l){
  l->first = NULL;
  l->last = NULL;
  l->bp = NULL;
}

void dispose_list(t_list *l){
  t_list_elem* pom;
  
  while (l->first != NULL)
  {
    pom = l->first;  
    l->first = l->first->rptr;
    if(pom->type == T_STRING){
        str_free(&(pom->item.sVal));
    } 
    free(pom);
  }
  l->bp = NULL;  
  l->first = NULL;
  l->last = NULL;
  return;
}

//Vlozeni base pointeru do seznamu
int list_insert_bp(t_list* l, t_list_elem* bp){
	t_list_elem* new_elem;
  if ((new_elem = malloc (sizeof(struct t_list_elem))) == NULL)
   return INT_ERR;
  else
  {
	new_elem->type = BASE_POINTER;
    new_elem->lptr = bp; 
    new_elem->rptr = NULL;  
    if (l->first == NULL)
      l->first = new_elem;  
    else
      l->last->rptr = new_elem;
    l->last = new_elem;
  }
  return SUCCESS;
}

int list_insert_last(t_list* l, tValue item, int union_type){	
  t_list_elem* new_elem;
  if ((new_elem = malloc (sizeof(struct t_list_elem))) == NULL)
   return INT_ERR;
  else
  {
    new_elem->type = union_type;
    new_elem->item = item;  
    new_elem->lptr = l->last; 
    new_elem->rptr = NULL;  
    if (l->first == NULL)
      l->first = new_elem;  
    else
      l->last->rptr = new_elem;
    l->last = new_elem;
  }
  return SUCCESS;
}

void list_delete_last (t_list *l){
  if (l->first != NULL)
  {
    t_list_elem* tmp;
    tmp = l->last;  
    if (l->first != l->last)  
    {
      l->last = l->last->lptr;
      l->last->rptr = NULL; 
    }
    else 
    {
      l->first = NULL; 
      l->last = NULL;
    }
    if(tmp->type == T_STRING){
        str_free(&(l->first->item.sVal));
    }
    free(tmp);
  }
  return;
}

t_list_elem* list_next_elem(t_list_elem* item){
	if(item != NULL)
		return item->rptr;
	else
		return NULL;
}


t_list_elem* list_add_offset(t_list_elem* item, int offset){
	t_list_elem* tmp = item;
	for(int i = 0; i < offset; i++){
		if(tmp != NULL)
			tmp = list_next_elem(tmp);
	}
	return tmp;
}

/**
 * Naalokuje prazdny ramec o var_count prvcich
 */
void prealloc_frame(t_list* list, int var_count){
	tValue item;
	for(int i = 0; i < var_count; i++){
		list_insert_last(list, item, 0);
		// nastaveni base pointeru na prvni prvek ramce
		if(i == 0)
			list->bp = list->last;
	}
}
	
/**
 * Funkce vytvari frame v t_list
 */ 
void fill_frame(tLTS* rootPtr, t_list* list) {
   if(rootPtr != NULL) {
      // rekurzivni zanoreni do leveho a potom praveho podstromu
      fill_frame(((rootPtr)->lPtr), list);
      fill_frame(((rootPtr)->rPtr), list);
      
	  t_list_elem* tmp;
	  tmp = (list_add_offset(list->bp, rootPtr->data.varOffset));
	  tmp->item = rootPtr->data.varValue;
	  tmp->type = rootPtr->data.varType;

      rootPtr = NULL;
   }
}
