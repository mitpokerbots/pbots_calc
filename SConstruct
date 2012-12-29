#!/bin/python

env = Environment()
env.Append(CCFLAGS="-I. -I../PEC/poker-eval/include -Wall -Wpointer-arith -O3")
env.Append(LINKFLAGS = ['-Wl','-g', '-O3'])
env.Program(target="calc_main",source=["calc_main.c","util.c"], LIBS=['poker-eval'], LIBPATH='../PEC/poker-eval/lib/.libs')
