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

# Number of pages
NUM_PAGES= 25

# Probability of an edge existing between two nodes
p = 0.4

DAMPING = 0.85
CONVERGENCE = 1e-6



# Create an empty adjacency matrix
adj_matrix = [[0 for _ in range(NUM_PAGES)] for _ in range(NUM_PAGES)]

# Populate the adjacency matrix with edges
for i in range(NUM_PAGES):
    for j in range(NUM_PAGES):
        if i != j and rand.random() < p:
            adj_matrix[i][j] = 1

# Function to normalize columns of the matrix
def normalize_columns(matrix):
  n = len(matrix)
  column_sums = [sum(column) for column in zip(*matrix)]
  return [
      [1/n if column_sums[i] == 0 else row[i]/column_sums[i] for i in range(len(row))] 
      for row in matrix
  ]


# Create the link matrix
link_matrix = normalize_columns(adj_matrix)
# Convert list to numpy array
A = np.array(link_matrix, dtype=np.float64)


dtype = np.float64
#A= np.zeros([NUM_PAGES,NUM_PAGES], dtype=dtype) #init linking matrix
PR= np.zeros([NUM_PAGES,1], dtype=dtype) #init pagerank vector
PR_new=np.zeros([NUM_PAGES,1], dtype=dtype) #init pagerank vector
M= np.zeros([NUM_PAGES,1], dtype=dtype) #init mean vector


# Initialize score and mean column
PR_ = np.ones(NUM_PAGES) / NUM_PAGES
mean_column_ = np.ones(NUM_PAGES) / NUM_PAGES

# Calculate PageRank
def calculate_page_rank(A, PR_, mean_column_, damping=DAMPING, convergence=CONVERGENCE):
    num_pages = len(PR_)
    while True:
        PR_new_ = np.dot(A, PR_)
        PR_new_ = damping * PR_new_ + (1 - damping) * mean_column_
        if np.sum(np.abs(PR_new_ - PR_)) < convergence:
            break
        PR_ = PR_new_
    return PR_new_

# Compute and display PageRank scores
result = calculate_page_rank(A, PR_, mean_column_)

# Golden result matrix
#G = np.matmul(A, B).astype(dtype)

# Create the file
print(".section .data,\"aw\",@progbits")
emit("num_pages", np.array(NUM_PAGES, dtype=np.uint64))
emit("a", A, 'NR_LANES*4')
emit("pr", PR, 'NR_LANES*4')
emit("pr_new", PR_new, 'NR_LANES*4')
emit("m", M, 'NR_LANES*4')
emit("golden_o", result, 'NR_LANES*4')

