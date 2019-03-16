#!/usr/bin/env python3
from math import acos, sqrt

with open('angle.in', 'r') as inf:
    while True:
        a = [float(x) for x in inf.readline().strip().split()]
        b = [float(x) for x in inf.readline().strip().split()]
        semi = inf.readline().strip()

        if not semi or semi != ';':
            exit()

        print("{:.6f}".format(
            acos(
                sum(ai * bi for (ai, bi) in zip(a, b)) /
                    (sqrt(sum(ai * ai for ai in a)) *
                     sqrt(sum(bi * bi for bi in b))))))
