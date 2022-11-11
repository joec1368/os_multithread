#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>



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
Array_Type *read_array(char* file_name);
void creat_final_array();
void* multiply_row_col(void* data);
void create_file();
char proc_addr[] = "./proc/thread_info";  // need to be fixed on vm

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

    create_file();
    first_array = read_array(first_file);
    second_array = read_array(second_file);
    creat_final_array();


    int temp[2];
    int avarge_row_to_do = first_array -> row/ thread_number;
    printf("number : %d \n", thread_number);
    temp[0] = 0;
    temp[1] = first_array -> row - (first_array -> row / thread_number) * thread_number + avarge_row_to_do - 1;

    pthread_t thread[thread_number]; // 宣告 pthread 變數
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
    for(int i = 0 ; i < thread_number ; i++){
        pthread_join(thread[i], NULL);
    }


    print_array(final_array);
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

void creat_final_array(){
    final_array = malloc(sizeof (Array_Type));
    final_array -> row = first_array -> row;
    final_array -> col = second_array -> col;
    final_array->array = calloc(first_array->row, sizeof (int*));
    for(int i = 0 ; i < first_array -> row ; i ++){
        final_array -> array[i] = calloc(second_array->col , sizeof (int));
    }
}

void* multiply_row_col(void* data){
    int *data_Set = (int*) data;
    int first_array_row_start = data_Set[0] ;
    int first_array_row_final = data_Set[1];
    // 0 : row num start
    // 1 : row num end
    // 2 : col number;
    for(int j = first_array_row_start ; j <= first_array_row_final ; j++ ) {
        for(int z =  0 ; z < second_array -> col ; z++){
            int target = 0;
            for (int i = 0; i < first_array->col; i++) {
                int first = first_array->array[j][i];
                int second = second_array->array[i][z];
                target += (first * second);
            }
            final_array->array[j][z] = target;
        }
    }

    pthread_mutex_lock(&lock1);
    FILE *fptr;
    fptr = fopen(proc_addr,"a");
    fscanf(fptr,"\t end : %d \n ", pthread_self());
    fclose(fptr);
    pthread_mutex_unlock(&lock1);
    //fprintf(stderr,"end :");
    free(data);
    pthread_exit(NULL); // 離開子執行緒
}

void create_file(){
    FILE *fptr;
    fptr = fopen(proc_addr,"w");
    fscanf(fptr,"PID:%d\n ", getpid());
    fclose(fptr);
}