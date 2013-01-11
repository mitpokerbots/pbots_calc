#!/usr/bin/python
#
# Copyright (C) 2012-2013 Owen Derby (ocderby@gmail.com)
#
# This file is part of pbots_calc.
#
# pbots_calc is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# pbots_calc is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# pbots_calc in a file in teh toplevel directory called "GPLv3".  If not, see
# <http://www.gnu.org/licenses/>.
#

import os
import sys
import subprocess

linkflags = []
ccflags = ['-I.', '-MD']
platform = sys.platform
# need to do something like  on windows?
if sys.platform.startswith('win'):
    subprocess.call("vcvars32.bat")
    linkflags.extend(['-OPT:REF', '-OPT:ICF', '-NOLOGO'])
    ccflags.extend(['-W3', '-Ox', '-nologo'])
else:
    linkflags.extend(['-O3'])
    ccflags.extend(['-Wall', '-O3', '-Wpointer-arith'])

include = "#export/%s/include" % platform
lib = "#export/%s/lib" % platform
bin = "#export/%s/bin" % platform

ld_library_path = os.path.abspath(lib[1:])

env = Environment(ENV = os.environ,
                  TARGET_ARCH = "x86",
                  PLATFORM = platform,
                  CCFLAGS = ccflags,
                  LINKFLAGS = linkflags,
                  BINDIR = bin,
                  INCDIR = include,
                  LIBDIR = lib,
                  CPPPATH = [include],
                  LIBPATH = [lib],
                  LD_LIBRARY_PATH = ld_library_path)

Export("env")

#env.Alias('install', ['/usr/local'])
env.Alias('build', ['.'])

env.SConscript(["src/SConscript", "example/SConscript","java/SConscript", "python/SConscript"], exports='env')
