#!/bin/python

env = Environment()
env.Append(CCFLAGS="-I. -Wall -Wpointer-arith -O3")
env.Append(LINKFLAGS = ['-Wl','-g', '-O3'])

Export("env")

env.Alias('install', ['/usr/local'])
env.Alias('build', ['.'])

SConscript(["src/SConscript","example/SConscript","java/SConscript"],export="env")
