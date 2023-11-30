// pagerank.c

#include "pagerank.h"

void matrix_vector_Mult_Scalar(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column,double *score_column_new){
    printf("Matrix-vector multiplication");
    for (uint64_t i = 0; i < num_pages; i++) {
           double sum = 0.0;
           for (int64_t idx = row_ptr[i]; idx < row_ptr[i + 1]; idx++) {
               sum += data_array[idx] * score_column[col_array[idx]];
           }
           score_column_new[i] = sum;
       //printf("%ld \t", (int64_t)(score_column_new[i] * 10000));
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
}

void matrix_vector_Mult_Vector(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column,double *score_column_new){

    unsigned long int vl;
    for (uint64_t i = 0; i < num_pages  ; i++) {
        // printf("Printing row pntr\n");
        // for (uint64_t i = 0; i < 10; i++) {
        //     printf("%d \t",row_ptr[i] );
        // }
        uint64_t idx= row_ptr[i];
        uint64_t end= row_ptr[i+1];
        unsigned long int avl = end -idx;
        
        
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
       
        asm volatile("vmv.v.i v16, 0");
        asm volatile("vmv.v.i v4, 0");
        asm volatile("vmv.v.i v8, 0");
        asm volatile("vmv.v.i v12, 0");
        asm volatile("vmv.v.i v20, 0");

        double *data_array_= data_array+ idx;
        uint64_t *col_array_= col_array+idx;

        for (; avl > 0; avl -= vl) {
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            
            asm volatile("vle64.v v4,  (%0)" ::"r"(data_array_));
            asm volatile("vle64.v v8,  (%0)" ::"r"(col_array_)); // v8 contains indexes for scatter gather
            // printf("\n Printing col_array\n");
            // for (uint64_t i = 0; i < vl; i++) {
            //     printf("%d \t",col_array_[i] );
            // }
            uint64_t factor=8;
            asm volatile ("vmul.vx v8,v8,%0"::"r"(factor));
            

            asm volatile("vloxei64.v v12,(%0),v8"::"r"(score_column)); 


            //perform reduction 
            asm volatile ("vfmul.vv v20,v12,v4");

            asm volatile("vfredusum.vs v16, v20, v16"); //add elements in v4 to first element in v12 and store in first element of v12
            //store sum to appropriate place in memory
            

            data_array_+=vl;
            col_array_+=vl;

        }
        double *score_column_new_= score_column_new +i;
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1)); //change vector length to store single element
        asm volatile("vse64.v   v16, (%0)" :: "r"(score_column_new_));


    }
}




uint64_t calculate_page_rank(uint64_t num_pages, double *data_array,uint64_t *col_array,uint64_t *row_ptr, double *score_column, double *mean_column,double *score_column_new) {
    // Implement the PageRank calculation here
    // compute PR_+1= (1-t)*A*PR + t*v 
    printf("Calculating PageRank...\n");
    uint64_t iteration =0;
    for (uint64_t i = 0; i < num_pages; i++) {

		double entry = 1 / (double) num_pages;
		mean_column[i] = entry;
		score_column[i] = entry;
	}

    unsigned long int vl;
    
    double sum_of_differences=0.0;
    double dampingvalue= (double)DAMPING;
    double dampingmean= (double)(1-DAMPING);

    // Define a maximum number of iterations to prevent infinite loops
    const uint64_t max_iterations = 1000;

    for (iteration = 0; iteration < max_iterations; iteration++) {
        unsigned long int avl=num_pages;
        //v4 for score_column_new
        //v16 for score_column
        //v8 for mean_column
        //v12 for temp values
        
        //matrix_vector_Mult_Scalar(num_pages,data_array,col_array,row_ptr, score_column,score_column_new);
        matrix_vector_Mult_Vector(num_pages,data_array,col_array,row_ptr, score_column,score_column_new);
       
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
      
        asm volatile("vmv.v.i v4, 0");
        asm volatile("vmv.v.i v8, 0");
        asm volatile("vmv.v.i v16, 0");
        asm volatile("vmv.v.i v20, 0");
        
        double *score_column_new_=score_column_new;
    
        double *score_column_=score_column;
     

        for (; avl > 0; avl -= vl) {
            //printf("Avl value is: %ld and vl value is: %ld \n",avl ,vl);
            
            asm volatile("vmv.v.i v12, 0"); //init temp vector to 0
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            //  printf("\nPrinting score column before weigning \n");
            // for (uint64_t i = 0; i < (vl); i++) {
            //     printf("%d \t",(int64_t)(score_column_new_[i]*10000));
            // }

           
            //load score_column
            asm volatile("vle64.v v16,  (%0)" ::"r"(score_column_));
            asm volatile("vle64.v v4,  (%0)" ::"r"(score_column_new_));

            //double fv_temp;
           // asm volatile("fmv.d.x %0, %1" : "=f"(fv_temp) : "r"(dampingvalue));
            asm volatile("vfmul.vf v4, v4, %0":: "f" (dampingvalue)); //weigh score columnnew
            

            asm volatile("vse64.v   v4, (%0)" :: "r"(score_column_new_));
            // printf("\nPrinting score column after weigning \n");
            // for (uint64_t i = 0; i < (vl); i++) {
            //     printf("%d \t",(int64_t)(score_column_new_[i]*10000));
            // }

           // asm volatile("fmv.d.x %0, %1" : "=f"(fv_temp) : "r"(dampingmean));
            asm volatile("vle64.v v8,  (%0)" ::"r"(mean_column ));
            asm volatile("vfmul.vf v8, v8, %0":: "f"(dampingmean)); //weigh mean column
           

            asm volatile("vfadd.vv v4, v4, v8"); // add both weighted vectors and store in score column new
            asm volatile("vse64.v   v4, (%0)" :: "r"(score_column_new_));
            

            //compute abs difference and add it all up to see if it converges
            asm volatile("vsub.vv v12, v16, v4");
            asm volatile("vfsgnjx.vv v12,v12,v12"); //take absolute value
            asm volatile("vredsum.vs v20, v20, v12") ; //add elements together

            //copy new score column to score column for next iteration
            asm volatile("vse64.v   v4, (%0)" :: "r"(score_column_));

            score_column_new_+=vl;
            score_column_+=vl;
        }
        sum_of_differences = 0.0;

        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));
        asm volatile("vse64.v   v20, (%0)" :: "r"(&sum_of_differences));

        

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

}
