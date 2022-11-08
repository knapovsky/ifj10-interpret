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
 * Soubor prec_analyse.c
 *
 * Soubor s implementaci syntakticke analyzy zdola nahoru pomoci precedencni analyzy.
 */

#include <stdio.h>
#include "const.h"
#include "ial.h"
#include "scanner.h"
#include "prec_analyse.h"
#include "str.h"
#include "prec_stack.h"
#include "ilist.h"

extern string attr;    // globalni promenna pro atribut tokenu
extern int token;      // poromena uchovavajici posledni token v SA
tStack tokeny;
extern int line;       // globalni promenna pro cislo zpracovavaneho radku
extern tGNode* tmp;    // globalni promenna uchovavajici ukazatel na uzel GTS
extern tLNode* lNode;
extern tGTS** globalTS;
extern tIList* list;

int parse_expr() {

char pre_table[15][15]={
//        0+  1-  2*  3\  4/  5(  6) 7ID  8= 9<> 10< 11<= 12> 13>= 14$
/* + */ {'>','>','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* - */ {'>','>','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* * */ {'>','>','>','>','>','<','>','<','>','>','>','>', '>','>','>'},
/* \ */ {'>','>','<','>','<','<','>','<','>','>','>','>', '>','>','>'},
/* / */ {'>','>','>','>','>','<','>','<','>','>','>','>', '>','>','>'},
/* ( */ {'<','<','<','<','<','<','=','<','<','<','<','<', '<','<','-'},
/* ) */ {'>','>','>','>','>',' ','>',' ','>','>','>','>', '>','>','>'},
/* ID */{'>','>','>','>','>',' ','>',' ','>','>','>','>', '>','>','>'},
/* = */ {'<','<','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* <> */{'<','<','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* < */ {'<','<','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* <= */{'<','<','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* > */ {'<','<','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* >= */{'<','<','<','<','<','<','>','<','>','>','>','>', '>','>','>'},
/* $ */ {'<','<','<','<','<','<','-','<','<','<','<','<', '<','<',' '}
};

  char p_symbol;
  string empty;
  str_init(&empty);
  tStack token_stack;
  int x=0;
  int prvni,treti,sama;
  tLNode *tln, *tln2, *tln3, *tln_prevod;

  InitList (&token_stack);
  InsertFirst (&token_stack, T_EOL,empty);
  FirstTerminal(&token_stack);

  while(!((index(token) == END) && (index(Copy(&token_stack))== END))){
  	if (index(token) == ERR){
      DisposeList (&token_stack);
      fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {operator|id|end}\n", line);
	  str_free(&empty);
	  return SYN_ERR;  
    }
	  
    FirstTerminal(&token_stack);
	p_symbol = pre_table[index(Copy(&token_stack))][index(token)];
	string tmp_attr;
    str_init(&tmp_attr);
  
	switch (p_symbol){
      case '=': //Shiftuj
        InsertFirst(&token_stack,token,attr);
        if((token = get_next_token(&attr)) == LEX_ERR) {
          DisposeList (&token_stack);
		  str_free(&empty);
	      return LEX_ERR;
        }
        else if(token == INT_ERR) {
		 str_free(&empty);
         DisposeList (&token_stack);
         return INT_ERR;
        }
		break;
	  case '<': //vloz za první terminal < a shiftuj
        FirstTerminal(&token_stack);
        PreInsert(&token_stack,FLAG,empty);
        InsertFirst(&token_stack,token,attr);
        if((token = get_next_token(&attr)) == LEX_ERR) {
          DisposeList (&token_stack);
		  str_free(&empty);
		  return LEX_ERR;
        }
        else if(token == INT_ERR) {
         DisposeList (&token_stack);
		 str_free(&empty);
         return INT_ERR;
        }
		break;
	  case '>':  //Redukuj
        x = FirstLower(&token_stack);
		int cislo;
		
		if (x==1){
		  if (((cislo=CopyFirst(&token_stack))== T_INT)||(cislo== T_REAL)||(cislo== T_REAL_EXP)||(cislo== T_HEXA)||(cislo== T_OCTAL)||(cislo== T_BIN)||(cislo==T_STR)){
		    First(&token_stack);
			tln3 = lts_set_const(globalTS, tmp, CopyFirst(&token_stack), CopyPtrAttr(&token_stack));
			DelettoLower (&token_stack);
            InsertFirst(&token_stack,NONT,tln3->key);
			break;
		  }          
		  First(&token_stack);
          if(((sama = lts_get_vartype(tmp, CopyPtrAttr(&token_stack)))==SEM_ERR)){
            str_copy_string(&tmp_attr, CopyPtrAttr(&token_stack));
			fprintf(stderr, "radek %d: Semanticka chyba. Neznamy identifikator '%s'.\n", line, str_get_str(&tmp_attr));
			str_free(&empty);
	        DisposeList (&token_stack);
            str_free(&tmp_attr);  
			return SEM_ERR;  
	      }
          First(&token_stack);
          str_copy_string(&tmp_attr, CopyPtrAttr(&token_stack));
		  DelettoLower (&token_stack);
          InsertFirst(&token_stack,NONT,tmp_attr);
		  First(&token_stack);
		  tln3 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
          break;
     	}
		else if (x==3){
		  First(&token_stack);
		  Succ(&token_stack);
           int t;
		   switch(Copy(&token_stack)){
              case T_E:
				  if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
                    str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_STRING   && treti == T_STRING    ){
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  //ziskani adresy1
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); //ziskani adresy2
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);       //vytvoreni adresy3
					listInsertLast(list, I_E_S, tln, tln2, tln3);        //ulozeni instrukce
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); 
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);            
					listInsertLast(list, I_E_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_E_I, tln, tln2, tln3);
				  }
				  else{
				    fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest porovnani nad temito datovymi typy.\n", line);
				    DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
			        return SEM_ERR;
				  }
			      DelettoLower (&token_stack);
				  InsertFirst(&token_stack,NONT,tln3->key); 
                  break;
              case NONT:
				 First(&token_stack);Succ(&token_stack);
				 str_copy_string(&tmp_attr, CopyPtrAttr(&token_stack));
			     DelettoLower (&token_stack);
			     InsertFirst(&token_stack,NONT,tmp_attr);
                break;
              case T_ADD:			 
                  if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_INTEGER && treti == T_DOUBLE){
				    //prevod do double
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln_prevod = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_INT2DBL, tln, NULL, tln_prevod);
					//vytvoreni instrukce pro soucet
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_ADD_D, tln_prevod, tln2, tln3);
				  }
                  else if (prvni == T_DOUBLE && treti == T_INTEGER  ){
				    //prevod do double
					First(&token_stack);
					Succ(&token_stack); Succ(&token_stack);
                    tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln_prevod = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_INT2DBL, tln2, NULL, tln_prevod);
					//vytvoreni instrukce pro soucet
					First(&token_stack);
					tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_ADD_D, tln, tln_prevod, tln3);
				  }
				  else if (prvni == T_STRING   && treti == T_STRING){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_STRING);
					listInsertLast(list, I_KONK, tln, tln2, tln3);
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_ADD_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_ADD_I, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest scitani nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
               str_free(&tmp_attr);  
					 return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_SUB:         
				 if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_INTEGER && treti == T_DOUBLE){
				    //prevod do double
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln_prevod = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_INT2DBL, tln, NULL, tln_prevod);
					//vytvoreni instrukce pro soucet
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_SUB_D, tln_prevod, tln2, tln3);
				  }
                  else if (prvni == T_DOUBLE && treti == T_INTEGER){
				    //prevod do double
					First(&token_stack);
					Succ(&token_stack); Succ(&token_stack);
                    tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln_prevod = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_INT2DBL, tln2, NULL, tln_prevod);
					//vytvoreni instrukce pro soucet
					First(&token_stack);
					tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_SUB_D, tln, tln_prevod, tln3);
				  }
				    else if (prvni == T_DOUBLE && treti == T_DOUBLE){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_SUB_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_SUB_I, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest odcitani nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
               str_free(&tmp_attr);  
					 return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_MUL:           
				 if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_INTEGER && treti == T_DOUBLE){
				    //prevod do double
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln_prevod = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_INT2DBL, tln, NULL, tln_prevod);
					//vytvoreni instrukce pro soucet
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_MUL_D, tln_prevod, tln2, tln3);
				  }
                  else if (prvni == T_DOUBLE && treti == T_INTEGER){
				    //prevod do double
					First(&token_stack);
					Succ(&token_stack); Succ(&token_stack);
                    tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln_prevod = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_INT2DBL, tln2, NULL, tln_prevod);
					//vytvoreni instrukce pro soucet
					First(&token_stack);
					tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_MUL_D, tln, tln_prevod, tln3);
				  }
				    else if (prvni == T_DOUBLE && treti == T_DOUBLE){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_MUL_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_MUL_I, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest nasobeni nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
               str_free(&tmp_attr);  
					 return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_IDIV:
                  if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_INTEGER && treti == T_INTEGER){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_IDIV, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest celociselne deleni nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
               str_free(&tmp_attr);  
					 return SEM_ERR;
				  }

				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_DIV:                
				  if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni != T_STRING && treti != T_STRING){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
					listInsertLast(list, I_DIV, tln, tln2, tln3);
				  }
				  else{
				    DisposeList (&token_stack);
					fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest deleni nad temito datovymi typy.\n", line);
				    DisposeList (&token_stack);
					str_free(&empty);
                    str_free(&tmp_attr);  
					return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_G:
				  if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
                    str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_STRING   && treti == T_STRING    ){
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  //ziskani adresy1
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); //ziskani adresy2
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);            //vytvoreni adresy3
					listInsertLast(list, I_G_S, tln, tln2, tln3);            //ulozeni instrukce
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); 
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);            
					listInsertLast(list, I_G_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_G_I, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest porovnani nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
               str_free(&tmp_attr);  
					 return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_GE:
				 if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_STRING   && treti == T_STRING    ){
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  //ziskani adresy1
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); //ziskani adresy2
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);       //vytvoreni adresy3
					listInsertLast(list, I_GE_S, tln, tln2, tln3);       //ulozeni instrukce
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); 
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);            
					listInsertLast(list, I_GE_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_GE_I, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest porovnani nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
               str_free(&tmp_attr);  
					 return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_L:
                 if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_STRING   && treti == T_STRING    ){
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  //ziskani adresy1
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); //ziskani adresy2
					tln3 = lts_set_varnode(globalTS, tmp, T_STRING);       //vytvoreni adresy3
					listInsertLast(list, I_L_S, tln, tln2, tln3);        //ulozeni instrukce
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); 
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);            
					listInsertLast(list, I_L_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_L_I, tln, tln2, tln3);
				  }
				  else{
				     fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest porovnani nad temito datovymi typy.\n", line);
				     DisposeList (&token_stack);
					 str_free(&empty);
                     str_free(&tmp_attr);  
					 return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
              case T_LE:
				 if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_STRING   && treti == T_STRING    ){
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  //ziskani adresy1
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); //ziskani adresy2
					tln3 = lts_set_varnode(globalTS, tmp, T_STRING);       //vytvoreni adresy3
					listInsertLast(list, I_LE_S, tln, tln2, tln3);       //ulozeni instrukce
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); 
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);            
					listInsertLast(list, I_LE_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_LE_I, tln, tln2, tln3);
				  }
				  else{
				    fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest porovnani nad temito datovymi typy.\n", line);
				    DisposeList (&token_stack);
					str_free(&empty);
               str_free(&tmp_attr);  
					return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
			  case T_NE:
				 if ((t = get_types(&token_stack, &prvni, &treti))!=SUCCESS){
					DisposeList (&token_stack);
					str_free(&empty);
                    str_free(&tmp_attr);  
					return t;
				  }
				  if (prvni == T_STRING   && treti == T_STRING    ){
					First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  //ziskani adresy1
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); //ziskani adresy2
					tln3 = lts_set_varnode(globalTS, tmp, T_STRING);       //vytvoreni adresy3
					listInsertLast(list, I_NE_S, tln, tln2, tln3);       //ulozeni instrukce
				  }
				  else if (prvni == T_DOUBLE && treti == T_DOUBLE    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));  
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack)); 
					tln3 = lts_set_varnode(globalTS, tmp, T_DOUBLE);            
					listInsertLast(list, I_NE_D, tln, tln2, tln3);
				  }
				  else if (prvni == T_INTEGER && treti == T_INTEGER    ){
				    First(&token_stack);
                    tln = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					Succ(&token_stack); Succ(&token_stack);
					tln2 = lts_get_varnode(tmp,CopyPtrAttr(&token_stack));
					tln3 = lts_set_varnode(globalTS, tmp, T_INTEGER);
					listInsertLast(list, I_NE_I, tln, tln2, tln3);
				  }
				  else{
					DisposeList (&token_stack);
				    fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest porovnani nad temito datovymi typy.\n", line);
					str_free(&empty);
               str_free(&tmp_attr);  
				    return SEM_ERR;
				  }
				 DelettoLower (&token_stack);
				 InsertFirst(&token_stack,NONT,tln3->key); 
                 break;
		    }//konec switch(Copy(&token_stack)){
		  }
		  else{
		    DisposeList (&token_stack);
		    fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {vyraz}\n", line);
			str_free(&empty);
            str_free(&tmp_attr);  
		    return SYN_ERR;
		  }//konec if (x==1){
          break;
	    case ' ':
          DisposeList (&token_stack);
		  fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {operator}\n", line);
		  str_free(&empty);
         str_free(&tmp_attr);  
		  return SYN_ERR;
        case '-':
          DisposeList (&token_stack);
		  fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {zavorka}\n", line);
		  str_free(&empty);
         str_free(&tmp_attr);  
		  return SYN_ERR;
	}//konec switch (p_symbol){
    FirstTerminal(&token_stack);
    str_free(&tmp_attr);  
  }
  lNode = tln3;
  DisposeList (&token_stack);
  str_free(&empty);
  return SUCCESS;
}

int index(int val_token){
  switch (val_token){
    case T_E:
      return EQU;
    case T_ID:
	case T_INT:                
	case T_REAL:           
	case T_REAL_EXP: 
	case T_HEXA:            
	case T_OCTAL:     
	case T_BIN:
   case T_STR:
      return ID;
	case T_SEMI:
    case T_EOL:
    case T_THEN:
      return END;
    case T_LEFT:
      return LEFT;
    case T_RIGHT:
      return RIGHT;
    case T_G:
      return GRE;
    case T_ADD:                 
      return ADD;
    case T_SUB:
      return SUB;                
    case T_MUL:
      return MUL;                 
    case T_IDIV:
      return IDIV;                
    case T_DIV:
      return DIV;               
    case T_GE:
      return GREEQU;             
    case T_L:
      return LOW;                
    case T_LE:
      return LOWEQU;                 
    case T_NE:
      return NEQU;        
    default:
      //chyba
    return ERR;
  }
}

int get_types(tStack *token_stack, int* prvni, int* treti){
	string tmp_attr;
	First(token_stack);
	if(((*prvni = lts_get_vartype(tmp, CopyPtrAttr(token_stack)))==SEM_ERR)){
      tmp_attr = CopyAttr(token_stack);
	  fprintf(stderr, "radek %d: Semanticka chyba. Neznamy identifikator '%s'.\n", line, str_get_str(&tmp_attr));
	  return *prvni;  
	}
	else if (*prvni ==INT_ERR){
	  tmp_attr = CopyAttr(token_stack);
	  //fprintf(stderr, "radek %d: Semanticka chyba. Neznamy identifikator '%s'.\n", line, str_get_str(&tmp_attr));
	  return *prvni;
	}
	Succ(token_stack);
	Succ(token_stack);
	if(((*treti = lts_get_vartype(tmp, CopyPtrAttr(token_stack)))==SEM_ERR)){
      tmp_attr = CopyAttr(token_stack);
	  fprintf(stderr, "radek %d: Semanticka chyba. Neznamy identifikator '%s'.\n", line, str_get_str(&tmp_attr));
	  return *prvni;  
	}
	else if (*treti ==INT_ERR){
	  tmp_attr = CopyAttr(token_stack);
	  //fprintf(stderr, "radek %d: Semanticka chyba. Neznamy identifikator '%s'.\n", line, str_get_str(&tmp_attr));
	  return *prvni;
	}


   return SUCCESS;
}

