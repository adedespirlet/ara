

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


NUM_PAGES= 100
dtype = np.float64
A= np.zeros([NUM_PAGES,NUM_PAGES], dtype=dtype) #init linking matrix
PR= np.zeros([NUM_PAGES,1], dtype=dtype) #init pagerank vector
M= np.zeros([NUM_PAGES,1], dtype=dtype) #init mean vector

# Golden result matrix
#G = np.matmul(A, B).astype(dtype)

# Create the file
print(".section .data,\"aw\",@progbits")
emit("num_pages", np.array(NUM_PAGES, dtype=np.uint64))
emit("a", A, 'NR_LANES*4')
emit("pr", PR, 'NR_LANES*4')
emit("m", M, 'NR_LANES*4')


# # Create the file
# print(".section .data,\"aw\",@progbits")
# emit("M", np.array(M, dtype=np.uint64))
# emit("N", np.array(N, dtype=np.uint64))
# emit("P", np.array(P, dtype=np.uint64))
# emit("a", A, 'NR_LANES*4')
# emit("b", B, 'NR_LANES*4')
# emit("c", C, 'NR_LANES*4')
# emit("g", G, 'NR_LANES*4')

