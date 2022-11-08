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
#ifndef _DLLIST_H
#define	_DLLIST_H

#include <stdio.h>
#include <stdlib.h>
#include "ial.h"

typedef struct t_list_elem {               
        int    type;
        tValue item;                                         
        struct t_list_elem* lptr;     
        struct t_list_elem* rptr;        
} t_list_elem;

typedef struct {                                  
    t_list_elem* first;
    t_list_elem* bp;                     
    t_list_elem* last;                    
} t_list;

void init_list(t_list*);
void dispose_list(t_list*);
int list_insert_last(t_list* , tValue, int);
int list_insert_bp(t_list*, t_list_elem*);
void list_delete_last (t_list*);
t_list_elem* list_next_elem(t_list_elem*);
t_list_elem* list_add_offset(t_list_elem*, int);
void prealloc_frame(t_list*, int);
void fill_frame(tLTS*, t_list*);
void print_list(t_list*);

#endif
