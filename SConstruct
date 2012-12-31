#!/bin/python

env = Environment()
env.Append(CCFLAGS="-I. -Wall -Wpointer-arith -O3")
env.Append(LINKFLAGS = ['-Wl','-g', '-O3'])
#env.Program(target="calc_main",source=["pbots_calc.c","util.c"], LIBS=['poker-eval'], LIBPATH='../PEC/poker-eval/lib/.libs')
pbots_calc = env.SharedLibrary(target="pbots_calc",source=["pbots_calc.c","util.c"],
                  LIBS=['poker-eval'])
env.Install("/usr/local/lib", pbots_calc)
env.Install("/usr/local/include/pbots_calc", "pbots_calc.h")
#env.Append(LINKFLAGS = ['-rpath=../PEC/poker-eval/lib/.libs'])
env.Program(target="calculator",source=["calculator.c"], LIBS=['pbots_calc','poker-eval'])
