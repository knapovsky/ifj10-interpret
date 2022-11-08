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
 * Soubor parser.c
 *
 * Soubor s implementaci syntakticke analyzy shora dolu metodou
 * rekurzivniho sestupu. A precedencni syntakticke analyzy pro
 * zpracovani vyrazu.
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "const.h"
#include "ial.h"
#include "scanner.h"
#include "parser.h"
#include "prec_analyse.h"
#include "ilist.h"

extern int line;  // promenna uchovavajici cislo aktualniho radku vstupniho souboru
tGTS** globalTS;  // globalni promenna pro globalni tabulku symbolu
tGNode* tmp;      // globalni promenna pro uzel globalni tabulky symbolu
tLNode* lNode;    // globalni promenna pro vysledny uzel z parse_expr()
tIList* list;     // globalni promenna pro seznam instrukci
int token;        // globalni promenna pro aktualni token
string attr;      // globalni promenna pro atribut tokenu
bool isDecl = false;
int paramCount = 0;

/* ############################################################# */
/* ##### Sekce implementace SA metodou rekurzivnho sestupu ##### */
/* ############################################################# */

/**
 * Funkce spoustici syntaktickou analyzu
 * @param tabPtr ukazatel na tabulku symbolu
 * @param instPtr ukazatel na seznam instrukci
 * @return SUCCESS pri uspechu
 *         LEX_ERR pri lexikalni chybe
 *         SYN_ERR pri syntakticke chybe
 *         SEM_ERR pri semanticke chybe
 *         INT_ERR pri ostatnich chybach
 */
int parse(tGTS** tabPtr, tIList* listPtr) {
   int result = SUCCESS;
   globalTS = tabPtr;   // prirazeni uk. na tab. symbolu
   list = listPtr;      // prirazeni uk. na list instrukci

   if(str_init(&attr) == INT_ERR) {  // inicializace struktury pro atribut tokenu
      #ifdef DEBUG
         fprintf(stderr, "parser.c parse(): Nepodarilo se inicializovat strukturu pro atribut tokenu.\n");
      #endif
      return INT_ERR;
   }
  
   // nacteni prvniho tokenu (preskoci uvodni T_EOL tokeny) a test na chyby
   do {
      if((token = get_next_token(&attr)) == LEX_ERR) {
         result = LEX_ERR;
      }
      else if(token == INT_ERR) {
         result = INT_ERR;
      }
   } while(token == T_EOL);

   if(result == SUCCESS) {
      result = prog();
   }
  
   str_free(&attr);
   return result;
}

// funkce pro nonterminal <DECL> - SYN_OK, SEM_OK, INSTR_NONE
int decl(void) {
   int result;
   int tmp_token;
   string tmp_attr;
   
   if(str_init(&tmp_attr) != SUCCESS)
      return INT_ERR;
      
   switch(token) {
      // simulace pravidla: <DECL> -> dim id as <TYPE>
      case T_DIM:
         // nacteni tokenu - musi byt T_ID
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR; 
         }


         if(token != T_ID) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator}.\n", line);
            str_free(&tmp_attr);
            return SYN_ERR;
         }
         
         // ulozeni atributu s jmenem identifikatoru lokalni promenne
         if(str_copy_string(&tmp_attr, &attr) != SUCCESS) {
            str_free(&tmp_attr);
            return INT_ERR;
         }
         
         // nacteni tokenu - musi byt T_AS     
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR; 
         }
            
         if(token != T_AS) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {as}.\n", line);
            str_free(&tmp_attr);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <TYPE>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR;
         }
            
         // ulozeni tokenu s typem parametru
         tmp_token = token;   
         
         result = type();
         if(result != SUCCESS) {
            str_free(&tmp_attr);
            return result;
         }
         
         // pridani parametru do tabulky symbolu, kontrola jmena (nesmi byt stejne jako nazev funkce ani jako nazev jine funkce)
         result = gts_set_var(globalTS, tmp, tmp_token, &tmp_attr);
         if(result != SUCCESS) {
            if(result == SEM_ERR)
               fprintf(stderr, "radek %d: Semanticka chyba. Promenna '%s' ve funkci '%s' byla jiz deklarovana nebo je deklarovana funkce se stejnym jmenem.\n", line, str_get_str(&tmp_attr), str_get_str(&(tmp->key)));
            str_free(&tmp_attr);
            return result;
         }
         
         str_free(&tmp_attr);
         return result;
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {dim}.\n", line);
   str_free(&tmp_attr);
   return SYN_ERR;   
}

// funkce pro nonterminal <DC-LIST> - SYN_OK, INSTR_NONE
int dc_list(void) {
   int result;

   switch(token) {
      // simulace pravidla: <DC-LIST> -> <DECL> eol <DC-LIST>
      case T_DIM:
         // volani fce pro <DECL>
         result = decl();
         if(result != SUCCESS) return result;
         
         // token nacten z fce decl() - musi byt T_EOL
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a rekurzivni volani fce pro <DC-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return dc_list();
      break;
      // simulace pravidla 14: <DC-LIST> -> eps
      case T_DO:
      case T_END:
      case T_ID:
      case T_IF:
      case T_INPUT:
      case T_PRINT:
      case T_RETURN:
         // konec rekurze
         return SUCCESS;    
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {dim|do|end|id|if|input|print|return}.\n", line);
   return SYN_ERR;
}

// funkce pro nonterminal <EX-LIST> - SYN_OK, INST_OK
int ex_list(void) {
   int result;
   switch(token) {
      // simulace pravidla: <EX-LIST> -> <EXPR> ; <EX-LIST>
      default: // default, protoze nevim co za token je ve vyrazu
         // predani rizeni precedencni SA pro vyrazy
         result = parse_expr();
         if(result != SUCCESS) return result;
         
         switch(lNode->data.varType) {
            case T_INTEGER:
               listInsertLast(list, I_WRITE_I, lNode, NULL, NULL); 
               break;
            case T_DOUBLE:
               listInsertLast(list, I_WRITE_D, lNode, NULL, NULL);
               break;
            case T_STRING:
               listInsertLast(list, I_WRITE_S, lNode, NULL, NULL);
               break;
         }

         // token nacteny z fce parse_expr() - musi byt T_SEMI
         if(token != T_SEMI) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {;}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a rekurzivni volani fce pro <EX-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return ex_list();
      break;
      // simulace pravidla: <EX-LIST> -> eps
      case T_EOL:
         // konec rekurze
         return SUCCESS;    
      break;
   }
   
   // zde nemohu zjistit pri kterem tokenu je syntakticka chyba, protoze nevim, ktery token
   // patri do vyrazu, vim jen, ze T_EOL ukoncuje rekurzi, o syntakticke chyby se musi 
   // postarat precedencni SA pro vyrazy
}

// funkce pro nonterminal <FN-LIST> - SYN_OK, INTR_NONE
int fn_list(void) {
   int result;
   switch(token) {
      // simulace pravidla: <FN-LIST> -> <FUNC> eol <FN-LIST>
      case T_DECLARE:
      case T_FUNCTION:
         // volani fce pro <FUNC>
         result = func();
         if(result != SUCCESS) return result;
         
         // token nacteny z fce func() - musi byt T_EOL
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a rekurzivni volani fce pro <FN-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return fn_list();
      break;
      // simulace pravidla: <FN-LIST> -> eps
      case T_SCOPE:
         // konec rekurze
         return SUCCESS;
      break;
   }
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {declare|function|scope} kokot.\n", line);
   return SYN_ERR;    
}

// funkce pro nonterminal <FUNC> - SYN_OK, SEM_OK, INSTR_OK
int func(void) {
   int result;
   int tmp_token;
   switch(token) {
      // simulace pravidla: <FUNC> -> declare function id ( <IT-LIST> ) as <TYPE>
      case T_DECLARE:
         // nacteni tokenu - musi byt T_FUNCTION     
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_FUNCTION) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {function}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_ID
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_ID) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator}.\n", line);
            return SYN_ERR;
         }
         
         // vlozeni deklarace funkce do globalni TS
         result = gts_insert(globalTS, &attr, &tmp);
         if(result == TAB_INSERT_FAIL) {
            fprintf(stderr, "radek %d: Semanticka chyba. Funkce '%s' jiz byla deklarovana/definovana nebo je identifikator jiz rezervovan.\n", line, str_get_str(&attr));
            return SEM_ERR;
         }
         else if(result == TAB_INSERT_EMEM) {
            return INT_ERR;
         }
         
         // nacteni tokenu - musi byt T_LEFT      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_LEFT) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {(}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <IT-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = it_list();
         if(result != SUCCESS) return result;
         
         // token nacten z fce it_list() - musi byt T_RIGHT
         if(token != T_RIGHT) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {)}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_AS      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_AS) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {as}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <TYPE>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         // ulozeni tokenu s typem navratove hodnoty funkce
         tmp_token = token;
         
         result = type();
         if(result != SUCCESS) return result;
         
         // pridani navratoveho typu funkce do tabulky symbolu
         return gts_set_retval(tmp, tmp_token);
      break;
      // simulace pravidla: <FUNC> -> function id ( <IT-LIST> ) as <TYPE> eol <DC-LIST> <ST-LIST> end function   
      case T_FUNCTION:
         // nacteni tokenu - musi byt T_ID    
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_ID) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator}.\n", line);
            return SYN_ERR;
         } 

         // vyhledani deklarace funkce v globalni TS
         tmp = gts_search(*globalTS, &attr);
         if(tmp == NULL) {
            // neni v tabulce -> vlozeni deklarace funkce do globalni TS
            result = gts_insert(globalTS, &attr, &tmp);
            if(result == TAB_INSERT_FAIL) {
               fprintf(stderr, "radek %d: Semanticka chyba. Funkce '%s' jiz byla deklarovana/definovana nebo je identifikator jiz rezervovan.\n", line, str_get_str(&attr));
               return SEM_ERR;
            }
            else if(result == TAB_INSERT_EMEM) {
               return INT_ERR;
            }
         }
         else {
            // uz byla deklarovana, kontrola zda nebyla definovana
            result = gts_check_def(tmp);
            if(result == INT_ERR)
               return INT_ERR;
            else if(result == 1) {
               fprintf(stderr, "radek %d: Semanticka chyba, funkce '%s' jiz byla definovana.\n", line, str_get_str(&attr));
               return SEM_ERR;
            }
            
            // nastaveni, ze funkce jiz je deklarovana
            // parametry a navratovy datovy typ se budou jen kontrolovat, ne vkladat
            isDecl = true;
         }
                  
         // nacteni tokenu - musi byt T_LEFT      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_LEFT) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {(}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <IT-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = it_list();
         if(result != SUCCESS) return result;
         
         // token nacten z fce it_list() - musi byt T_RIGHT
         if(token != T_RIGHT) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {)}.\n", line);
            return SYN_ERR;
         }
        
         // nacteni tokenu - musi byt T_AS      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_AS) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {as}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <TYPE>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         
         tmp_token = token;

         result = type();
         if(result != SUCCESS) return result;
          
         if(isDecl) {
            // funkce jiz byla deklarovana, pouze kontrola navratoveho typu fce
            result = gts_check_retval(tmp, tmp_token);
            if(result != SUCCESS) {
               if(result == SEM_ERR)
                  fprintf(stderr, "radek %d: Semanticka chyba. Navratovy typ funkce '%s' se lisi od deklarace.\n", line, str_get_str(&(tmp->key)));
               return result;   
            }
            isDecl = false;
            gts_set_def(tmp);               
         }
         else {
            // pridani navratoveho typu fce do tabulky symbolu
            result = gts_set_retval(tmp, tmp_token);
            if(result != SUCCESS) {
               return result;
            }
            gts_set_def(tmp);
         }
         
         // token nacten z fce type() - musi byt T_EOL
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nagenerovani prvni instrukce pro danou funkci a ulozeni prvni instrukce funkce do polozky GTS
         listInsertLast(list, I_START, NULL, NULL, NULL);
         tmp->data.fInstr = list->last;

         // nacteni tokenu a volani fce pro <DC-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = dc_list();
         if(result != SUCCESS) return result;
         
         // token nacteny z fce dc_list(), volani fce pro <ST-LIST>
         result = st_list();
         if(result != SUCCESS) return result;
        
         // token nacteny z fce st_list() - musi byt T_END
         if(token != T_END) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {end}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_FUNCTION     
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_FUNCTION) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {function}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {declare|function}.\n", line);
   return SYN_ERR; 
}

// funkce pro nonterminal <ID-LIST> - SYN_OK, INST_OK
int id_list(void) {
   tLNode *ad1;
   switch(token) {
      // simulace pravidla: <ID-LIST> -> , id <ID-LIST>
      case T_COMMA:
         // nacteni tokenu - musi byt T_ID
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_ID) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator}.\n", line);
            return SYN_ERR;
         }
         
         // semanticka akce pro ID
         ad1 = lts_get_varnode(tmp, &attr);
         if(ad1 == NULL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Promenna '%s' neni deklarovana.\n", line, str_get_str(&attr));
            return SEM_ERR;
         }

         switch(ad1->data.varType) {
            case T_INTEGER:
               listInsertLast(list, I_READ_I, NULL, NULL, ad1);
               break;
            case T_DOUBLE:
               listInsertLast(list, I_READ_D, NULL, NULL, ad1);
               break;
            case T_STRING:
               listInsertLast(list, I_READ_S, NULL, NULL, ad1);
               break;
         }

         // nacteni tokenu a rekurzivni volani fce pro <ID-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return id_list();
      break;
      // simulace pravidla: <ID-LIST> -> eps
      case T_EOL:
         // konec rekurze
         return SUCCESS;    
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {,|eol}.\n", line);
   return SYN_ERR;
}

// funkce pro nonterminal <IT-LIST> - SYN_OK, SEM_OK, NON_INSTR
int it_list(void) {
   int result;
   int tmp_token;
   string tmp_attr;
   
   if(str_init(&tmp_attr) != SUCCESS)
      return INT_ERR;
      
   switch(token) {
      // simulace pravidla: <IT-LIST> -> id as <TYPE> <IT-LIST2>
      case T_ID:
         // ulozeni atributu s jmenem identifikatoru parametru
         if(str_copy_string(&tmp_attr, &attr) != SUCCESS)
            return INT_ERR;
            
         // nacteni tokenu - musi byt T_AS     
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR; 
         }   
            
         if(token != T_AS) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {as}.\n", line);
            str_free(&tmp_attr);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <TYPE>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR;
         }
         
         // ulozeni tokenu s typem parametru
         tmp_token = token;
         
         result = type();
         if(result != SUCCESS) {
            str_free(&tmp_attr);
            return result;
         }
        
         if(isDecl) {
            // funkce jiz byla deklarovana, pouze kontrola parametru
            if(tmp->data.argCount > paramCount) {
               result = gts_check_arg(tmp, tmp_token, &tmp_attr, paramCount);
            }
            else {
               fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru deklarace (%d) a definice (>%d) funkce '%s'.\n", line, tmp->data.argCount, paramCount, str_get_str(&(tmp->key)));
               str_free(&tmp_attr);
               return SEM_ERR;
            }
            if(result != SUCCESS) {
               if(result == SEM_ERR)
                  fprintf(stderr, "radek %d: Semanticka chyba. %d. parametr funkce '%s' se lisi od deklarace.\n", line, paramCount+1, str_get_str(&(tmp->key)));   
               str_free(&tmp_attr);
               return result;               
            }
            paramCount++;   
         }
         else {
            // pridani parametru do tabulky symbolu, kontrola jmena (nesmi byt stejne jako nazev funkce ani jako nazev jine funkce)
            result = gts_set_arg(globalTS, tmp, tmp_token, &tmp_attr);
            if(result != SUCCESS) {
               if(result == SEM_ERR)
                  fprintf(stderr, "radek %d: Semanticka chyba. Parametr '%s' ve funkci '%s' byl jiz deklarovan nebo je deklarovana funkce se stejnym jmenem.\n", line, str_get_str(&tmp_attr), str_get_str(&(tmp->key)));
               str_free(&tmp_attr);
               return result;
            }
         }
         
         str_free(&tmp_attr);
         
         // token nacteny z fce type(), volani fce pro <IT-LIST2>
         return it_list2();
      break;
      // simulace pravidla: <IT-LIST> -> eps
      case T_RIGHT:
         // konec rekurze
         // kontrola, zda souhlasi pocty parametru pri deklaraci/definici
         if(isDecl && (tmp->data.argCount > paramCount)) {
            fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru deklarace (%d) a definice (%d) funkce '%s'.\n", line, tmp->data.argCount, paramCount, str_get_str(&(tmp->key)));
            result = SEM_ERR;
         }
         else {
            result = SUCCESS;
         }
         paramCount = 0;
         str_free(&tmp_attr);
         return result;
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {id|)}.\n", line);
   str_free(&tmp_attr);
   return SYN_ERR;
}

// funce pro nonterminal <IT-LIST2> - SYN_OK, SEM_OK, NON_INSTR
int it_list2(void) {
   int result;
   int tmp_token;
   string tmp_attr;
   
   if(str_init(&tmp_attr) != SUCCESS)
      return INT_ERR;
      
   switch(token) {
      // simulace pravidla: <IT-LIST2> -> , id as <TYPE> <IT-LIST2>
      case T_COMMA:
         // nacteni tokenu - musi byt T_ID
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR; 
         }
            
         if(token != T_ID) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator}.\n", line);
            str_free(&tmp_attr);
            return SYN_ERR;
         }
         
         // ulozeni atributu s jmenem identifikatoru parametru
         if(str_copy_string(&tmp_attr, &attr) != SUCCESS)
            return INT_ERR;
         
         // nacteni tokenu - musi byt T_AS      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR; 
         }
            
         if(token != T_AS) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {as}.\n", line);
            str_free(&tmp_attr);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <TYPE>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            str_free(&tmp_attr);
            return LEX_ERR; 
         }
         else if(token == INT_ERR) {
            str_free(&tmp_attr);
            return INT_ERR;
         }
            
         // ulozeni tokenu s typem parametru
         tmp_token = token;
         
         result = type();
         if(result != SUCCESS) {
            str_free(&tmp_attr);
            return result;
         }

         if(isDecl) {
            // funkce jiz byla deklarovana, pouze kontrola parametru
            if(tmp->data.argCount > paramCount) {
               result = gts_check_arg(tmp, tmp_token, &tmp_attr, paramCount);
            }
            else {
               fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru deklarace (%d) a definice (>%d) funkce '%s'.\n", line, tmp->data.argCount, paramCount, str_get_str(&(tmp->key)));
               str_free(&tmp_attr);
               return SEM_ERR;
            }
            if(result != SUCCESS) {
               if(result == SEM_ERR)
                  fprintf(stderr, "radek %d: Semanticka chyba. %d. parametr funkce '%s' se lisi od deklarace.\n", line, paramCount+1, str_get_str(&(tmp->key)));   
               str_free(&tmp_attr);
               return result;
            }
            paramCount++;   
         }
         else {
            // pridani parametru do tabulky symbolu
            result = gts_set_arg(globalTS, tmp, tmp_token, &tmp_attr);
            if(result != SUCCESS) {
               if(result == SEM_ERR)
                  fprintf(stderr, "radek %d: Semanticka chyba. Parametr '%s' ve funkci '%s' byl jiz deklarovan nebo je deklarovana funkce se stejnym jmenem.\n", line, str_get_str(&tmp_attr), str_get_str(&(tmp->key)));
               str_free(&tmp_attr);   
               return result;
            }   
         }
         
         str_free(&tmp_attr);
         
         // token nacten z fce type(), rekurzivni volani fce it_list2()
         return it_list2();
      break;
      // simulace pravidla: <IT-LIST2> -> eps
      case T_RIGHT:
         // konec rekurze
         // kontrola, zda souhlasi pocty parametru pri deklaraci/definici
         if(isDecl && (tmp->data.argCount > paramCount)) {
            fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru deklarace (%d) a definice (%d) funkce '%s'.\n", line, tmp->data.argCount, paramCount, str_get_str(&(tmp->key)));
            result = SEM_ERR;
         }
         else {
            result = SUCCESS;
         }
         paramCount = 0;
         str_free(&tmp_attr);
         return result;
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {,|)}.\n", line);
   str_free(&tmp_attr);
   return SYN_ERR;   
}

// funkce pro nonterminal <MAIN-P> - SYN_OK, INSTR_OK
int main_p(void) {
   int result;
   switch(token) {
      // simulace pravidla: <MAIN-P> -> <DC-LIST> <ST-LIST> end scope eol
      case T_DIM:
      case T_DO:
      case T_END:
      case T_ID:
      case T_IF:
      case T_INPUT:
      case T_PRINT:
      case T_RETURN:
         // semanticka kontrola, jestli jsou vsechny funkce definovany
         if((result = gts_check_alldefs(*globalTS)) != SUCCESS) {
            if(result == SEM_ERR) 
               fprintf(stderr, "Syntakticka chyba. Vsechny deklarovane funkce nebyly definovany.\n");
            
            return result;   
         }           
        
         // vlozeni hlavni scope funkce do globalni TS
         if(str_copy_array(&attr, "$SCOPE") != SUCCESS)
            return INT_ERR;

         result = gts_insert(globalTS, &attr, &tmp);
         if(result == TAB_INSERT_EMEM)
            return INT_ERR;
         
         // nagenerovani prvni instrukce pro danou funkci a ulozeni prvni instrukce funkce do polozky GTS
         listInsertLast(list, I_START, NULL, NULL, NULL);
         tmp->data.fInstr = list->last;
         
         // volani fce pro <DC-LIST>
         result = dc_list();
         if(result != SUCCESS) return result;
         
         // token nacten z fce dc_list(), volani fce pro <ST-LIST>
         result = st_list();
         if(result != SUCCESS) return result;
         
         // token nacten z fce dc_list() - musi byt T_END
         if(token != T_END) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {end}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_SCOPE      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_SCOPE) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {scope}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_EOL      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;
   }

   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {dim|do|end|id|if|input|print|return}.\n", line);
   return SYN_ERR;   
}

// funkce pro nonterminal <PARAM> - SYN_OK, INSTR_OK
int param(tGNode* tmp_node) {
   tLNode* checkParam;
   switch(token) {
      case T_ID:
      case T_INT:
      case T_REAL:
      case T_REAL_EXP:
      case T_STR:
      case T_HEXA:
      case T_OCTAL:
      case T_BIN:
         // semanticka kontrola dalsich parametru 
         if(token == T_ID) {
            // kontrola, zda je dana promenna deklarovana
            checkParam = lts_get_varnode(tmp, &attr);
            if(checkParam == NULL) {
               fprintf(stderr, "radek %d: Semanticka chyba. Promenna '%s' predavana %d. parametru funkce neni deklarovana.\n", line, str_get_str(&attr), paramCount+1);
               return SEM_ERR;
            }
         }
         else {
            // vytvor vnitrni promennou s hodnotou
            checkParam = lts_set_const(globalTS, tmp, token, &attr); 
         }
         
         if(tmp_node->data.argCount > paramCount) {
            // kontrola datoveho typu - pozdeji poresit, ze pro param double muze byt int
            char* t = str_get_str(&(tmp_node->data.types));
            int tmpType;
            switch(t[paramCount]) {
               case 'i':
                  tmpType = T_INTEGER;
                  break;
               case 'd':
                  tmpType = T_DOUBLE;
                  break;
               case 's':
                  tmpType = T_STRING;
                  break;
            }
            if(checkParam->data.varType != tmpType) {
               fprintf(stderr, "radek %d: Semanticka chyba. Pri volani funkce '%s' nesouhlasi %d. parametr s deklaraci.\n", line, str_get_str(&(tmp_node->key)), paramCount+1);
               return SEM_ERR;
            }

            // predani hodnoty do parametru funkce
            listInsertLast(list, I_PAR, checkParam, NULL, NULL);
            
            paramCount++;
         }
         else {
            fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru funkce '%s' (%d) a volani (%d).\n", line, str_get_str(&(tmp_node->key)), tmp_node->data.argCount, paramCount);
            return SEM_ERR;
         }

         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;
   }

   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator|konstanta}.\n", line);
   return SYN_ERR;
}

// funkce pro nonterminal <PM-LIST> - SYN_OK, INSTR_OK
int pm_list(tGNode* tmp_node) {
   tLNode* checkParam; 
   switch(token) {
      // simulace pravidla: <PM-LIST> -> <PARAM> <PM-LIST2>
      case T_ID:
      case T_INT:
      case T_REAL:
      case T_REAL_EXP:
      case T_STR:
      case T_HEXA:
      case T_OCTAL:
      case T_BIN:
         // semanticka kontrola prvniho parametru funkce
         if(token == T_ID) {
            // kontrola, zda je dana promenna deklarovana
            checkParam = lts_get_varnode(tmp, &attr);
            if(checkParam == NULL) {
               fprintf(stderr, "radek %d: Semanticka chyba. Promenna '%s' predavana %d. parametru funkce neni deklarovana.\n", line, str_get_str(&attr), paramCount+1);
               return SEM_ERR;
            }
         }
         else {
            // vytvor vnitrni promennou s hodnotou
            checkParam = lts_set_const(globalTS, tmp, token, &attr); 
         }
         
         if(tmp_node->data.argCount > paramCount) {
            // kontrola datoveho typu - pozdeji poresit, ze pro param double muze byt int
            char* t = str_get_str(&(tmp_node->data.types));
            int tmpType;
            switch(t[paramCount]) {
               case 'i':
                  tmpType = T_INTEGER;
                  break;
               case 'd':
                  tmpType = T_DOUBLE;
                  break;
               case 's':
                  tmpType = T_STRING;
                  break;
            }
            if(checkParam->data.varType != tmpType) {
               fprintf(stderr, "radek %d: Semanticka chyba. Pri volani funkce '%s' nesouhlasi %d. parametr s deklaraci.\n", line, str_get_str(&(tmp_node->key)), paramCount+1);
               return SEM_ERR;
            }
            
            // predani hodnoty do parametru funkce
            listInsertLast(list, I_PAR, checkParam, NULL, NULL);

            paramCount++;
         }
         else {
            fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru funkce '%s' (%d) a volani (%d).\n", line, str_get_str(&(tmp_node->key)), tmp_node->data.argCount, paramCount);
            return SEM_ERR;
         }

         // nacteni tokenu a volani fce pro <PM-LIST2>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return pm_list2(tmp_node);
      break;
      // simulace pravidla: <PM-LIST> -> eps
      case T_RIGHT:
         // konec rekurze
         if(tmp_node->data.argCount > paramCount) {
            fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru funkce '%s' (%d) a volani (%d).\n", line, str_get_str(&(tmp_node->key)), tmp_node->data.argCount, paramCount);
            return SEM_ERR;
         }
         paramCount = 0;
         return SUCCESS;
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator|konstanta|)}.\n", line);
   return SYN_ERR;
}

// funkce pro nonterminal <PM-LIST2> - SYN_OK, INSTR_NONE
int pm_list2(tGNode* tmp_node) {
   int result;
   switch(token) {
      // simulace pravidla: <PM-LIST2> -> , <PARAM> <PM-LIST2>
      case T_COMMA:
         // nacteni tokenu a volani fce pro <PARAM>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = param(tmp_node);
         if(result != SUCCESS) return result;
         
         // token nacten z fce param(), rekurzivni volani fce pm_list2()
         return pm_list2(tmp_node);
      break;
      // simulace pravidla: <PM-LIST2> -> eps
      case T_RIGHT:
         // konec rekurze
         if(tmp_node->data.argCount > paramCount) {
            fprintf(stderr, "radek %d: Semanticka chyba. Nesouhlasi pocet parametru funkce '%s' (%d) a volani (%d).\n", line, str_get_str(&(tmp_node->key)), tmp_node->data.argCount, paramCount);
            return SEM_ERR;
         }
         paramCount = 0;
         return SUCCESS;
      break;   
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {,|identifikator|konstanta|)}.\n", line);
   return SYN_ERR;   
}

// funkce pro nonterminal <PROG> - SYN_OK, INSTR_OK
int prog(void) {
   int result;
   switch(token) {
      // simulace pravidla: <PROG> -> <FN-LIST> scope eol <MAIN-P> eof
      case T_DECLARE:
      case T_FUNCTION:
      case T_SCOPE:
         // volani fce pro <FN-LIST>
         result = fn_list();
         if(result != SUCCESS) return result;
         
         // token nacteny z fce fn_list() - musi byt T_SCOPE
         if(token != T_SCOPE) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {scope}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_EOL      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <MAIN-P>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = main_p();
         if(result != SUCCESS) return result;
         
         // token nacteny z fce main_p() - musi byt T_EOF
         if(token != T_EOF) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eof}.\n", line);
            return SYN_ERR;
         }
         
         // posledni instrukce 
         listInsertLast(list, I_STOP, NULL, NULL, NULL);
         
         // konec zdrojoveho programu 
         return SUCCESS;
      break;
   }

   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {declare|function|scope}.\n", line);
   return SYN_ERR; 
}

// funkce pro nonterminal <REST> - SYN_OK, INSTR_NONE
int rest(tGNode* tmp_node) {
   int result;
   switch(token) {
      // simulace pravidla: <REST> -> ( <PM-LIST> )
      case T_LEFT:
         // nacteni tokenu a volani fce pro <PM-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = pm_list(tmp_node);
         if(result != SUCCESS) return result;
         
         // token nacten z fce pm_list() - musi byt T_RIGHT
         if(token != T_RIGHT) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {)}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;         
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {(}.\n", line);
   return SYN_ERR;   
}

// funkce pro nonterminal <RHS>, INSTR_OK
int rhs(int* rv) {
   tGNode* tmp_node;
   switch(token) {
      // simulace pravidla: <RHS> -> id <REST>
      case T_ID:
         // token je T_ID, muze byt bud uziv. promenna nebo fce
         // vyhledani klice v globalni tabulce symbolu
         tmp_node = gts_search(*globalTS, &attr);
         if(tmp_node != NULL && ((tmp_node->data.varReserved) == 0)) {
            // identifikator odpovida uzivatelske funkci
            if((token = get_next_token(&attr)) == LEX_ERR) {
               fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
               return LEX_ERR;
            }
            else if(token == INT_ERR)
               return INT_ERR;
           
            // navratova hodnota fce
            *rv = tmp_node->data.retType;
            
            // vytvoreni intrukce pro call
            listInsertLast(list, I_FCALL, tmp_node, NULL, NULL);

            return rest(tmp_node);
         }
         else {
            // identifikator odpovida uzivatelske promenne
            *rv = 0;
            return  parse_expr();
         }
      break;
      case T_EOL:
         fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavan vyraz v prirazeni.\n", line);
         return SEM_ERR;
      break;
      // pokud neni token T_ID, preda se rizeni precedencni SA
      default:
         *rv = 0;
         return parse_expr();
      break;         
   } 
}

// funkce pro nonterminal <STAT> - SYN_OK
int stat(void) {
   int result;
   int rv = 0;
   tLNode* tmp_node;
   tLNode *ad1;
   tListItem *tmp_item, *tmp_item2;
   switch(token) {
      // simulace pravidla: <STAT> -> do while <EXPR> eol <ST-LIST> loop - INSTR_OK
      case T_DO:
         // nacteni tokenu - musi byt T_WHILE
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_WHILE) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {while}.\n", line);
            return SYN_ERR;
         }
        
         // vytvoreni navesti pro while
         listInsertLast(list, NAV_WHILE, NULL, NULL, NULL);
         tmp_item = list->last;

        // nacteni tokenu a predani rizeni precedencni SA pro vyrazy
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = parse_expr();
         if(result != SUCCESS) return result;
         
         // vytvoreni instrukce podmineneho skoku a ulozeni adresy
         listInsertLast(list, I_NOT, lNode, NULL, lNode);
         listInsertLast(list, I_JMP, lNode, NULL, NULL);

         // semanticka kontrola lNode
         if(lNode->data.varType != T_INTEGER) {
            fprintf(stderr, "radek %d: Semanticka chyba. Vyraz v cyklu neni typu integer.\n", line);
            return SEM_ERR;
         }
   
         // token nacteny z fce parse_expr() - musi byt T_EOL
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <ST-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = st_list();
         if(result != SUCCESS) return result;
         
         // token nacteny z fce st_list() - musi byt T_LOOP
         if(token != T_LOOP) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {loop}.\n", line);
            return SYN_ERR;
         }
         
         // ulozeni instrukce na nepodmineny skok
         listInsertLast(list, I_JMP, tmp_item, NULL, NULL);
         listInsertLast(list, NAV_WHILE, NULL, NULL, NULL);
         tmp_item->Instruction.addr2 = list->last; 

         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;
      // simulace pravidla: <STAT> -> id = <RHS> - INST_OK
      case T_ID:
         // semanticka kontrola zda je id deklarovany
         tmp_node = lts_get_varnode(tmp, &attr);
         if(tmp_node == NULL) {
            fprintf(stderr, "radek %d: Semanticka chyba. Promenna '%s' neni deklarovana.\n", line, str_get_str(&attr));
            return SEM_ERR;
         }             

         // nacteni tokenu - musi byt T_E
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_E) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {=}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <RHS> 
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
        
         result = rhs(&rv);
         if(result != SUCCESS) return result;
         
         // prirazeni vyrazu z parse_expr()
         if(rv == 0) {
            // semanticka kontrola prirazeni - porovnani typu
            if((tmp_node->data.varType == T_INTEGER) && (lNode->data.varType == T_INTEGER)) {
               // instrukce pro INTEGER
               listInsertLast(list, I_MOV, lNode, NULL, tmp_node);
            }
            else if((tmp_node->data.varType == T_DOUBLE) && (lNode->data.varType == T_DOUBLE)) {
               // instrukce pro DOUBLE
               listInsertLast(list, I_MOV, lNode, NULL, tmp_node);
            }
            else if((tmp_node->data.varType == T_STRING) && (lNode->data.varType == T_STRING)) {
               // instrukce pro STRING
               listInsertLast(list, I_MOV, lNode, NULL, tmp_node);
            }
            else if((tmp_node->data.varType == T_DOUBLE) && (lNode->data.varType == T_INTEGER)) {
               // prevod INT na DOUBLE a instrukce prirazeni pro DOUBLE
               ad1 = lts_set_varnode(globalTS, tmp, T_DOUBLE);
               listInsertLast(list, I_INT2DBL, lNode, NULL, ad1); 
               listInsertLast(list, I_MOV, ad1, NULL, tmp_node);
            }
            else {
               // semanticka chyba
               fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest prirazeni nad temito typy.\n", line);
               return SEM_ERR;
            }
         }
         else {   // prirazeni z funkce TODO: DODELAT RETURN
            // semanticka kontrola prirazeni - porovnani typu
            if((tmp_node->data.varType == T_INTEGER) && (rv == T_INTEGER)) {
               // instrukce pro INTEGER
               printf("radek %d: DEBUG => prirazeni INT2INT\n", line);
            }
            else if((tmp_node->data.varType == T_DOUBLE) && (rv == T_DOUBLE)) {
               // instrukce pro DOUBLE
               printf("radek %d: DEBUG => prirazeni DBL2DBL\n", line);
            }
            else if((tmp_node->data.varType == T_STRING) && (rv == T_STRING)) {
               // instrukce pro STRING
               printf("radek %d: DEBUG => prirazeni STR2STR\n", line);
            }
            else if((tmp_node->data.varType == T_DOUBLE) && (rv == T_INTEGER)) {
               // prevod INT na DOUBLE a instrukce prirazeni pro DOUBLE
               printf("radek %d: DEBUG => prirazeni INT2DBL\n", line);

            }
            else {
               // semanticka chyba
               fprintf(stderr, "radek %d: Semanticka chyba. Nelze provest prirazeni nad temito typy.\n", line);
               return SEM_ERR;
            }
         }
         // zpetna inicializace na 0
         rv = 0;
         return result;         
      break;
      // simulace pravidla: <STAT> -> if <EXPR> then eol <ST-LIST> else eol <ST-LIST> end if - INST_OK
      case T_IF:
         // nacteni tokenu a predani rizeni precedencni SA pro vyrazy
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = parse_expr();
         if(result != SUCCESS) return result;
         
         // semanticka kontrola, ze vyraz je integer
         if(lNode->data.varType != T_INTEGER) {
            fprintf(stderr, "radek %d: Semanticka chyba. Vyraz v ifu neni typu integer.\n", line);
            return SEM_ERR;
         }

         // instrukce pro negovani
         listInsertLast(list, I_NOT, lNode, NULL, lNode);
         // instrukce pro podmineny skok na else
         listInsertLast(list, I_JMP, NULL, NULL, NULL);
         tmp_item = list->last;
         
         // token nacteny z fce parse_expr() - musi byt T_THEN
         if(token != T_THEN) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {then}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_EOL      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <ST-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = st_list();
         if(result != SUCCESS) return result;
         
         // instrukce pro nepodmineny skok za else
         listInsertLast(list, I_UNJMP, NULL, NULL, NULL);
         tmp_item2 = list->last;
         // instrukce navesti pro skok na else
         listInsertLast(list, NAV_IF, NULL, NULL, NULL);
         tmp_item->Instruction.addr1 = list->last;


         // token nacteny z fce st_list() - musi byt T_ELSE
         if(token != T_ELSE) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {else}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_EOL      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <ST-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = st_list();
         if(result != SUCCESS) return result;
         
         // token nacteny z fce st_list() - musi byt T_END
         if(token != T_END) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {end}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu - musi byt T_IF
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_IF) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {if}.\n", line);
            return SYN_ERR;
         }
         
         // instrukce navesti pro skok za else
         listInsertLast(list, NAV_IF, NULL, NULL, NULL);
         tmp_item2->Instruction.addr1 = list->last;

         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;
      // simulace pravidla: <STAT> -> input ; id <ID-LIST> - INST_OK
      case T_INPUT:
         // nacteni tokenu - musi byt T_SEMI      
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_SEMI) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {;}.\n", line);
            return SYN_ERR;
         }
         
         // instrukce pro vytisknuti zacatku inputu
         listInsertLast(list, I_INPUT, NULL, NULL, NULL);

         // nacteni tokenu - musi byt T_ID
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR; 
            
         if(token != T_ID) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {identifikator}.\n", line);
            return SYN_ERR;
         }

         // semanticka akce pro ID
         ad1 = lts_get_varnode(tmp, &attr);
         if(ad1 == NULL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Promenna '%s' neni deklarovana.\n", line, str_get_str(&attr));
            return SEM_ERR;
         }

         switch(ad1->data.varType) {
            case T_INTEGER:
               listInsertLast(list, I_READ_I, NULL, NULL, ad1);
               break;
            case T_DOUBLE:
               listInsertLast(list, I_READ_D, NULL, NULL, ad1);
               break;
            case T_STRING:
               listInsertLast(list, I_READ_S, NULL, NULL, ad1);
               break;
         }

         // nacteni tokenu a volani fce pro <ID-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return id_list();
      break;
      // simulace pravidla: <STAT> -> print <EXPR> ; <EX-LIST> - INST_OK
      case T_PRINT:
         // nacteni tokenu a predani rizeni precedencni SA pro vyrazy
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = parse_expr();
         if(result != SUCCESS) return result;
         
         switch(lNode->data.varType) {
            case T_INTEGER:
               listInsertLast(list, I_WRITE_I, lNode, NULL, NULL); 
               break;
            case T_DOUBLE:
               listInsertLast(list, I_WRITE_D, lNode, NULL, NULL);
               break;
            case T_STRING:
               listInsertLast(list, I_WRITE_S, lNode, NULL, NULL);
               break;
         }

        // token nacteny z fce parse_expr() - musi byt T_SEMI
         if(token != T_SEMI) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {;}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a volani fce pro <EX-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return ex_list();
      break;
      //simulace pravidla: <STAT> -> return <EXPR>
      case T_RETURN:
         // semanticka kontrola - return nesmi byt ve fci main scope
         if(strcmp(str_get_str(&(tmp->key)), "$SCOPE") == 0) {
            fprintf(stderr, "radek %d: Semanticka chyba. Prikaz return ve funkci '%s'.\n", line, str_get_str(&(tmp->key)));
            return SEM_ERR;
         }
            
          
         // nacteni tokenu a predani rizeni precedencni SA pro vyrazy
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         result = parse_expr();
         if(result != SUCCESS) return result;

         // semanticka kontrola, zdali se return rovna navratovemu typu fce
         if((tmp->data.retType == T_INTEGER) && (lNode->data.varType == T_INTEGER)) {
            // instrukce pro INTEGER
            printf("radek %d: DEBUG => prirazeni INT2INT\n", line);
         }
         else if((tmp->data.retType == T_DOUBLE) && (lNode->data.varType == T_DOUBLE)) {
            // instrukce pro DOUBLE
            printf("radek %d: DEBUG => prirazeni DBL2DBL\n", line);
         }
         else if((tmp->data.retType == T_STRING) && (lNode->data.varType == T_STRING)) {
            // instrukce pro STRING
            printf("radek %d: DEBUG => prirazeni STR2STR\n", line);
         }
         else if((tmp->data.retType == T_DOUBLE) && (lNode->data.varType == T_INTEGER)) {
            // prevod INT na DOUBLE a instrukce prirazeni pro DOUBLE
            printf("radek %d: DEBUG => prirazeni INT2DBL\n", line);
         }
         else {
            // semanticka chyba
            fprintf(stderr, "radek %d: Semanticka chyba. Vyraz v return prikazu neodpovida navratove hodnote funkce.\n", line);
            return SEM_ERR;
         }
         return result;
      break;
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {do|id|if|input|print|return}.\n", line);
   return SYN_ERR;
}

// funkce pro nonterminal <ST-LIST> - SYN_OK, INST_NONE
int st_list(void) {
   int result;
   switch(token) {
      // simulace pravidla: <ST-LIST> -> <STAT> eol <ST-LIST>
      case T_DO:
      case T_ID:
      case T_IF:
      case T_INPUT:
      case T_PRINT:
      case T_RETURN:
         // volani fce pro <STAT>
         result = stat();
         if(result != SUCCESS) return result;
         
         // token nacten z fce stat() - musi byt T_EOL
         if(token != T_EOL) {
            fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {eol}.\n", line);
            return SYN_ERR;
         }
         
         // nacteni tokenu a rekurzivni volani fce pro <ST-LIST>
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;
         
         return st_list();
      break;
      // simulace pravidla: <ST-LIST> -> eps
      case T_ELSE:
      case T_END:
      case T_LOOP:
         // konec rekurze
         return SUCCESS;    
      break;   
   }
   
   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {do|else|end|id|if|input|loop|print|return}.\n", line);
   return SYN_ERR;   
}

// funkce pro nonterminal <TYPE> - SYN_OK, INST_NONE
int type(void) {
   //int result;
   switch(token) {
      case T_INTEGER:
      case T_DOUBLE:
      case T_STRING:
         // nacteni tokenu
         if((token = get_next_token(&attr)) == LEX_ERR) {
            fprintf(stderr, "radek %d: Lexikalni chyba.\n", line);
            return LEX_ERR; 
         }
         else if(token == INT_ERR)
            return INT_ERR;

         return SUCCESS;
      break;
   }

   // jiny token - syntakticka chyba
   fprintf(stderr, "radek %d: Syntakticka chyba. Ocekavano {integer|double|string}.\n", line);
   return SYN_ERR;
}
