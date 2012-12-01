ARGS="-I. -I../PEC/poker-eval/include -Wall -Wpointer-arith"
gcc $ARGS util.c -c -o util.o
gcc $ARGS calc_main.c -c -o calc_main.o
/bin/bash ../PEC/poker-eval/libtool --tag=CC --mode=link gcc -g -O2 -o calc_main calc_main.o util.o ../PEC/poker-eval/lib/libpoker-eval.la
