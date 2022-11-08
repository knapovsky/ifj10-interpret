CFLAGS=-std=c99 -Wall -pedantic -g
CFLAGS2=-std=c99 -Wall -pedantic -O2
DBG=-DDEBUG
BIN=ifj10
BINOPT=ifj10_optimalized
CC=gcc

ALL: str.o ial.o scanner.o const.h parser.o interpret.o prec_analyse.o prec_stack.o ilist.o dllist.o main.o 
	$(CC) $(CFLAGS) -o $(BIN) str.o ial.o scanner.o parser.o interpret.o prec_analyse.o prec_stack.o ilist.o dllist.o main.o

optimalize:	str.o ial.o const.o scanner.o parser.o interpret.o main.o
	$(CC) $(CFLAGS2) -o $(BINOPT) str.o ial.o const.o scanner.o parser.o interpret.o main.o
   
clean:
	rm -f ifj10 *.o *~ tt_*

pack:
	zip xknapo02.zip *.c *.h rozdeleni Makefile
