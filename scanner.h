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
  * Soubor scanner.h
  *
  * hlavicka lexikalniho analyzatoru
  */

#ifndef _SCANNER_H
#define _SCANNER_H

#include "str.h"

//nekoncove stavy automatu
#define S_START               10  //pocatecni stav
#define S_STR                 11  //string
#define S_BL_CM               12  //block comment
#define S_BL_CM_EC            13  //block comment end-check
#define S_LN_CM               14  //line commnent
#define S_EXP                 15  //exponent
#define S_EXP_SIGN            16  //exponentove znamenko
#define S_STR_ESC             17  //escape sekvence
#define S_NUM_DOT             18  //cekani na cislo po tecce
#define S_NUM_EXP             19  //cekani na cislo po exponentu

//operatory
#define T_E                   20  //= (equals)
#define T_ADD                 21  //+
#define T_SUB                 22  //-
#define T_MUL                 23  //*
#define T_IDIV                24  //celociselne deleni
#define T_DIV                 25  //realne deleni
#define T_G                   26  //is greater > 
#define T_GE                  27  //is greater or equal >=
#define T_L                   28  //is lower <
#define T_LE                  29  //is lower or equal <=
#define T_NE                  30  //not-equal <>

#define S_EOL                 31  //carriage return
#define T_LEFT                32  //leva zavorka
#define T_RIGHT               33  //prava zavorka
#define LF                    34  //line feed
#define T_EOL                 35  //end-of-line
#define T_EOF                 36  //end-of-file
#define T_SEMI                37  //;
#define T_COMMA               38  //,

//cisla
#define T_INT                 40  //cele cislo
#define T_REAL                41  //realne cislo
#define T_REAL_EXP            42  //realne cislo s exponentem

//string
#define T_STR                 50  //retezec
#define T_ID                  51  //identifikator

//klicova slova
#define T_AS                  61
#define T_DECLARE             62
#define T_DIM                 63
#define T_DO                  64
#define T_DOUBLE              65
#define T_ELSE                66
#define T_END                 67
#define T_FIND                68
#define T_FUNCTION            69
#define T_IF                  70
#define T_INPUT               71
#define T_INTEGER             72
#define T_LOOP                73
#define T_PRINT               74
#define T_RETURN              75
#define T_SCOPE               76
#define T_SORT                77
#define T_STRING              78
#define T_THEN                79
#define T_WHILE               80

//extra sekce
#define S_NUM                 90  //cisla o zakladu 2, 8, 16
#define S_HEXA                91  //cisla o zakladu 16
#define S_OCTAL               92  //cisla o zakladu 8
#define S_BIN                 93  //cisla o zakladu 2
#define T_HEXA                94
#define T_OCTAL               95
#define T_BIN                 96

//hlavicky funkci
int is_keyword(string *attr);
void set_source_file(FILE* f);
int get_next_token(string *attr);

#endif
