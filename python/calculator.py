#!/usr/bin/python
import pbots_calc
import sys

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
