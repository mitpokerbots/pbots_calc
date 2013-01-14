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

"""
Python wrapper around the pbots_calc library. Requires the poker-eval library to
be installed. Provides a single function, calc, which corresponds to the calc
function provided by pbots_calc. Returns a Results object.
"""

import ctypes
import ctypes.util
import sys

if sys.platform.startswith('win'):
    pbots_calc = "pbots_calc"
elif sys.platform.startswith('darwin'):
    pbots_calc = "libpbots_calc.dylib"
else:
    pbots_calc = "libpbots_calc.so"

class _Results(ctypes.Structure):
    _fields_ = [("ev", ctypes.POINTER(ctypes.c_double)),
                ("hands", ctypes.POINTER(ctypes.c_char_p)),
                ("iters", ctypes.c_int),
                ("size", ctypes.c_int),
                ("MC", ctypes.c_int)]

try:
    pcalc = ctypes.CDLL(pbots_calc)
except OSError:
    print "ERROR: Could not locate %s. Please ensure your enviroment library load path is set properly." % pbots_calc
    sys.exit(1)

# Set the argtype and return types from the library.
pcalc.calc.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(_Results)]
pcalc.calc.restype = ctypes.c_int
pcalc.alloc_results.argtypes = []
pcalc.alloc_results.restype = ctypes.POINTER(_Results)
pcalc.free_results.argtypes = [ctypes.POINTER(_Results)]
pcalc.free_results.restype = None

class Results:
    def __init__(self, res):
        self.size = res.size
        self.MC_used = res.MC
        self.iters = res.iters
        self.ev = []
        self.hands = []
        for i in range(self.size):
            self.ev.append(res.ev[i])
            self.hands.append(res.hands[i])

    def __str__(self):
        return str(zip(self.hands, self.ev))

def calc(hands, board, dead, iters):
    res = pcalc.alloc_results()
    err = pcalc.calc(hands, board, dead, iters, res)
    if err > 0:
        results = Results(res[0])
    else:
        print "error: could not parse input or something..."
        results = None
    pcalc.free_results(res)
    return results
