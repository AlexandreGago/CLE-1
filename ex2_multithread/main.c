#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

void *worker(void *arg);
void *distributor (void *arg);

int max_threads = 4;

typedef struct{
    int id;
    pthread_t thread;
    pthread_cond_t worker_ready;
    pthread_mutex_t mutex;
} worker_data;


int main(int argc, char *argv[]) {

    FILE *f;
    int *arr;
    int arr_size;
    // int i;
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Error opening file %s   \n", argv[1]);
    }
    //read the number of elements
    if(fread(&arr_size, sizeof(int), 1, f)<=0){
        printf("Error reading file %s   \n", argv[1]);
        exit(0);
    };
    //allocate memory for the array
    arr = (int *)malloc(arr_size * sizeof(int));
    //read to the array with malloc
    if(fread(arr, sizeof(int), arr_size, f)<=0){
        printf("Error reading file %s   \n", argv[1]);
        exit(0);
    };


    //use struct worker_data to pass data to the threads
    worker_data *workers = malloc(max_threads * sizeof(worker_data));

    for (int i = 0; i < max_threads; i++) {
        worker_data *worker = &workers[i];
        worker->id = i;
        pthread_cond_init(&worker->worker_ready, NULL);
        pthread_mutex_init(&worker->mutex, NULL);
        pthread_create(&worker->thread, NULL, worker, worker);
    }




    //thread creation
    pthread_t threads[max_threads];
    int thread_id[max_threads];
    int i;

    //workers condition variables and mutex
    pthread_cond_t conds[max_threads];
    for (int i = 0; i < max_threads; i++) {
        pthread_cond_init(&conds[i], NULL);
    }
    pthread_mutex_t mutex[max_threads];
    for (int i = 0; i < max_threads; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    //distributor condition variables and mutex
    pthread_cond_t cond_distributor;
    pthread_mutex_t mutex_distributor;


    //create the distribuitor thread
    pthread_t distribuitor;
    pthread_create(&distribuitor, NULL, distributor,  );

    //create the worker threads
    for (i = 0; i < max_threads; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, worker_func, &thread_id[i]);
    }


    //join the threads
    for (i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(distribuitor, NULL);



    // mergeSortItr(arr, arr_size);
    //print the array
    // for (i = 0; i < arr_size; i++) {
    //     printf("%d ", arr[i]);
    // }
    
    //check if array is sorted in crescent order
    // bool is_sorted = true;
    // for (int i = 0; i < arr_size - 1; i++) {
    //     if (arr[i] > arr[i + 1]) {
    //         is_sorted = false;
    //         printf("The array is incorrect at index %d and %d (%d,%d)", i, i + 1, arr[i], arr[i + 1]);
    //     }
    // }
    // if (is_sorted) {
    //     printf("The array is sorted in crescent order \n");
    // } else {
    //     printf("The array is not sorted in crescent order \n");
    // }

    return 0;
}

void *worker_func(void *arg) {

    worker_data *worker = (worker_data *)arg;
    printf("Thread %d created \n", worker->id);
    //signal the distributor that the thread is ready and wait for the signal
    
    while (1) {
        
    }

    // int *id = (int *)arg;
    // printf("Thread %d created \n", *id);
    // //signal the distributor that the thread is ready and wait for the signal
    // int pthread_cond_signal(pthread_cond_t *cond);
    // int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
    // // get an array of numbers from the shared region and sort it
    
    // //signal the distribuitor that the thread is done
    // int pthread_cond_signal(pthread_cond_t *cond);

    // return NULL;
}

void *distributor (void *arg){
    
    printf("Thread distributor created \n");

    while(1){
        //wait for a request from a worker
        int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

        // Send to the shared region subsequences of the array to be sorted

        //signal the worker that the array is ready
        int pthread_cond_signal(pthread_cond_t *cond);
        //wait for all the workers to finish
        // int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
    }
    return NULL;
}