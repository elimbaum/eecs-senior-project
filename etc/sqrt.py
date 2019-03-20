#!/usr/bin/env python3

import math
import sys
import itertools

# for proper mac semantics, return should be first arg
# x = mac(x, ...)

def mac(a, b, c):
    return a + b * c

def b_sqrt(S):
    x = 0
    x = mac(x, S, 0.5)

    _old_x = 0 # not actually part of the circuit

    # i is approximately the ceiling of the natural log of S
    for i in itertools.count():
        _old_x = x
        x = mac(x, S, 1 / x)
        x = mac(x, x, -0.5)
        # print(i, x)

        # either can break when below some threshold, or set # of iterations
        # based on exponent of argument (can do this from floating point repr)
        if (abs(_old_x - x)) < 1e-8: break

    print(S, "\t", i)
    # print("correct:", math.sqrt(S), "(est {:.3}% off)".format(100 * (x / math.sqrt(S) - 1)))

for i in range(1, 1000000):
    b_sqrt(i)
