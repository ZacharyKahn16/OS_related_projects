 deadlock.cpp

 This file prints a list of all processes that are in a deadlock. The
 deadlocked processes are the ones that are in a cycle in the process-resource
 graph, which is also created by the program.

 The structure of the input file is:

 process -> resource  // process is requesting resource
 process <- resource  // process already holds resource
 ...
 # End of graph A
 ...
 process -> resource
 process <- resource
 ...
 # End of graph B
 ...
 process -> resource
 process <- resource

 compile and run as follows:
 $ g++ deadlock.cpp -o deadlock
 $ ./deadlock cycle_test1.txt
              ^ Test file
