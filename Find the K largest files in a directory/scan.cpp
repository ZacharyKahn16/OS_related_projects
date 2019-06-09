/**********************************************
 * Last Name:   Kahn
 * First Name:  Zachary
 * Student ID:  10151534
 * Course:      CPSC 457
 * Tutorial:    T02
 * Assignment:  1
 * Question:    Q3
 *
 * File name: scan.cpp
 *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FNAME_SIZE 512
#define MAX_FILES 1000

// Function prototypes.
int hasSuffix(char *word, const char *suffix);
int comparator(const void *f1, const void *f2);
int get_min(int a, int b);

// File info (required for sorting).
struct file_details {
  long long int f_size; // Size of file.
  char *f_name;         // Name of file.
};

using namespace std;


int main(int argc, char const *argv[]) {

  // Handle command line arguments.
  if( argc != 3) {
    fprintf(stderr, "Exactly 2 command line arguments must be given.\n");
    exit(-1);
  }

  // Suffix being filtered for.
  const char *suff = argv[1];
  // Number of files to display.
  const char *display = argv[2];
  int head = atoi(display);

  // Get list of files in working directory.
  FILE *fp = popen("./myFind", "r");

  if (fp == NULL){
    perror("popen failed:");
    exit(-1);
  }

  // Read in all filenames.
  char buff[MAX_FNAME_SIZE];
  int nFiles = 0;
  char *files[MAX_FILES];
  while (fgets(buff, MAX_FNAME_SIZE, fp)) {
    int len = strlen(buff)-1;
    char *check = strndup(buff,len);

    // Check file has the required suffix.
    if (hasSuffix(check, suff) == 1){
      files[nFiles] = check; // Add file to the list of files.
      nFiles++;
    }
  }

  int ok = fclose(fp);

  if(ok < 0){
    printf("Error closing file.\n");
    exit(-1);
  }

  // Array that store details of each file.
  struct file_details info[nFiles];

  struct stat st;
  // Go through each file and get its size.
  for (int i = 0; i< nFiles; i++){
    if (0 != stat(files[i], &st)){
      perror("stat failed:");
      exit(-1);
    }
    info[i].f_size = st.st_size;
    info[i].f_name = files[i];

  }

  // Sort files by sizes.
  qsort(info, nFiles, sizeof(struct file_details), comparator);

  long long totalSize = 0;

  // Number of files to print to standard output.
  int d = get_min(head, nFiles);

  // Print file name and file size for each file.
  for (int i = 0; i < d; i++){
    printf("%s %lld\n", info[i].f_name, info[i].f_size);
    totalSize += info[i].f_size;
  }
  // Print total size of all the files that were printed to standard output.
  printf("Total size: %lld\n", totalSize);

  return 0;
}

// Check if a file has the specified suffix.
int hasSuffix(char *word, const char *suffix){
  if (word == NULL || suffix == NULL){
    return 0;
  }
  size_t len_suffix = strlen(suffix);
  size_t len_word = strlen(word);

  // If suffix is longer than word, the word does not contain the suffix.
  if (len_suffix > len_word){
      return 0;
  }

  int i;
  // Check the last strlen(suffix) characters of the word and see if it has
  // the inputted suffix.
  for (i=0; i<len_suffix; i++){
    if (word[len_word-len_suffix+i] != suffix[i]){
      break;
    }
  }
  if (i == len_suffix){
    return 1;
  }
  return 0;
}

// Comparison function for qsort.
// Files should be ordered by decreasing file size.
int comparator(const void *f1, const void *f2){
  long long int p = ((struct file_details *)f1)->f_size;
  long long int q = ((struct file_details *)f2)->f_size;

  return (q-p);
}

// Get the minimum of two numbers.
int get_min(int a, int b){
  if (a < b){
    return a;
  }
  return b;
}
