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
  * Modul interpretu
  */

#ifndef _INTERPRET_T
#define _INTERPRET_T

#include <stdlib.h>
#include <stdio.h>
#include "dllist.h"
#include "ial.h"
#include "ilist.h"

t_list_elem* get_list_elem(tLNode*, t_list*);
int interpret(tGTS*, tIList*);

#endif
