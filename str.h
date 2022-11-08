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
  * Soubor str.h
  *
  * knihovna pro praci s potencionalne nekonecne dluhymi retezci
  */

#ifndef _STR_H
#define _STR_H

typedef struct string
{
	char    *str;              // ukazatel na retezec ukonceny '\0'
	int		length;            // skutecna delka retezce
	int		alloc_size;        // velikost alokovane pameti pro retezec
} string;

int str_init(string *s);
void str_free(string *s);

void str_clear(string *s);
int str_add_char(string *s, char c);
int str_copy_string(string *s1, string *s2);
int str_copy_array(string *s1, char s2[]);
int str_add_array(string *s1, char s2[]);
int str_cmp_string(string *s1, string *s2);
int str_cmp_const_str(string *s1, char *s2);
int str_get_length(string *s);
char *str_get_str(string *s);

#endif
