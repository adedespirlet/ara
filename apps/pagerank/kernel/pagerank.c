// pagerank.c

#include <stdio.h>
#include <math.h>
#include "pagerank.h"

void init_link_matrix(unsigned int num_pages, double *link_matrix) {
    // Initialize and populate the link matrix 

    unsigned int num_entries=num_pages*num_pages;
    for (unsigned int i = 0; i < num_entries; i++){
        link_matrix[i] = 1/5;
    }

}


void calculate_page_rank(unsigned int num_pages, double *link_matrix, double *score_column, double *mean_column) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n");
    //initialize score and mean column
    init_link_matrix(num_pages,link_matrix);
    for (unsigned int i = 0; i < num_pages; i++) {
		double entry = 1 / (double) num_pages;
		mean_column[i] = entry;
		score_column[i] = entry;
	}
    double sum_of_differences = 0.0;
    do{
        // Store score column before operations
        double prev_score_column[num_pages]; //might have to be alllocate on the heap
        for (unsigned int i = 0; i < num_pages; i++){
            prev_score_column[i] = score_column[i];
        }

        //weigh link matrix 
        unsigned int num_entries=num_pages*num_pages;
        for (unsigned int i = 0; i < num_entries; i++){
            link_matrix[i] *= (1-WEIGHT);
        }

        //weigh mean vector
        num_entries=num_pages;
        for (unsigned int i = 0; i < num_entries; i++){
            mean_column[i] *= WEIGHT;
        }

        //multiple score (pagerank) vector with link matrix 
        for (unsigned int i=0; i<num_pages; i++){
            double sum =0;
            for (int j=0; j<num_pages;j++){
                sum+= link_matrix[i*num_pages+j] * score_column[j];
            }
            score_column[i]= sum;
            printf("score column: %f \t", score_column[i]);
        }

        //add score column with weigthed mean vector
        for (unsigned int i = 0; i < num_pages; i++){
            score_column[i] += mean_column[i];
        }

        //compute abs difference and see if it converges
        
        sum_of_differences = 0.0;
        for (unsigned int i = 0; i < num_pages; i++) {
            sum_of_differences += fabs(score_column[i] - prev_score_column[i]);
        }

    }
    while (sum_of_differences>CONVERGENCE);

    printf("Page Rank has converged\n");

}



