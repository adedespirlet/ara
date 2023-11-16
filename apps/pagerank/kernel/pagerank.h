// pagerank.h

#ifndef PAGERANK_H
#define PAGERANK_H
#include <math.h>

#define MAX_PAGES 100
#define MIN_PAGES 2
#define WEIGHT 0.15
#define CONVERGENCE 0.001

void init_link_matrix(unsigned int num_pages, float *link_matrix));
void calculate_page_rank(unsigned int num_pages, float *link_matrix, float *score_column, float *mean_column);

#endif
