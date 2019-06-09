/**********************************************
 * Multithreaded summation of integers.
 *********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define MAX_NUMBERS 1000000 // Maximum number of numbers in the inputted file.

void *thread_summer(void *info); // Method that is run by each thread.

// Initialize arrays where the data read in and the partial sums can be stored.
int thread_sums[MAX_NUMBERS] = {0};
int data[MAX_NUMBERS];

// Information required to compute a partial sum.
struct partial_sum_info {
  int thread_id; // ID of the thread.
  int t_range;   // Number a integers that need to be summed.
  int t_start;   // Index where partial sum should begin.
};

int main(int argc, char const *argv[]) {

  // Handle command line arguments.
  if( argc != 3) {
    fprintf(stderr, "Exactly 2 command line arguments must be given.\n");
    exit(-1);
  }

  // Get the number of threads requested.
  int num_threads = atoi(argv[2]);

  if (num_threads <1){
    fprintf(stderr, "Number of threads must be greater than 0.\n");
    exit(-1);
  }

  FILE *fp;

  // Get the inputted file.
  fp = fopen(argv[1], "r");


  if (fp == NULL){
    perror("Error opening file.");
    exit(-1);
  }

  // Keep track of the number of numbers in the inputted file.
  int count = 0;

  int n;
  // Read in the file.
  while (fscanf(fp,"%d\n", &n) != EOF){
    data[count] = n; // Store file contents in the global data array.
    count++;

  }

  int ok = fclose(fp);

  if(ok < 0){
    printf("Error closing file.\n");
    exit(-1);
  }

  // Check that the number of threads is less than the number of numbers in
  // the inputted file.
  if (count < num_threads){
    printf("Number of threads: %d Numbers in file: %d\n", num_threads, count);
    fprintf(stderr, "The number of threads must be <= numbers in file.\n");
    exit(-1);
  }


  int start = 0; // Starting index to read from in global array.
  pthread_t threads[num_threads]; // Number of threads.
  struct partial_sum_info info[num_threads];
  long int status;
  int range; // Number of numbers to sum in a given thread.

  // Create the threads.
  for (int i = 0; i<num_threads; i++){
    if (i < count % num_threads){
      range = count/num_threads +1;
    } else{
      range = count/num_threads;
    }

    // Enter the information required for the thread.
    info[i].thread_id = i;
    info[i].t_range = range;
    info[i].t_start = start;
    status = pthread_create(&threads[i], NULL, thread_summer, (void *)&info[i]);

    if (status != 0){
      printf("pthread_create returned error code %ld \n", status);
      exit(-1);
    }
    // Update the starting index for the next thread.
    start += range;
  }

  // Wait for all threads to finish.
  for (int i =0; i<num_threads; i++){
    status = pthread_join(threads[i], NULL);
    if (status != 0){
      printf("pthread_join returned error code %ld \n", status);
      exit(-1);
    }
  }

  int total_sum = 0;

  // Print out the partial sum for each thread.
  for (int i = 0; i<num_threads; i++){
    printf("Thread %d: %d\n", i+1, thread_sums[i]);
    total_sum += thread_sums[i];
  }
  // Print out the total sum.
  printf("Sum = %d\n", total_sum);

  return 0;
}

// Have thread compute a partial sum.
void *thread_summer(void *info){
  int thread_ID = ((struct partial_sum_info*)info)->thread_id;
  int range = ((struct partial_sum_info*)info)->t_range;
  int start = ((struct partial_sum_info*)info)->t_start;

  for (int i = 0; i<range; i++){

    thread_sums[thread_ID] += data[start + i];
  }
  return NULL;
}
