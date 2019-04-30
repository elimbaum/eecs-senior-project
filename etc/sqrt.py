#!/usr/bin/env python3

from math import sqrt, frexp, exp
import sys
import itertools

perfect = 0

ITER = 5

# for proper mac semantics, return should be first arg
# x = mac(x, ...)

def mac(a, b, c):
    return a + b * c

def b_sqrt(S):
    global perfect

    m, e = frexp(S)
    x = m * (2 ** (e / 2))
    # print(S, "est", x, "actual", sqrt(S))

    _old_x = 0 # not actually part of the circuit

    percent_off = 0

    # i is approximately the ceiling of the natural log of S
    for i in range(ITER):
        _old_x = x
        x = mac(x, S, 1 / x)
        x = mac(x, x, -0.5)
        # print(i, x)

        # either can break when below some threshold, or set # of iterations
        # based on exponent of argument (can do this from floating point repr)
        percent_off = x / sqrt(S) - 1
        # if (abs(_old_x - x)) < 1e-8: break
        # if percent_off < 1e-20: break

    # print(S, i, percent_off, sep="\t")
    if percent_off == 0:
        perfect += 1
    # print("correct:", sqrt(S), "(est {:.3}% off)".format(100 * (x / sqrt(S) - 1)))

for n in range(1, 15):
    ITER = n
    for i in range(-200, 200):
        b_sqrt(exp(i))

    print(n, perfect)
    perfect = 0
