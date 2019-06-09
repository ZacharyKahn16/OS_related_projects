/**********************************************
 * Last Name:   Kahn
 * First Name:  Zachary
 * Student ID:  10151534
 * Course:      CPSC 457
 * Tutorial:    T02
 * Assignment:  3
 * Question:    Q7
 *
 * File name: scheduler.cpp
*********************************************/

/**********************************************
This program takes an input file in the form of:
Arrival_Time Burst_Time
Ex. config.txt
1 3
3 4

A simulated output of the schedule second by second is made. The available
scheduling algorithms to run are Round Robin and Shortest Job First. The time
quantum can be specified as a command line input.

To run Round Robin on config.txt with time quantum = 3:
$ ./scheduler config.txt RR 3

To run Shortest Job First
$ ./scheduler config.txt SJF
*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <vector>

#define MAX_PROCESSES 30

using namespace std;

struct scheduling_info {
  int id;
  int arrival_time;
  int burst_time;
  int wait_time;
  int state;          // 0 is running, 1 is ready, 2 is non-existent.
  char state_symbol;  // '.' is running, '+' is ready, '' is non-existent.
};

struct SJF_ReadyQ {
  int id;
  int burst_time;
};

// Comparator for priority queue for SJF.
struct CompareJob {
  bool operator()(SJF_ReadyQ const &j1, SJF_ReadyQ const &j2){
    return j1.burst_time > j2.burst_time;
  }
};

void RR(struct scheduling_info process_info[], int num_processes, int quantum);
void SJF(struct scheduling_info process_info[], int num_processes);
int min(int a, int b);
void updateReadyQ(struct scheduling_info process_info[], int num_processes,
   queue <int> &readyQ, int time);
void updatePriorityQ(struct scheduling_info process_info[], int num_processes,
  priority_queue<SJF_ReadyQ, vector<SJF_ReadyQ>, CompareJob> &readyQ, int time);


int main(int argc, char const *argv[]) {

  // Check valid number of arguments are inputted.
  if (argc != 3 && argc != 4){
    printf("Only 3 or 4 command line arguments are allowed.\n");
    exit(-1);
  }

  if (strcmp(argv[2], "RR") != 0 && strcmp(argv[2], "SJF") != 0){
    printf("Only Round Robin (RR) and Shortest Job First (SJF) are valid algorithm options.\n");
    exit(-1);
  }

  if (argc == 4 && strcmp(argv[2], "SJF") == 0){
    printf("SJF requires 3 command line arguments. Please do not include quantum.\n");
    exit(-1);
  }

  if (argc == 3 && strcmp(argv[2], "RR") == 0){
    printf("RR requires 4 command line arguments.\n");
    exit(-1);
  }

  int quant = -1; // Time quantum.
  if (strcmp(argv[2], "RR") == 0){
    quant = atoi(argv[3]);

    if(quant < 1){
      printf("Quantum value must be larger than 0 units.\n");
      exit(-1);
    }
  }

  FILE *fp;

  // Get the inputted file.
  fp = fopen(argv[1], "r");

  if (fp == NULL){
    perror("Error opening file.");
    exit(-1);
  }

  // Array of processes.
  struct scheduling_info processes[MAX_PROCESSES];
  // Keep track of processes.
  int num_processes = 0;
  int arrival;
  int burst;
  // Read in the file.
  while (fscanf(fp,"%d %d\n", &arrival, &burst) != EOF){
    processes[num_processes].id = num_processes;
    processes[num_processes].arrival_time = arrival;
    processes[num_processes].burst_time = burst;
    processes[num_processes].wait_time = 0;
    processes[num_processes].state = 2;
    processes[num_processes].state_symbol = '\0';
    num_processes++;
  }

  int ok = fclose(fp);

  if(ok < 0){
    printf("Error closing file.\n");
    exit(-1);
  }

  printf("Time\t");

  for (int i=0;i<num_processes;i++){
    printf("P%d\t",processes[i].id);
  }
  printf("\n");
  // Ask if this is ok, or if it has to be more exact since this
  // won't look too pretty with lots of processes.
  printf("---------------------------------------------------\n");

  if (strcmp(argv[2], "RR") == 0){
    RR(processes, num_processes, quant);
  } else {
    SJF(processes, num_processes);
  }

  printf("---------------------------------------------------\n");

  double avg_wait_time = 0.0;

  // Display wait times for each process.
  for (int i = 0; i < num_processes; i++){
    printf("P%d waited %.3f sec.\n", processes[i].id, (double)processes[i].wait_time);
    avg_wait_time += (double)processes[i].wait_time;
  }
  // Display average wait time.
  if (num_processes>0){
    printf("Average waiting time = %.3f sec.\n", avg_wait_time/num_processes);
  } else{
    printf("Average waiting time = 0 sec.\n");
  }

  return 0;
}

// Round Robin algorithm.
void RR(struct scheduling_info process_info[], int num_processes,
  int quantum){

  // Ready Queue for waiting processes.
  queue <int> readyQ;

  int time = 0;
  int complete_processes = 0;

  // Update Ready Queue with processes that begin at time 0.
  updateReadyQ(process_info, num_processes, readyQ, time);

  // Continue until all processes have run.
  while (complete_processes < num_processes){

    // If readyQ is not empty:
    if (!readyQ.empty()){

      // Pop off element
      int cur_process = readyQ.front();
      readyQ.pop();
      process_info[cur_process].state = 0; // Process is now running.
      process_info[cur_process].state_symbol = '.';

      int runs = min(process_info[cur_process].burst_time, quantum);

      for (int i = 0; i< runs; i++){

        process_info[cur_process].burst_time = process_info[cur_process].burst_time -1;

        printf("%d\t", time);
        // Update all wait times of all processes in readyQ
        for (int j = 0; j<num_processes; j++){
          if (process_info[j].state == 1){
            process_info[j].wait_time += 1;
          }
          printf("%c\t", process_info[j].state_symbol);
        }
        printf("\n");

        time++;

        // Add process to Ready Queue if it is not done by the end of quantum.
        // TOOD: Ask if this handles FCFS tie-breakers correctly.
        if (i == runs -1 && process_info[cur_process].burst_time >0){
          process_info[cur_process].state = 1;
          process_info[cur_process].state_symbol = '+';
          readyQ.push(cur_process);
        }
        // Add processes to the Ready Queue that arrive at current time.
        updateReadyQ(process_info, num_processes, readyQ, time);
      }

      // Check if process has completed.
      if (process_info[cur_process].burst_time <= 0){
        complete_processes++;
        process_info[cur_process].state = 2;
        process_info[cur_process].state_symbol = '\0';
      }
    } else{
      // No process is in Ready Queue.
      // Increase time by 1
      printf("%d\t", time);
      for (int i = 0; i<num_processes; i++){
        printf("%c\t", process_info[i].state_symbol);
      }
      printf("\n");
      time++;
      // Add processes to the Ready Queue that arrive at current time.
      updateReadyQ(process_info, num_processes, readyQ, time);
    }
  }
  return;
}

// Return minimum.
int min(int a, int b){
  if (a<b){
    return a;
  }
  return b;
}

// Update the Ready Queue with new processes that arrive at current time.
void updateReadyQ(struct scheduling_info process_info[], int num_processes,
   queue <int> &readyQ, int time){
     for (int i = 0; i<num_processes; i++){
       // Add processes to readyQ that are at their arrival time.
       if (process_info[i].arrival_time == time){
         process_info[i].state = 1;
         process_info[i].state_symbol = '+';
         readyQ.push(i);
       }
     }
   }


// Shortest Job First algorithm.
void SJF(struct scheduling_info process_info[], int num_processes){

  priority_queue<SJF_ReadyQ, vector<SJF_ReadyQ>, CompareJob> readyQ;
  int time = 0;
  int complete_processes = 0;

  // Update Ready Queue with processes that begin at time 0.
  updatePriorityQ(process_info, num_processes, readyQ, time);

  while (complete_processes < num_processes){

    // If Ready Queue is not empty:
    if (!readyQ.empty()){

      // Pop off element
      struct SJF_ReadyQ cur_process = readyQ.top();
      readyQ.pop();
      process_info[cur_process.id].state = 0;
      process_info[cur_process.id].state_symbol = '.';

      // Run the current process until it has completed.
      for (int i = 0; i< process_info[cur_process.id].burst_time; i++){

        printf("%d\t", time);
        // Update all wait times of all processes in Ready Queue.
        for (int j = 0; j<num_processes; j++){
          if (process_info[j].state == 1){
            process_info[j].wait_time += 1;
          }
          printf("%c\t", process_info[j].state_symbol);
        }
        printf("\n");

        time++;
        // Add processes to the Ready Queue that arrive at current time.
        updatePriorityQ(process_info, num_processes, readyQ, time);
      }
      // Update number of complete processes.
      complete_processes++;
      process_info[cur_process.id].state = 2;
      process_info[cur_process.id].state_symbol = '\0';
    } else{
      // No process is in Ready Queue.
      // Increase time by 1
      printf("%d\t", time);
      for (int i = 0; i<num_processes; i++){
        printf("%c\t", process_info[i].state_symbol);
      }
      printf("\n");
      time++;
      // Add processes to the Ready Queue that arrive at current time.
      updatePriorityQ(process_info, num_processes, readyQ, time);
    }
  }
  return;
}

// Update the Priority Queue with new processes that arrive at current time.
void updatePriorityQ(struct scheduling_info process_info[], int num_processes,
   priority_queue<SJF_ReadyQ, vector<SJF_ReadyQ>, CompareJob> &readyQ, int time){
     for (int i = 0; i<num_processes; i++){
       // Add processes to readyQ that are at their arrival time
       if (process_info[i].arrival_time == time){
         process_info[i].state = 1;
         process_info[i].state_symbol = '+';
         struct SJF_ReadyQ new_job = {i,process_info[i].burst_time};
         readyQ.push(new_job);
       }
     }
   }
