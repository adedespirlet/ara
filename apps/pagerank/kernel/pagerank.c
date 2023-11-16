// pagerank.c

#include <stdio.h>
#include <math.h>
#include "pagerank.h"

void init_link_matrix(int num_pages, float *link_matrix) {
    // Initialize and populate the link matrix 

    unsigned int num_entries=num_pages*num_pages;
    for (int i = 0; i < num_entries; i++){
        *link_matrix = 1/5;
    }

}


void calculate_page_rank(int num_pages, float *link_matrix, float *score_column, float *mean_column) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n")
    //initialize score and mean column
    for (int i = 0; i < num_pages; i++) {
		float entry = 1 / (float) num_pages;
		mean_column[i] = entry;
		score_column[i] = entry;
	}

    do{
        // Store score column before operations
        float prev_score_column[num_pages]; //might have to be alllocate on the heap
        for (int i = 0; i < num_pages; i++){
            prev_score_column[i] = score_column[i];
        }

        //weigh link matrix 
        unsigned int num_entries=num_pages*num_pages;
        for (int i = 0; i < num_entries; i++){
            *link_matrix++ *= (1-WEIGHT);
        }

        //weigh mean vector
        num_entries=num_pages;
        for (int i = 0; i < num_entries; i++){
            *mean_column++ *= WEIGHT;
        }

        //multiple score (pagerank) vector with link matrix 
        float product[num_cols];
        for (int i=0; i<num_pages, i++){
            float sum =0;
            float *temp= score_column;
            for (int j=0; j<num_pages;j++){
                sum+= *link_matrix++ * *temp++:
            }
            score_column[i]= sum;
            printf("score column: %f \t", score_column[i]);
        }

        //add score column with weigthed mean vector
        for (int i = 0; i < num_pages; i++){
            *score_column++ += *mean_column++;
        }

        //compute abs difference and see if it converges
        
        float sum_of_differences = 0.0;
        for (int i = 0; i < num_pages; i++) {
            sum_of_differences += fabs(score_column[i] - prev_score_column[i]);
        }

    }
    while (sum_of_differences>CONVERGENCES);

    printf("Page Rank has converged\n");

}



