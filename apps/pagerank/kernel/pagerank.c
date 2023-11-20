// pagerank.c


#include "pagerank.h"


void calculate_page_rank(unsigned int num_pages, double *link_matrix, double *score_column, double *mean_column,double *score_column_new) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n");
    //initialize score and mean column
    printf("Num_pages :%ld \n",num_pages);
    printf("INitialized score column\n");
    for (unsigned int i = 0; i < num_pages; i++) {
		double entry = 1 / (double) num_pages;
		mean_column[i] = entry;
		score_column[i] = entry;
        printf("%ld \t", (int64_t)(score_column[i]*10000));
        //printf("%a \t", score_column[i]);
	}

    double sum_of_differences = 0.0;
    do{

        printf("entered the do-while loop\n");

        
        printf("Multiplying linking matrix with score column:\n");
        //multiple score (pagerank) vector with link matrix 
        for (unsigned int i=0; i<num_pages; i++){
            double sum =0;
            for (int j=0; j<num_pages;j++){
                sum+= link_matrix[i*num_pages+j] * score_column[j];
            }
            score_column_new[i]= sum;
            printf("%ld \t",(int64_t)(score_column_new[i]*10000));
            //printf("%a \t", score_column[i]);
            //printf("score column: %ld \t", (int64_t)(score_column[i]*100));
        }

        //add damping factor
        unsigned int num_entries=num_pages;
        for (unsigned int i = 0; i < num_entries; i++){
            score_column_new[i] = DAMPING* score_column_new[i] + (1-DAMPING)*mean_column[i];
            //printf("%ld \t",(int64_t)(link_matrix[i]*100));
        }

          //compute abs difference and see if it converges
        sum_of_differences = 0.0;
        for (unsigned int i = 0; i < num_pages; i++) {
            sum_of_differences += fabs(score_column_new[i] - score_column[i]);
        }

        //update score column
        unsigned int num_entries=num_pages;
        for (unsigned int i = 0; i < num_entries; i++){
            score_column[i] = score_column_new[i];
        }

      
    }
    while (sum_of_differences>CONVERGENCE);

    printf("Page Rank has converged\n");

}



