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
 * Soubor prec_analyse.h
 *
 * Soubor s implementaci syntakticke analyzy zdola nahoru pomoci precedencni analyzy.
 */

#ifndef _PPARSER_H
#define _PPARSER_H

#include "prec_stack.h"

/**
 * Enum pomoci, ktereho se indexuje precedencni tabulka
 */
typedef enum {

  ADD=0,  // +
  SUB,    // -
  MUL,    // *
  IDIV,   // "\"
  DIV,    // "/"
  LEFT,   // (
  RIGHT,  // )
  ID,     // identifikator
  EQU,    // =
  NEQU,   // <>
  LOW,    // <
  LOWEQU, // <=
  GRE,    // >
  GREEQU, // >=
  END,    // $
  NONT = 200, //nonterminal
  NUM = 201, //nonterminal
  FLAG = 300,
  ERR = 400,
}E_INDEX;


/**
 * Hlavni funkce precedencni analyzy. Obsahuje automat s peti stavy, precedencni tabulku(urcuje 
 * jaky stav nastane), zasobnik(pro ukladani tokenu a jejich nasledne spracovani), semanticke akce(rozhoduji jake instrukce se vytvori).
 * Funkce vraci pomoci parametru vysledek derivacniho stromu.
 */
int parse_expr();
/**
 * Funkce ktera prevadi hodnotu tokenu na hodnotu indexu precedencni tabulky.
 */
int index(int);
/**
 * Funkce ktera vraci typ promene pomoci klice a kontroluje zda byla promena drive deklarovana.
 */
int get_types(tStack*, int* ,int*);

#endif
