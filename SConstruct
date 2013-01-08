#!/bin/python
import os
import sys

env = Environment(TARGET_ARCH="x86")
newpath=os.environ.get('PATH')
env.Append(ENV = { 'PATH' : newpath })

ccflags = ['-I.', '-MDd']
if not sys.platform.startswith('win'):
    ccflags.extend(['-Wall', '-O3', '-Wpointer-arith'])
env.Append(CCFLAGS=ccflags)
env.Append(LINKFLAGS = ['-Wl','-g', '-O3'])

Export("env")

env.Alias('install', ['/usr/local'])
env.Alias('build', ['.'])

SConscript(["src/SConscript","example/SConscript","java/SConscript"],export="env")
