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


#define WEIGHT 0.15
#define CONVERGENCE 0.001


void calculate_page_rank(unsigned int num_pages, double *link_matrix, double *score_column, double *mean_column, double *score_column_prev);

#endif
