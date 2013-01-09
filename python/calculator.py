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

import pbots_calc
import sys

"""
Simple command line tool for equity calculations. Demonstrates how to use the
pbots_calc library python wrapper.
"""

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "usage: %s hand1:hand2:hand... [board [dead]]" % sys.argv[0]
        sys.exit(0)
    board = ""
    dead = ""
    if len(sys.argv) >= 3:
        board = sys.argv[2]
        if len(sys.argv) >=4:
            dead = sys.argv[3]

    r = pbots_calc.calc(sys.argv[1], board, dead, 1000000)
    if r:
        print zip(r.hands, r.ev)
