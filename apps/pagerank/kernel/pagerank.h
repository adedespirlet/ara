// pagerank.h

#ifndef PAGERANK_H
#define PAGERANK_H
#include <math.h>


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <float.h> 
#include <time.h>
#include <stddef.h>
#include <stdbool.h>
#ifndef SPIKE
#include "printf.h"
#endif



#define DAMPING 0.85
#define CONVERGENCE 0.001


uint64_t calculate_page_rank(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column, double *mean_column, double *score_column_new);

#endif