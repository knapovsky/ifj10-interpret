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
  * Soubor interpret.c
  *
  * Modul interpretu
  */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "interpret.h"
#include "ilist.h"
#include "const.h"
#include "dllist.h"
#include "ial.h"
#include "scanner.h"

  t_list_elem* get_list_elem(tLNode* addr, t_list *list){
	int offset;
	if(addr != NULL){
		offset = addr->data.varOffset;
		return list_add_offset(list->bp, offset);
	}
	else
		return NULL;
}

int interpret(tGTS *rootPtr, tIList *L)
{
  //Pomocne promenne
  int int_prom, c;
  double double_prom;
  tListItem* item_address;
  char *str;
  tLNode* ptr2;
 
  t_list_elem* target, *target2, *target3;
  target = NULL;
  target2 = NULL;
  target3 = NULL;

  tGNode *ptr = NULL;	//pomocny ukazatel na vyhledavani v GTS

  t_list *list; // "zasobnik"
  list = (t_list *) malloc(sizeof(t_list));	//alokace zasobniku
  init_list(list);  //inicializace zasobniku
  //void *addr1, *addr2, *addr3;
  
  string pom;  
  str_init(&pom);
  str_copy_array(&pom, "$SCOPE");
  ptr = gts_search(rootPtr, &pom); // vyhledani hlavni funkce
  str_free(&pom);
  
  L->active = ptr->data.fInstr;	//nastaveni aktivni intrukce na hlavni funkci

  list->bp = list->first;	//nastaveni BP (base pointeru)

  //prvotni naplneni zasobniku
  prealloc_frame(list, ptr->data.varCount);
  fill_frame(ptr->data.localTS, list);
  
  /*for (int i = 0; i < ptr->data.varCount; i++) {
    list_insert_last(list, ptr->data.localTS->data.varValue, ptr->data.localTS->data.varType);
    //inkrementace ukazatele na prvek lokalni tabulky symbolu
    ptr++;
  }*/
  int s = 0;
  while (L->active->Instruction.instType != I_STOP) {
	  
    switch(L->active->Instruction.instType) {
	    
	    
	    case I_START:
	    if(s > 0){
        dispose_list(list);
        return RUN_ERR;
      }else{
        s++;
      }
	    break;
	    
	    /**Precte cislo typu INT a ulozi 
	    *a ulozi ho do addr3
	    */
	    case I_READ_I:
		    //Nacitani bilych znaku
		    while(c = getchar(), isspace(c)){
			    if(c == '\n'){
				    fprintf(stderr, "Nebyl zadan zadny vstup. Ocekavana hodnota typu INT\n");
				     dispose_list(list);
				    return RUN_ERR;
			    }
		    }
		    //Narazili jsme na prvni nebily znak - vratime ho zpet
		    ungetc(c, stdin);
		    //Pokud zadane cislo neni integer
		    if(scanf("%d", &int_prom) == 0){
          fprintf(stderr, "Ocekavana hodnota typu INT.\n");
           dispose_list(list);
          return RUN_ERR;
        }
        //Nyni kouknem,co nasleduje. Je-li promenna typu double a precetli jsme cislo typu INT,zbytek zahodime.
        while(c = getchar(), !isspace(c)){
        }
        ungetc(c, stdin);

        //Zde jsme nacetli zadanou hodnotu do int_prom a musi nasledovat bily znak
        if(c = getchar(), !isspace(c)){
          //Za cislem neni bily znak
          fprintf(stderr, "Za cislem nenasleduje bily znak.\n");
           dispose_list(list);
          return RUN_ERR;
        }
        //Zapsani nacteneho integeru do listu
        target = get_list_elem(L->active->Instruction.addr3, list);
        target->item.iVal = int_prom;
	      break;

	  case I_READ_D:
	  	while(c = getchar(), isspace(c)){
		  	if(c == '\n') {
			  	fprintf(stderr, "Nebyl zadan zadny vstup. Ocekavana hodnota typu DOUBLE\n");
			  	 dispose_list(list);
			  	return RUN_ERR;
		  	}
	  	}
		  ungetc(c, stdin);
		  if (scanf("%lf", &double_prom) == 0){
			  fprintf(stderr, "Ocekavana hodnota typu DOUBLE.\n");
			   dispose_list(list);
			  return RUN_ERR;
	  	}
	  	target = get_list_elem(L->active->Instruction.addr3, list);
	  	target->item.dVal = double_prom;
	    break;

    case I_WRITE_I:
		target = get_list_elem(L->active->Instruction.addr1, list);
		printf("% d", target->item.iVal);
        break;
    
    case I_WRITE_D:
		target = get_list_elem(L->active->Instruction.addr1, list);
		printf("% g", target->item.dVal);
		break;
	
	case I_WRITE_S:
		target = get_list_elem(L->active->Instruction.addr1, list);
		str = str_get_str(&target->item.sVal);
		printf("%s", str);
		break;

    case I_INC:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->type == T_INTEGER){
          target3->item.iVal = target->item.iVal + 1;
        }
        if(target->type == T_DOUBLE){
          target3->item.dVal = target->item.dVal + 1;
        }
        if(target->type == T_STRING){
         dispose_list(list);
          return RUN_ERR;
        }
        break;

      case I_DEC:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->type == T_INTEGER){
          target3->item.iVal = target->item.iVal - 1;
        }
        if(target->type == T_DOUBLE){
          target3->item.dVal = target->item.dVal - 1;
        }
        if(target->type == T_STRING){
         dispose_list(list);
          return RUN_ERR;
        }
        break;

      case I_NOT:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal == 0){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
      
      // EQUAL
      case I_E_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal == target2->item.iVal){
          target3->item.iVal = 1;      
        }else{
          target3->item.iVal = 0;      
        }
        break;
        
      case I_E_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.dVal == target2->item.dVal){
          target3->item.iVal = 1;      
        }else{
          target3->item.iVal = 0;      
        }
        break;
        
      case I_E_S:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(str_cmp_string(&(target->item.sVal), &(target2->item.sVal)) == 0){
          target3->item.iVal = 1;      
        }else{
          target3->item.iVal = 0;      
        }
        break;
        
      // ADD
      case I_ADD_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.iVal = target->item.iVal + target2->item.iVal;
        break;
        
      case I_ADD_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.dVal = target->item.dVal + target2->item.dVal;
        break;
        
      // SUBTRACTION
      case I_SUB_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.iVal = target->item.iVal - target2->item.iVal;
        break;
        
      case I_SUB_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.dVal = target->item.dVal - target2->item.dVal;
        break;

      // MULTIPLICATION
      case I_MUL_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.iVal = target->item.iVal * target2->item.iVal;
        break;
      case I_MUL_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.dVal = target->item.dVal * target2->item.dVal;
        break;
        
        // DIVISION OF INTEGERS
      case I_IDIV:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target2->item.iVal != 0){
          target3->item.iVal = target->item.iVal / target2->item.iVal;
        }else{
         dispose_list(list);
          return RUN_ERR;
        }
        break;
      
      // DIVISION OF DOUBLES
      case I_DIV:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
          
          if(target->type == T_INTEGER){
            if(target2->type == T_INTEGER){
              if(target2->item.iVal == 0){
               dispose_list(list);
                return RUN_ERR;
              }else{
                target3->item.iVal = target->item.iVal / target2->item.iVal; 
              }            
            }else if(target2->type == T_DOUBLE){
              if(target2->item.dVal == 0){
               dispose_list(list);
                return RUN_ERR;
              }else{
                target3->item.dVal = target->item.iVal / target2->item.dVal; 
              }           
            }
          }else if(target->type == T_DOUBLE){
            if(target2->type == T_INTEGER){
              if(target2->item.iVal == 0){
               dispose_list(list);
                return RUN_ERR;
              }else{
                target3->item.dVal = target->item.dVal / target2->item.iVal; 
              }            
            }else if(target2->type == T_DOUBLE){
              if(target2->item.dVal == 0){
               dispose_list(list);
                return RUN_ERR;
              }else{
                target3->item.dVal = target->item.dVal / target2->item.dVal; 
              }           
            }          
          }
          
        break;
        
      // GREATER
      case I_G_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal > target2->item.iVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_G_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.dVal > target2->item.dVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_G_S:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(str_cmp_string(&(target->item.sVal),&(target2->item.sVal)) > 0){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
   
      
      // GREATER OR EQUAL
      case I_GE_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal >= target2->item.iVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_GE_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.dVal >= target2->item.dVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_GE_S:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(str_cmp_string(&(target->item.sVal),&(target2->item.sVal)) >= 0){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      // LESSER
      case I_L_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal < target2->item.iVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_L_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.dVal < target2->item.dVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_L_S:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(str_cmp_string(&(target->item.sVal),&(target2->item.sVal)) < 0){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      // LESSER OR EQUAL
      case I_LE_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal <= target2->item.iVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_LE_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.dVal <= target2->item.dVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_LE_S:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(str_cmp_string(&(target->item.sVal),&(target2->item.sVal)) <= 0){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
      
      // NOT EQUAL
      case I_NE_I:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.iVal != target2->item.iVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_NE_D:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(target->item.dVal != target2->item.dVal){
          target3->item.iVal = 1;
        }else{
          target3->item.iVal = 0;
        }
        break;
        
      case I_NE_S:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        if(str_cmp_string(&(target->item.sVal), &(target2->item.sVal)) != 0){
          target3->item.iVal = 1;      
        }else{
          target3->item.iVal = 0;      
        }
        break;
        
      // INT2DOUBLE
      case I_INT2DBL:
        target = get_list_elem(L->active->Instruction.addr1,list);
        target3 = get_list_elem(L->active->Instruction.addr3,list);
        target3->item.dVal = (double) target->item.iVal;
        break;
        
      // CONCATENTATION
      case I_KONK:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        str_clear(&(target3->item.sVal));
        str_add_array(&(target3->item.sVal),str_get_str(&(target->item.sVal)));
        target3->item.sVal.length -= 1;
        str_add_array(&(target3->item.sVal),str_get_str(&(target2->item.sVal)));
        
        break;
      
      case I_MOV:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
      if(target->type == T_INTEGER){
        target3->item.iVal = target->item.iVal;
      }
      if(target->type == T_DOUBLE){
        target3->item.dVal = target->item.dVal;
      }
      if(target->type == T_STRING){
        str_copy_string(&(target3->item.sVal),&(target->item.sVal));
      }
        break;
      
      case I_FIND:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target2 = get_list_elem(L->active->Instruction.addr2, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.iVal = find(&(target->item.sVal), &(target2->item.sVal));
        break;

      case I_SORT:
        target = get_list_elem(L->active->Instruction.addr1, list);
        target3 = get_list_elem(L->active->Instruction.addr3, list);
        target3->item.sVal = sort(&(target->item.sVal));

       break;
       
      case I_FCALL:
		//uchovani ukazatele na prvni instrukci funkce
		item_address = ((tGNode* )L->active->Instruction.addr1)->data.fInstr; 
		//uchovani ukazatele na lokalni tabulku symbolu funkce
		ptr2 = ((tGNode* )L->active->Instruction.addr1)->data.localTS;
		//nastaveni ukazatele na instrukci
		if(item_address != NULL)
			L->active = item_address;
		//nastaveni base pointeru a jeho uchovani
		list_insert_bp(list, list->bp);
		//nakopirovani lokalni tabulky symbolu
            prealloc_frame(list, ptr->data.argCount + ptr->data.varCount);
            fill_frame(((tGNode* )(L->active->Instruction.addr1))->data.localTS, list);
		
		
	  break;
	  
	  case I_JMP:
		//zmena ukazatele na instrukci
		//rozhodovani podle 1 a 0
		//(addr1 = 1/0, addr2 = adresa)
		if((((tLNode* )L->active->Instruction.addr1)->data.varValue.iVal) != 0)
			L->active = (tListItem* )L->active->Instruction.addr2;
	  break;
	  
	  case I_UNJMP:
		//zmena ukazatele na instrukci
		//musi se skocit vzdy
		//v addr1 je adresa instrukce
		L->active = (tListItem* )L->active->Instruction.addr1;
	  break;
	  
	  case NAV_WHILE:
		  //navesti while
	    break;
   
    case I_INPUT:
      fprintf(stdout, "? ");
      break;
   }
  //Automaticky se presuneme na interpretaci dalsi instrukce POKUD NENI PRIKAZEM NEJAKY SKOK
  if(L->active->Instruction.instType == I_JMP || L->active->Instruction.instType == I_FCALL || L->active->Instruction.instType == I_UNJMP){
  
  }else{
    listNext(L);
  }
  }
  dispose_list(list);
return SUCCESS;
}
