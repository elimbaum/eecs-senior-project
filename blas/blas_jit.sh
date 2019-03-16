#!/usr/bin/env bash

LD_PRELOAD=/usr/lib/libopenblas.so.0 lli -jit-kind=orc-lazy $@

