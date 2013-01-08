#!/bin/python
import os

env = Environment(TARGET_ARCH="x86")
newpath=os.environ.get('PATH')
env.Append(ENV = { 'PATH' : newpath })
env.Append(CCFLAGS="-I. -Wall -O3 -MDd")
# Windows doesn't like -Wpointer-arith
#env.Append(CCFLAGS="-I. -Wall -O3 -Wpointer-arith -MDd")
env.Append(LINKFLAGS = ['-Wl','-g', '-O3'])

Export("env")

env.Alias('install', ['/usr/local'])
env.Alias('build', ['.'])

SConscript(["src/SConscript","example/SConscript","java/SConscript"],export="env")
