/**********************************************
 * Last Name:   Kahn
 * First Name:  Zachary
 * Student ID:  10151534
 * Course:      CPSC 457
 * Tutorial:    T02
 * Assignment:  5
 * Question:    Q6
 *
 * File name: pagesim.cpp
 *********************************************/

 /**********************************************
 * This program takes in a reference string that list the pages to look up, as
 * well as the number of available frames, and computes the number of page
 * faults as well as the final frame state for the Optimal, LRU, and Clock page
 * replacement algorithms.
 *********************************************/

#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <vector>


using namespace std;

// List of functions.
void computeOptimalOutput(vector<int> &refString, int numFrames);
void computeLRUOutput(vector<int> &refString, int numFrames);
void computeClockOutput(vector<int> &refString, int numFrames);
void printResults(vector<int> &frames, int pageFaults, string alg);
int find_ele(int frames[], int maxIndex, int valToFind);

// LRU stack info
struct stack_info {
  int page;   // Page value
  int frame_loc;  // Location in frames array
};

int main(int argc, char const *argv[]) {

  // Ensure user enters a frame size.
  if (argc != 2){
    printf("Please include the number of frames as an arugment.\n");
    exit(-1);
  }

  int numFrames = atoi(argv[1]);
  vector<int> refString;

  // Read in the contents of the file.
  int x;
  cin >> x;
  refString.push_back(x);

  while (cin >> x){
    refString.push_back(x);
  }

  // Run the Optimal, LRU, and Clock page replacement algorithms.
  computeOptimalOutput(refString, numFrames);
  computeLRUOutput(refString, numFrames);
  computeClockOutput(refString, numFrames);

  return 0;
}


/*
 * Computes the Optimal page replacement algorithm.
 */
void computeOptimalOutput(vector<int> &refString, int numFrames){
  // Frames list.
  vector<int> frames;
  vector<int>::iterator pos;

  // Number of page faults.
  int pageFaults = 0;

  for (vector<int>::iterator iter = refString.begin(); iter != refString.end(); iter++){
    // Check if the current frame is in the frames list.
    int curPage = *iter;
    pos = find(frames.begin(), frames.end(), curPage);

    if (pos == frames.end()){
      // Frame not found, therefore, this is a page fault.
      pageFaults++;

      // If the frames list is not full, add the page to it.
      if (frames.size() < numFrames){
        frames.push_back(curPage);
      } else{
        // Evict the page that appears again the latest or does not appear again
        // at all.
        vector<int>::iterator  nextLocation = iter;
        vector<int>::iterator pageToEvict = frames.begin();

        //Look through each page in frame list.
        vector<int>::iterator f_it = frames.begin();

        for (f_it; f_it != frames.end(); f_it++){
          pos = find(iter, refString.end(), *f_it);

          if (pos == refString.end()){
            // This page does not appear again, so replace it.
            pageToEvict = f_it;
            break;
          } else if (pos > nextLocation){
            nextLocation = pos;
            pageToEvict = f_it;
          }
        }
        // Replace the page that appears fartherst ahead in refString with the
        // new page.
        *pageToEvict = curPage;

      }
    }
  }

  // Print out results.
  printResults(frames, pageFaults, "Optimal");

}


/*
 * Computes the least recently used (LRU) page replacment algorithm.
 *
 * A stack-like implementation is used to keep track of the least recently used
 * pages. New pages are added to the top of the stack, and the LRU page is at
 * the bottom of the stack.
 */
void computeLRUOutput(vector<int> &refString, int numFrames){
  // Frames list.
  int frames[numFrames];
  // LRU Stack.
  struct stack_info lruStack[numFrames];

  int pos;
  int pageFaults = 0;
  int loc = 0;

  // Iterate through all the pages in the refString.
  for (vector<int>::iterator iter = refString.begin(); iter != refString.end();
    iter++){

    // Check if the current frame is in the frames list.
    int curPage = *iter;
    // Find the position of the read in page in the frames list.
    pos = find_ele(frames, loc, curPage);

    if (pos == -1){ // The curPage is not in the frames list.

      pageFaults++;

      // If frame is not full, at new page to end of frames list.
      if (loc < numFrames){
        frames[loc] = curPage;
        struct stack_info newInfo = {curPage, loc};
        lruStack[loc] = newInfo;
        loc++;

      } else {
        // Erase the oldest element in the frame list.
        struct stack_info evictedPair = lruStack[0];
        frames[evictedPair.frame_loc] = curPage;

        for (int i = 0; i< numFrames-1; i++){
          lruStack[i] = lruStack[i+1];
        }
        struct stack_info tmp = {curPage, evictedPair.frame_loc};
        lruStack[loc-1] = tmp;

      }

    } else {
      // If curPage is in the frame list, move it to the back of the frames list.
      struct stack_info tmp = {curPage, pos};
      int foundInStack = 0;
      for (int i = 0; i < loc -1; i++){
        if (lruStack[i].page == curPage){
          foundInStack = 1;
        }
        if (foundInStack == 1){
          lruStack[i] = lruStack[i+1];
        }
      }
      lruStack[loc-1] = tmp;

    }

  }

  // Print out results.
  vector<int> framesPrint;
  for (int i = 0; i< loc; i++){
    framesPrint.push_back(frames[i]);
  }
  printResults(framesPrint, pageFaults, "LRU");

}


/*
 * Compute the Clock page replacement algorithm.
 */
void computeClockOutput(vector<int> &refString, int numFrames){
  // Frames list.
  vector<int> frames;
  // Reference bit list.
  vector<int> refBits;
  // Clock head position.
  int clockHead = 0;
  vector<int>::iterator pos;

  int pageFaults = 0;

  // Iterature through the reference string.
  for (vector<int>::iterator iter = refString.begin(); iter != refString.end();
    iter++){

    int curPage = *iter;
    pos = find(frames.begin(), frames.end(), curPage);

    // Check if the page is in the frames list.
    if (pos == frames.end()){

      pageFaults++;

      // If frame is not full, at new page to end of frames list.
      if (frames.size() < numFrames){
        frames.push_back(curPage);
        refBits.push_back(1);
      } else{

        while (refBits[clockHead] == 1){
          refBits[clockHead] = 0;
          clockHead = (clockHead + 1) % numFrames;
        }
        frames[clockHead] = curPage;
        refBits[clockHead] = 1;
        clockHead = (clockHead + 1) % numFrames;

      }
    } else {
      // If the page is already in, mark its refBit as 1.
      refBits[pos - frames.begin()] = 1;
    }
  }

  // Print out results.
  printResults(frames, pageFaults, "Clock");

}

/*
 * Print out the results.
 */
void printResults(vector<int> &frames, int pageFaults, string alg){

  // Print out results.
  cout << alg << ":" << endl;
  cout << " - frames: ";
  for (vector<int>::iterator it = frames.begin(); it != frames.end(); it++){
    cout << *it << " ";
  }
  cout << endl;
  cout << " - page faults: " << pageFaults << endl;

}


/*
 * Get the index of value in the frames list.
 *
 * If it exists, its index is returned. Otherwise -1 is returned.
 */
int find_ele(int frames[], int maxIndex, int valToFind){
  int foundIndex = -1;
  for (int i = 0; i < maxIndex; i++){
    if (frames[i] == valToFind){
      foundIndex = i;
      break;
    }
  }
  return foundIndex;
}
