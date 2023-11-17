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


data_points = 50
dimension = 3
P = 3


dtype = np.int64

UPPER_LIMIT = 50
LOWER_LIMIT = 0

np.random.seed(42)
# Matrices and results
# Generate random data points
A = np.random.randint(LOWER_LIMIT, UPPER_LIMIT, size=(dimension, data_points)).astype(dtype)    ## rows contain the features and each column is a datapoint

np.random.seed(42)  # Resetting/setting the seed for reproducibility

# Create an array of column indices
column_indices = np.arange(A.shape[1])  # This creates an array [0, 1, 2, ..., M-1]

# Randomly select P columns from A to initialize K. selected_columns will contain the indices
selected_columns = np.random.choice(column_indices, P, replace=False)

# Use the selected indices to get columns from A
K = A[:, selected_columns]


C = np.zeros([1,data_points], dtype=dtype) # contains the assigned cluster to each data point
B= np.zeros([1,data_points],dtype=dtype) ##set empty array to copy last clusters values
# Golden result matrix
#G = np.matmul(A, B).astype(dtype)

##GOLDEN MODEL

def kmeans(data, centers, max_iter=100):
    num_points = data.shape[1]
    num_clusters = centers.shape[1]
    clusters = np.zeros(num_points, dtype=np.int64)

    for iteration in range(max_iter):
        # Assignment step
        for i in range(num_points):
            distances = np.sum((data[:, i, None] - centers) ** 2, axis=0)
            clusters[i] = np.argmin(distances)

        # Update step
        new_centers = np.zeros_like(centers)
        for k in range(num_clusters):
            cluster_points = data[:, clusters == k]
            if cluster_points.size > 0:
                new_centers[:, k] = np.sum(cluster_points, axis=1) // cluster_points.shape[1]

        # Check for convergence
        if np.array_equal(centers, new_centers):
            break

        centers = new_centers

    return clusters, centers

result, updated_centers = kmeans(A, K)

# Create the file
print(".section .data,\"aw\",@progbits")
emit("data_points", np.array(data_points, dtype=np.uint64))
emit("dimension", np.array(dimension, dtype=np.uint64))
emit("P", np.array(P, dtype=np.uint64))
emit("a", A, 'NR_LANES*4')
emit("k", K, 'NR_LANES*4')
emit("c", C, 'NR_LANES*4')
emit("b", B, 'NR_LANES*4')
emit("golden_o", result, 'NR_LANES*4')



