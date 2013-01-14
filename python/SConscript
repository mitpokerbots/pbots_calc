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

Import('env')

if env['PLATFORM'].startswith('win'):
    fname = "calculator.bat"
    ftext = "@set PATH=%%PATH%%;%s\r\n" % env['LD_LIBRARY_PATH']
    args = "%1"
elif env['PLATFORM'].startswith('darwin'):
    fname = "calculator.sh"
    ftext = "#!/bin/sh\nexport DYLD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n" % env['LD_LIBRARY_PATH']
    args = "$@"
else:
    fname = "calculator.sh"
    ftext = "#!/bin/sh\nexport LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n" % env['LD_LIBRARY_PATH']
    args = "$@"

with open(os.path.abspath(fname),'w') as cmd:
    cmd.write(ftext)
    cmd.write("python %s %s" % (os.path.abspath("calculator.py"), args))

Execute(Chmod(os.path.abspath(fname), 0755))
