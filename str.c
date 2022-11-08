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
  * Soubor str.c
  *
  * knihovna pro praci s potencionalne nekonecne dluhymi retezci
  */

#include <string.h>
#include <malloc.h>
#include "str.h"
#include "const.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR INT_ERR;
#define STR_SUCCESS SUCCESS;

/**
 * Funkce vytvori novy retezec
 * @param *s struktura retezce
 */
int str_init(string *s){
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->alloc_size = STR_LEN_INC;
   return STR_SUCCESS;
}

/**
 * Uvolni retezec z pameti
 * @param *s struktura retezce
 */
void str_free(string *s){
   free(s->str);
}

/**
 * Vymaze obsah retezce
 * @param *s struktura retezce
 */
void str_clear(string *s){
   s->str[0] = '\0';
   s->length = 0;
}

/** 
 * Funkce prida na konec retezce jeden znak
 * @param *s1 struktura retezce
 * @param c znak
 */
int str_add_char(string *s1, char c){
   if (s1->length + 1 >= s1->alloc_size)
   {
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->alloc_size = s1->length + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

/**
 * Prekopirovani retezce s2 do s1
 * @param *s1 struktura retezce
 * @param *s2 struktura retezce
 */
int str_copy_string(string *s1, string *s2){
   int new_length = s2->length;
   if (new_length >= s1->alloc_size)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, new_length + 1)) == NULL)
         return STR_ERROR;
      s1->alloc_size = new_length + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = new_length;
   return STR_SUCCESS;
}

/**
 * Prekopirovani retezce s2 do s1
 * @param s1 struktura retezce
 * @param s2 klasicke pole znaku
 */
int str_copy_array(string *s1, char s2[]) {
   str_clear(s1);
   int len = strlen(s2);
   for(int i = 0; i < len; i++) {
      if(str_add_char(s1, s2[i]) != SUCCESS)
         return INT_ERR;     
   }

   return SUCCESS;
}
/**
 * Pridani retezce s2 do s1
 * @param s1 struktura retezce
 * @param s2 klasicke pole znaku
 */
int str_add_array(string *s1, char s2[]) {
   //str_clear(s1);
   int len = strlen(s2);
   for(int i = 0; i < len; i++) {
      if(str_add_char(s1, s2[i]) != SUCCESS)
         return INT_ERR;     
   }

   return SUCCESS;
}


/**
 * Porovnani dvou retezcu
 * @param *s1 struktura retezce 1
 * @param *s2 struktura retezce 2
 */
int str_cmp_string(string *s1, string *s2){
	return strcmp(s1->str, s2->str);
}

/**
 * Porovnani retezce s konstantnim retezcem
 * @param *s1 struktura string
 * @param *s2 retezec
 */
int str_cmp_const_str(string *s1, char* s2){
	return strcmp(s1->str, s2);
}

/**
 * Vrati textovou cast retezce
 * @param *s struktura retezce
 */
char *str_get_str(string *s){
	return s->str;
}

/**
 * Vrati delku retezce
 * @param *s struktura retezce
 */
int str_get_length(string *s){
	return s->length;
}

/**
 * Konkatenuje dva retezce
 * @param *s1 prvni struktura retezce
 * @param *s2 druha struktura retezce
 * @param *s3 vysledek konkatenace
 */
