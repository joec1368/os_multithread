#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>


typedef struct Array {
    int col;
    int row;
    int** array;
} Array_Type;

Array_Type *first_array;
Array_Type *second_array;
Array_Type *final_array;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

void print_array(Array_Type *array);
Array_Type * reverse(Array_Type *ori_array);
Array_Type *read_array(char* file_name);
void creat_final_array();
void* multiply_row_col(void* data);
void write_array_into_file(Array_Type *array);
void* multiply_col_row(void* data);
char answer[] = "./answer.txt";
char proc[] = "/proc/thread_info";

int main(int argc, char *argv[]) {
    int thread_number;
    char* first_file;
    char* second_file;
    if(argc < 4) {
        printf("need 3 arguments");
        printf("%d",argc);
        return 0;
    }else{
        thread_number = atoi(argv[1]);
        first_file = argv[2];
        second_file = argv[3];
    }
    first_array = read_array(first_file);
    second_array = read_array(second_file);
    second_array = reverse(second_array);
    creat_final_array();
    //print_array(second_array);
    pthread_t thread[thread_number];
    printf("number : %d \n", thread_number);
    int temp[2];

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

     if(first_array -> row >= thread_number){
        int avarge_row_to_do = first_array -> row/ thread_number;
        temp[0] = 0;
        temp[1] = first_array -> row - (first_array -> row / thread_number) * thread_number + avarge_row_to_do - 1;

        for(int i = 0 ; i < thread_number ; i++){
            int *buffer;
            buffer = calloc(2,sizeof (int));
            buffer[0] = temp[0];
            buffer[1] = temp[1];
            pthread_create(&thread[i], NULL, multiply_row_col, buffer); // 建立子執行緒
            temp[0] = temp[1] + 1;
            temp[1] += avarge_row_to_do;
        }
        // 0 : row num start
        // 1 : row num end
    }else{
        int avarge_col_to_do = second_array->row/thread_number;
        temp[0] = 0;
        temp[1] = second_array -> row - (second_array -> row / thread_number) * thread_number + avarge_col_to_do - 1;

        for(int i = 0 ; i < thread_number ; i++){
            int *buffer;
            buffer = calloc(2,sizeof (int));
            buffer[0] = temp[0]; // start position
            buffer[1] = temp[1]; // the number that need to be multiplied , the unit is col
            pthread_create(&thread[i], NULL, multiply_col_row, buffer); // 建立子執行緒
            temp[0] = temp[1] + 1;
            temp[1] += avarge_col_to_do;
        }
        // 0 : col num start
        // 1 : col num end
    }

    for(int i = 0 ; i < thread_number ; i++){
        pthread_join(thread[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("time : %lf\n",elapsed);
    write_array_into_file(final_array);
    //print_array(final_array);
    free(first_array);
    free(second_array);
    free(final_array);
    

    return 0;
}

Array_Type *read_array(char* file_name){
    FILE *fptr;
    fptr = fopen(file_name,"r");
    Array_Type *array;
    array = malloc(sizeof (Array_Type));
    fscanf(fptr,"%d", &array->row);
    fscanf(fptr,"%d", &array->col);

    array->array = malloc(sizeof (int *) * array->row);
    for(int i = 0 ; i < array->row ; i++){
        array->array[i] = calloc(array->col ,sizeof (int ));
    }
    for(int i = 0 ; i < array->row ; i++){
        for(int j = 0 ; j < array->col ; j++){
            int temp;
            fscanf(fptr,"%d", &temp);
            array->array[i][j] = temp;
        }
    }
    return array;
}

void print_array(Array_Type *array){
    for(int i = 0 ; i < array->row ; i++){
        for(int j = 0 ; j < array->col ; j++){
            printf("%d ", array->array[i][j]);
        }
        printf("\n");
    }
}

void write_array_into_file(Array_Type *array){
    FILE *fptr;
    fptr = fopen(answer,"w");
    fprintf(fptr,"%d %d\n",array-> row, array->col);
    for(int i = 0 ; i < array->row ; i++){
        for(int j = 0 ; j < array->col ; j++){
            fprintf(fptr,"%d ", array->array[i][j]);
        }
        fprintf(fptr,"\n");
    }

    fclose(fptr);

}


void creat_final_array(){
    final_array = malloc(sizeof (Array_Type));
    final_array -> row = first_array -> row;
    final_array -> col = second_array -> row;
    final_array->array = calloc(first_array->row, sizeof (int*));
    for(int i = 0 ; i < first_array -> row ; i ++){
        final_array -> array[i] = calloc(second_array->row , sizeof (int));
    }
}

void* multiply_row_col(void* data){//以row 為主


    int *data_Set = (int*) data;
    int first_array_row_start = data_Set[0] ;
    int first_array_row_final = data_Set[1];
    // 0 : row num start
    // 1 : row num end
    // 2 : col number;
    for(int j = first_array_row_start ; j <= first_array_row_final ; j++ ) {
        for(int z =  0 ; z < second_array -> row ; z++){
            int target = 0;
            for (int i = 0; i < first_array->col; i++) {
                int first = first_array->array[j][i];
                int second = second_array->array[z][i];
                target += (first * second);
            }
            final_array->array[j][z] = target;
        }
    }
    

    pthread_mutex_lock(&lock1);
    FILE *fptr;
    fptr = fopen(proc,"w");
    if(fptr == NULL){
    	printf("is NULL");
    	fflush(stdout);
    }
    fprintf(fptr,"%d\n",syscall(__NR_gettid)); 
    fclose(fptr);
    pthread_mutex_unlock(&lock1);
    //fprintf(stderr,"end : %d \n ",syscall(__NR_gettid));
    //fprintf(stderr,"end :");
    free(data);
    pthread_exit(NULL); // 離開子執行緒
}

void* multiply_col_row(void* data){ // 以col 為主
    int *data_Set = (int*) data;
    int second_array_col_start = data_Set[0] ;
    int second_array_col_final = data_Set[1];
    // 0 : col num start
    // 1 : col num end
    for(int j = second_array_col_start ; j <= second_array_col_final ; j++ ) {
        for(int z =  0 ; z < first_array -> row ; z++){
            int target = 0;
            for (int i = 0; i < first_array->col; i++) {
                int first = first_array->array[z][i];
                int second = second_array->array[j][i];
                target += (first * second);
            }
            final_array->array[z][j] = target;
        }
    }

    pthread_mutex_lock(&lock1);
    FILE *fptr;
    fptr = fopen(proc,"w");
    if(fptr == NULL){
    	printf("is NULL");
    	fflush(stdout);
    }
    fprintf(fptr,"%d\n",syscall(__NR_gettid));
    fclose(fptr);
    pthread_mutex_unlock(&lock1);
    //fprintf(stderr,"end : %d \n ",syscall(__NR_gettid));
    //fprintf(stderr,"end :");
    free(data);
    pthread_exit(NULL); // 離開子執行緒
}

Array_Type * reverse(Array_Type *ori_array){
    Array_Type *array;
    array = malloc(sizeof (Array_Type));
    array -> row = ori_array -> col;
    array -> col = ori_array -> row; 
    
    array->array = malloc(sizeof (int *) * array->row);
    for(int i = 0 ; i < array->row ; i++){
        array->array[i] = calloc(array->col ,sizeof (int ));
    }
    for(int i = 0 ; i < ori_array->row ; i++){
        for(int j = 0 ; j < ori_array->col ; j++){
            array->array[j][i] = ori_array->array[i][j];
        }
    }
    return array; 
}