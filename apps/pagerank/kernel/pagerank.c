// pagerank.c


#include "pagerank.h"


void calculate_page_rank(uint64_t num_pages, double *link_matrix, double *score_column, double *mean_column,double *score_column_new) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n");
    //initialize score and mean column
    printf("Num_pages :%ld \n",num_pages);
    printf("INitialized score column\n");
    for (uint64_t i = 0; i < num_pages; i++) {
		double entry = 1 / (double) num_pages;
		mean_column[i] = entry;
		score_column[i] = entry;
        printf("%ld \t", (int64_t)(score_column[i]*10000));
        //printf("%a \t", score_column[i]);
	}


    size_t avl=num_pages;
    size_t vl;
    //stripmine
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

    int64_t *link_matrix_ = (int64_t *)link_matrix ;
    int64_t *score_column_ = (int64_t *)score_column;


    do{
        for (uint64_t row=0;row<num_pages;row++){
            link_matrix_=link_matrix+row*num_pages; //set pointer to correct row in memory
            score_column_new_= score_column_new+row;
            score_column_= score_column+row;
            asm volatile("vmv.v.i v4, 0");
            asm volatile("vmv.v.i v8, 0");
            asm volatile("vmv.v.i v12, 0");
            asm volatile("vmv.v.i v, 0");
            ///// MAKING THREE CLUSTERS
            for (; avl > 0; avl -= vl) {
                printf("Vl value is: %ld and avl value is: %ld \n", vl,avl);
                asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

                //load score column and link matrix rows
                asm volatile("vle64.v v4,  (%0)" ::"r"(score_column_ )); 
                //asm volatile("vle64.v v8,  (%0)" ::"r"(score_column_new )); 
                asm volatile("vle64.v v12,  (%0)" ::"r"(link_matrix )); 

                asm volatile("vmacc.vv v4, , v20" ::"r"(t1));
                asm volatile("vmul.vv v16, v4, v12 ") ;// Multiply elements of v1 and v2 and store in vTemp
                asm volatile("vredsum.vs v20, v16, v20") ; //Sum all elements of vTemp and store in vSum

                link_matrix_+=vl;
                score_column_+=vl;

            }
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));

            // Store back each new computed pagescore 
            asm volatile("vse64.v   v20, (%0)" :: "r"(score_column_new_)); 
        }

        avl=num_pages;
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

        score_column_new_=score_column_new;
        score_column_=score_column;
        for (; avl > 0; avl -= vl) {
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            //load pagerank new into vrf
            //add damping factor
            asm volatile("vle64.v v8,  (%0)" ::"r"(score_column_new_)); 
            asm volatile("vmul.vx v8, v8, %0", DAMPING); //weigh score column
            asm volatile("vle64.v v4,  (%0)" ::"r"(mean_column )); 
            asm volatile("vmul.vx v4, v4, %0", 1-DAMPING); //weigh mean column

            asm volatile("vadd.vv v8, v8, v4", 1-DAMPING); // add both weighted vectors
            
            //compute abs difference and add it all up to see if it converges
            //use v28 for result of adding elements of vector
            asm volatile("vsub.vv v24, v8, v4", 1-DAMPING);
            asm volatile("vfsgnjx.vv v24,v24,v24"); //take absolute value
            asm volatile("vredsum.vs v28, v28, v24") ; //add elements together

            //copy new score column to score column for next iteration
            asm volatile("vse64.v   v28, (%0)" :: "r"(score_column_)); 

            score_column_new_+=vl;
            score_column_+=vl;
        }
        sum_of_differences = 0.0;

        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));
        // Store back each new computed pagescore 
        asm volatile("vse64.v   v28, (%0)" :: "r"(sum_of_differences)); 
    }while (sum_of_differences>CONVERGENCE);
    
    printf("Page Rank has converged\n");

}