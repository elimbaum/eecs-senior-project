#!/usr/bin/env python3

import random

vec_len = (600, 700)
num_vecs = 1000

second = False

n = random.randint(*vec_len)

for _ in range(2 * num_vecs):
    for _ in range(n):
        print("{:.5}".format(random.random()), end=" ")
    print()
    if second:
        n = random.randint(*vec_len)
        print(";")

    second = not second
