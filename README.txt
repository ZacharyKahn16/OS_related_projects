OS_related_projects

1. Deadlock detection algorithm.
  Refer to README.txt.

2. Find the K largest files in a directory.
  Implemented using a bash script and C.
  Input the suffix of the file (ex. pdf) and the number of files to display (K).
  The current and all subdirectories are searched to find all files that end in the
  specified suffix and the K largest files are displayed. 

3. count.c
  Counts the number of prime numbers in a file using a multithreaded solution for improved performance.
  Performance increase is proportional to the number of cores a given machine has.

4. pagesim.cpp
  This program takes in a reference string that list the pages to look up, as
  well as the number of available frames, and computes the number of page
  faults as well as the final frame state for the Optimal, LRU, and Clock page
  replacement algorithms.
  
  Example input:
    Ref string: 1 2 3 4 1 2 5 1 2 3 4 5
    Num frames: 3
  Example output:
    Optimal:
      - frames: 4 2 5 
      - page faults: 7
    LRU:
      - frames: 3 4 5 
      - page faults: 10
    Clock:
      - frames: 5 3 4 
      - page faults: 9
      
5. scheduler.cpp
  Simulated output of a schedule second by second is made. The available
  scheduling algorithms to run are Round Robin and Shortest Job First. The time
  quantum can be specified as a command line input.
 
6. sum.cpp
  Summation of list of integers using a multithreaded solution.
