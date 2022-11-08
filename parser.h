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
 * Soubor parser.h
 *
 * Hlavickovy soubor pro funkce syntakticke analyzy.
 */

#ifndef _PARSER_H
#define _PARSER_H

#include "str.h"
#include "ial.h"

// prototypy funkci pro SA shora dolu
int parse(tGTS**, tIList*);
int decl(void);
int dc_list(void);
int ex_list(void);
int fn_list(void);
int func(void);
int id_list(void);
int it_list(void);
int it_list2(void);
int main_p(void);
int param(tGNode*);
int pm_list(tGNode*);
int pm_list2(tGNode*);
int prog(void);
int rest(tGNode*);
int rhs(int*);
int stat(void);
int st_list(void);
int type(void);

#endif
