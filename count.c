// Note: Adapted from Dr. Federl's findPrimes.c

// Counts number of primes from standard input using a multithreaded approach.
//
// compile with:
//   $ gcc count.c -O2 -o count -lm -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

#define MAX_THREADS 256

int isPrime(int64_t n, int64_t start, int64_t end);
void *checkIfPrime(void *info);

int primeFlag = 1; // Flag = 1 means the number is prime.
pthread_t threads[MAX_THREADS];

// Thread information
struct thread_info{
  int64_t t_num;    // Number to determine is prime.
  int64_t t_start;  // Starting point.
  int64_t t_end;    // End point.
  int t_spacing;    // How much to increment the counter in the prime check loop.
};


int main( int argc, char ** argv)
{
    // Parse command line arguments.
    int nThreads = 1;
    if( argc != 1 && argc != 2) {
        printf("Uasge: countPrimes [nThreads]\n");
        exit(-1);
    }
    if( argc == 2) {
      nThreads = atoi( argv[1]);
    }

    // Handle invalid arguments.
    if( nThreads < 1 || nThreads > 256) {
        printf("Bad arguments. 1 <= nThreads <= 256!\n");
    }

    // Count the primes.
    printf("Counting primes using %d thread%s.\n",
           nThreads, nThreads == 1 ? "" : "s");
    int64_t count = 0;
    long int status;

    // Array to hold all the information needed for each thread.
    struct thread_info prime_info[nThreads];

    while( 1) {
        int64_t num;
        if( 1 != scanf("%lld", & num)) break;

        if (num < 10000){
          // If number is small or only 1 thread is being used, run the isPrime
          // function.
          if( num > 0 && isPrime(num, 5, sqrt(num))){
            count ++;
          }
        } else{ // Multithreaded solution.
          int64_t start = 5;
          int64_t max = sqrt(num);
          int spacing = 6*nThreads;

          for (int i=0; i< nThreads; i++){

            // Initialize info for thread.
            prime_info[i].t_num = num;
            prime_info[i].t_start = start + 6*i;
            prime_info[i].t_spacing = spacing;
            prime_info[i].t_end = max;

            // Create the threads.
            status = pthread_create(&threads[i], NULL, checkIfPrime,
              (void *)&prime_info[i]);

            if (status != 0){
              printf("pthread_create returned an error code. Error: %ld\n", status);
              exit(-1);
            }
          }

          // Wait for all threads to finish.
          for (int i = 0; i< nThreads; i++){
            status = pthread_join(threads[i], NULL);
            if (status != 0){
              printf("pthread_join returned an error code. Error: %ld\n", status);
              exit(-1);
            }
          }

          // Check primeFlag.
          if (primeFlag == 1){
            count++;
          }
        }
        // Reset primeFlag.
        primeFlag = 1;
    }

    // Report results.
    printf("Found %lld primes.\n", count);

    return 0;
}

// Primality test, if n is prime, return 1, else return 0
int isPrime(int64_t n, int64_t start, int64_t end)
{
     if( n <= 1) return 0; // small numbers are not primes
     if( n <= 3) return 1; // 2 and 3 are prime
     if( n % 2 == 0 || n % 3 == 0) return 0; // multiples of 2 and 3 are not primes
     int64_t i = start;
     while( i <= end) {
         if (n % i == 0 || n % (i+2) == 0){
           return 0;
         }
         i += 6;
     }
     return 1;
}

// Thread handler
void *checkIfPrime(void *info){

  int64_t num = ((struct thread_info *)info)->t_num;
  int64_t start = ((struct thread_info *)info)->t_start;
  int64_t end = ((struct thread_info *)info)->t_end;
  int spacing = ((struct thread_info *)info)->t_spacing;

  if( num <= 1){
    primeFlag = 0; // Small numbers are not primes.
    return NULL;
  }
  if( num <= 3) return NULL; // 2 and 3 are prime.
  if( num % 2 == 0 || num % 3 == 0){
    primeFlag = 0; // Multiples of 2 and 3 are not primes.
    return NULL;
  }
  int64_t i = start;
  while( i <= end) {
      if (num % i == 0 || num % (i+2) == 0){
        primeFlag = 0;
        return NULL;
      }

      if (primeFlag == 0){
        break;
      }

      i += spacing; // With 1 thread, spacing = 6. However, with nThreads,
                    // spacing = 6*nThreads. This is why the multithreaded
                    // solution is faster. Each thread is doing 1/nThreads the
                    // total amount of work.
  }
  return NULL;
}
