#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>



typedef struct Array {
    int col;
    int row;
    int** array;
} Array_Type;

Array_Type *first_array;
Array_Type *second_array;
Array_Type *final_array;

void print_array(Array_Type *array);
Array_Type *read_array(char* file_name);
void creat_final_array();
void multiply_row_col(int first_array_row,int second_array_col);

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
    creat_final_array();
    //print_array(second_array);

    for(int i = 0 ; i < first_array->row ; i++){
        for(int j = 0 ; j < second_array -> col ; j++){
            multiply_row_col(i,j);
        }
    }
    //print_array(first_array);
    //print_array(second_array);
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

void multiply_row_col(int first_array_row,int second_array_col){
    int target = 0;
    for(int i = 0 ; i < first_array->col ; i++){
        int first = first_array->array[first_array_row][i];
        int second = second_array -> array[i][second_array_col];
        target += (first * second);
    }
    final_array->array[first_array_row][second_array_col] = target;
}