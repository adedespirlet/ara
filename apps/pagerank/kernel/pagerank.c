// pagerank.c


#include "pagerank.h"

uint64_t calculate_page_rank(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column, double *mean_column,double *score_column_new) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n");
    double entry = 1 / (double) num_pages;
    for (uint64_t i = 0; i < num_pages; i++) {
        mean_column[i] = entry;
        score_column[i] = entry;
    }

   
    // printf("Printing col data array\n");
    // for (uint64_t i = 0; i < 2563; i++) {
    //     printf("%d \t",col_array[i]);
    // }
    // printf("\nPrinting data array\n");
    // for (uint64_t i = 0; i < 2563; i++) {
    //     printf("%ld \t", (int64_t)data_array[i]*10000);
    // }
    // printf("\nPrinting pntr array\n");
    // for (uint64_t i = 0; i < (num_pages+1); i++) {
    //     printf("%d \t",row_ptr[i]);
    // }
    uint64_t iteration=0;
    double sum_of_differences = 0.0;

    const uint64_t max_iterations = 1000;

    for (iteration = 0; iteration < max_iterations; iteration++) {
    
        printf("Iteration Number: %d",iteration);
        printf("Multiplying linking matrix with score column:\n");
     
        for (uint64_t i = 0; i < num_pages; i++) {
            double sum = 0.0;
            for (int64_t idx = row_ptr[i]; idx < row_ptr[i + 1]; idx++) {
                sum += data_array[idx] * score_column[col_array[idx]];
            }
            score_column_new[i] = sum;
        //printf("%ld \t", (int64_t)(score_column_new[i] * 10000));
        }

        //add damping factor
        unsigned int num_entries=num_pages;
        for (uint64_t i = 0; i < num_entries; i++){
            score_column_new[i] = DAMPING* score_column_new[i] + (1-DAMPING)*mean_column[i];
            //printf("%ld \t",(int64_t)(link_matrix[i]*100));
        }

          //compute abs difference and see if it converges
        sum_of_differences = 0.0;
        for (uint64_t i = 0; i < num_pages; i++) {
            sum_of_differences += fabs(score_column_new[i] - score_column[i]);
        }

        //update score column
        for (uint64_t i = 0; i < num_entries; i++){
            score_column[i] = score_column_new[i];
        }

         //break; //add a break statement to stop after first iteration

        // Check for convergence
        if (sum_of_differences <= CONVERGENCE) {
            break;
        }
        
    }

    double sum=0.0;
    for (uint64_t i = 0; i < (num_pages); i++) {
                sum+=score_column[i];
    } 
    printf("sum of all score is %ld \n",(int64_t)sum*10000);

    printf("PageRank has %s\n", (sum_of_differences <= CONVERGENCE) ? "converged" : "reached maximum iterations");
    
    return iteration;

    printf("Page Rank has converged\n");

}

