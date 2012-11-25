gcc -std=gnu99 -I../PEC/poker-eval/include -Wall -Wpointer-arith calc_main.c -c -o calc_main.o
/bin/bash ../PEC/poker-eval/libtool --tag=CC --mode=link gcc -g -O2 -o calc_main calc_main.o ../PEC/poker-eval/lib/libpoker-eval.la
