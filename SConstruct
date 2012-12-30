#!/bin/python

env = Environment()
env.Append(CCFLAGS="-I. -I../PEC/poker-eval/include -Wall -Wpointer-arith -O3 -fPIC")
env.Append(LINKFLAGS = ['-Wl','-g', '-O3'])
#env.Program(target="calc_main",source=["pbots_calc.c","util.c"], LIBS=['poker-eval'], LIBPATH='../PEC/poker-eval/lib/.libs')
env.SharedLibrary(target="pbots_calc",source=["pbots_calc.c","util.c"], LIBS=['poker-eval'], LIBPATH='../PEC/poker-eval/lib/.libs')
#env.Append(LINKFLAGS = ['-rpath=../PEC/poker-eval/lib/.libs'])
env.Program(target="calculator",source=["calculator.c"], LIBS=['pbots_calc','poker-eval'], LIBPATH='.:../PEC/poker-eval/lib/.libs')
