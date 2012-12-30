#!/bin/python

import ctypes

peval = ctypes.CDLL('../PEC/poker-eval/lib/.libs/libpoker-eval.so')
pcalc = ctypes.CDLL('./libpbots_calc.so')

class _Results(ctypes.Structure):
    _fields_ = [("ev", ctypes.POINTER(ctypes.c_double)),
                ("hands", ctypes.POINTER(ctypes.c_char_p)),
                ("iters", ctypes.c_int),
                ("size", ctypes.c_int),
                ("MC", ctypes.c_int)]

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

def calc(hands, board, dead, iters):
    res = pcalc.alloc_results()
    err = pcalc.calc(hands, board, dead, iters, res)
    if err > 0:
        #pcalc.print_results(res)
        results = Results(_Results.from_address(res))
    else:
        print "error: could not parse input or something..."
        results = None
    pcalc.free_results(res)
    return results
