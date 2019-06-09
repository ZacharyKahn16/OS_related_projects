 /**********************************************
 * This program examines multiple system states consisting of program processes
 * and resources. It is assumed that there are only single resource instances
 * for each resource. The objective of the program is to determine which
 * processes are deadlocked. Processes are deadlocked if the process-resource
 * graph constructed contains a cycle.
 *
 * A cycle detection algorithm is implemented to print out a list of all the
 * processes that are in the cycle, and are therefore in a deadlock state.

 * Please refer to README.txt for more details about input file structure.
 *********************************************/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>


using namespace std;


// Data required for each node.
typedef struct nodeInfo {
  string n_key;
  vector<string> neighbors;
  int status;
} Node;

// Keep track of the of a nodes predecessor in the dfs.
map<string, string> backtracker;
// Graph data structure.
map<string, struct nodeInfo> graph;
// List of deadlocked_processes.
vector<int> deadlocked_processes;

// Cycle detection algorithm.
void findAllCycles(string parent, string id);

int main(int argc, char const *argv[]) {

  ifstream config;       // Configuration file.
  string conffile;       // The configuration file name.

  // Read in the config file name from the commanda-line arguments
  if (argc < 2)
  {
      cout << "Usage: deadlock <config file>\n";
      return 0;
  }
  else
  {
      conffile = argv[1];
  }

  // Open the file.
  config.open(conffile.c_str());

  // Continue reading file until the end is reached.
  while (config){
    string line;
    int caseDone = 0;

    // There can be multiple graphs to inspect per file.
    // Cases are separated by the string "# end of x".
    while (!caseDone){
      getline(config, line);
      char endOfCase = '#';
      char firstChar = line[0];

      // End of case reached.
      if (!config || firstChar == endOfCase){
        caseDone = 1;
        map<string, Node>::iterator iter;
        deadlocked_processes.clear();

        int isDone = 0;
        // Continue cycling through graph until all nodes have been visited.
        while(!isDone){
          int count = 0;
          for (iter = graph.begin(); iter != graph.end(); iter++){
            if (iter->second.status == -1){
              backtracker.clear();
              findAllCycles("START", iter->first);
              break;
            }
            count ++;
          }
          if (count == graph.size()){
            isDone = 1;
          }
        }

        // Print out nodes in a deadlock.
        cout << "Deadlocked processes: ";
        if (deadlocked_processes.empty()){
          cout << "none" << endl;
        } else {
          sort(deadlocked_processes.begin(), deadlocked_processes.end());
          for (int i = 0; i < deadlocked_processes.size(); i++){
            cout << deadlocked_processes[i] << " ";
          }
          cout << endl;
        }

        graph.clear();

      } else {
        // Read in the current line.
        // Line format: Process dir Resource
        // Where dir = -> or <-, indicating direction of edge in graph.
        string process, resource, edgeType;
        istringstream iss(line);
        iss >> process >> edgeType >> resource;
        string key, edge;
        string arrow = "->";
        if (arrow.compare(edgeType) == 0){
          key = "P" + process;
          edge = "R" + resource;
        } else {
          key = "R" + resource;
          edge = "P" + process;
        }
        iss.clear();

        // Check if node already exists in graph.
        if (graph.count(key) >0){
          map<string, Node>::iterator iter;
          iter = graph.find(key);
          // Add edge node to list of key node's neighbors.
          iter->second.neighbors.push_back(edge);
        } else{
          // Instantiate new node.
          Node *node_key = new Node();
          node_key->n_key = key;
          node_key->neighbors.clear();
          node_key->neighbors.push_back(edge);
          node_key->status = -1;
          graph.insert(std::pair<string,struct nodeInfo>(key, *node_key));

          delete node_key;
        }
      }
    }
  }

  config.close();
  return 0;
}

// Algorithm to find all cycles in the graph.
// If a cycle is found, the backtracker map is used to traverse from the node
// where the cycle was detected (n) back along the recursion trail of nodes that
// lead to n until n is reached again. All  process nodes (nodes that start with
// P) along this path are added to the deadlocked_processes vector.
void findAllCycles(string parent, string id){
  // If the node has no outgoing edges, it was never added to the graph so
  // return, indicating it has been visited.
  if (graph.count(id)<=0){
    return;
  }
  // Get the current node associated with id.
  Node u = graph.find(id)->second;

  // If u has already been visited, return.
  if (u.status == 1){
    return;
  } else if (u.status == 0){
    // This means u is in the visiting state and a cycle exists.
    string cur = parent;
    map<string, string>::iterator iter;
    iter = backtracker.find(id);

    Node tmp;
    // If node is a process node, add it to deadlocked_processes.
    if (id[0] == 'P'){
      deadlocked_processes.push_back(stoi(id.substr(1,id.length()-1)));
    }

    // Continue backtracking until the node u is reached again, completing the
    // cycle.
    while(cur.compare(u.n_key) != 0){
      tmp = graph.find(cur)->second;
      if (cur[0] == 'P'){
        deadlocked_processes.push_back(stoi(cur.substr(1,cur.length()-1)));
      }
      iter = backtracker.find(cur);
      cur = iter->second;
    }
    return;
  } else{
    // Explore the current node.
    u.status = 0;
    graph[u.n_key] = u;

    if (!u.neighbors.empty()) {
      for (auto i = u.neighbors.begin(); i != u.neighbors.end(); i++){
        // Key : child  Value: parent
        backtracker[*i] = u.n_key;
        findAllCycles(u.n_key, *i);
      }
    }

    // Mark current node as visited.
    u.status = 1;
    graph[u.n_key] = u;
    return;
  }
}
