#!/usr/bin/env python3
# Copyright 2022 ETH Zurich and University of Bologna.
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Author: Matteo Perotti

# C = AB with A=[MxN], B=[NxP], C=[MxP]
# arg1, arg2, arg3: M, N, P

import random as rand
import numpy as np
import sys

def emit(name, array, alignment='8'):
  print(".global %s" % name)
  print(".balign " + alignment)
  print("%s:" % name)
  bs = array.data.tobytes()
  for i in range(0, len(bs), 4):
    s = ""
    for n in range(4):
      s += "%02x" % bs[i+3-n]
    print("    .word 0x%s" % s)

############
## SCRIPT ##
############


M = 100
N = 3
P = 3


dtype = np.int64

UPPER_LIMIT = 20
LOWER_LIMIT = 0

# Matrices and results
# Generate random data points
A = np.random.randint(LOWER_LIMIT, UPPER_LIMIT, size=(N, M)).astype(dtype)    ##rows contain the features and each column is a datapoint

# Randomly select P columns from A to initialize K. slected_columns will contain the indices
selected_columns = np.random.choice(M, P, replace=False)
K = A[:, selected_columns]



C = np.zeros([1,M], dtype=dtype) # contains the assigned cluster to each data point
B= np.zeros([1,M],dtype=dtype) ##set empty array to copy last clusters values
# Golden result matrix
#G = np.matmul(A, B).astype(dtype)

# Create the file
print(".section .data,\"aw\",@progbits")
emit("M", np.array(M, dtype=np.uint64))
emit("N", np.array(N, dtype=np.uint64))
emit("P", np.array(P, dtype=np.uint64))
emit("a", A, 'NR_LANES*4')
emit("k", K, 'NR_LANES*4')
emit("c", C, 'NR_LANES*4')
emit("b", B, 'NR_LANES*4')

# # Create the file
# print(".section .data,\"aw\",@progbits")
# emit("M", np.array(M, dtype=np.uint64))
# emit("N", np.array(N, dtype=np.uint64))
# emit("P", np.array(P, dtype=np.uint64))
# emit("a", A, 'NR_LANES*4')
# emit("b", B, 'NR_LANES*4')
# emit("c", C, 'NR_LANES*4')
# emit("g", G, 'NR_LANES*4')

