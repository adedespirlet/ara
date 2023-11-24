// pagerank.c


#include "pagerank.h"
void matrix_vector_Mult_Scalar(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column,double *score_column_new){
    for (uint64_t i = 0; i < num_pages; i++) {
           double sum = 0.0;
           for (int64_t idx = row_ptr[i]; idx < row_ptr[i + 1]; idx++) {
               sum += data_array[idx] * score_column[col_array[idx]];
           }
           score_column_new[i] = sum;
       printf("%ld \t", (int64_t)(score_column_new[i] * 10000));
       }
}

void calculate_page_rank(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column, double *mean_column,double *score_column_new) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n");
        
    for (uint64_t i = 0; i < num_pages; i++) {
		double entry = 1 / (double) num_pages;
		mean_column[i] = entry;
		score_column[i] = entry;
	}

    size_t vl;
    
    int64_t *link_matrix_ = (int64_t *)link_matrix ;
    int64_t *score_column_ = (int64_t *)score_column;

    asm volatile("vmv.v.i v4, 0");
    asm volatile("vmv.v.i v8, 0");
    asm volatile("vmv.v.i v16, 0");
    

   do{

        //v4 for score_column_new
        //v16 for score_column
        //v8 for mean_column
        //v12 for temp values
        
        
        matrix_vector_Mult_Scalar(num_pages,data_array,col_array,row_ptr, score_column,score_column_new);
           
        avl=num_pages;
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
        score_column_new_=score_column_new;
        score_column_=score_column;
        for (; avl > 0; avl -= vl) {
            asm volatile("vmv.v.i v12, 0"); //init temp vector to 0
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            
            //load score_column
            asm volatile("vle64.v v16,  (%0)" ::"r"(score_column_));
            asm volatile("vle64.v v4,  (%0)" ::"r"(score_column_new_));
            asm volatile("vmul.vx v16, v16, %0", DAMPING); //weigh score column

            asm volatile("vle64.v v8,  (%0)" ::"r"(mean_column ));
            asm volatile("vmul.vx v8, v8, %0", 1-DAMPING); //weigh mean column


            asm volatile("vadd.vv v4, v16, v8"); // add both weighted vectors and store in score column new
            

            //compute abs difference and add it all up to see if it converges
            asm volatile("vsub.vv v12, v16, v4");
            asm volatile("vfsgnjx.vv v12,v12,v12"); //take absolute value
            asm volatile("vredsum.vs v12, v12, v12") ; //add elements together

            //copy new score column to score column for next iteration
            asm volatile("vse64.v   v4, (%0)" :: "r"(score_column_));

            score_column_new_+=vl;
            score_column_+=vl;
        }
       sum_of_differences = 0.0;

       asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));
       // Store back each new computed pagescore
       asm volatile("vse64.v   v12, (%0)" :: "r"(sum_of_differences));
   }while (sum_of_differences>CONVERGENCE);
  
   printf("Page Rank has converged\n");

}
