#include <iostream>
#include <climits>
#include <iomanip>
#include <string>

using namespace std;

/* Project Title: City Map Finder using Dijkstra Algorithm [cite: 1, 13, 15]
   Algorithm: Dijkstra's Algorithm (Single Source Shortest Path) [cite: 1, 33, 34]
   Authors: Musaib Khan, Ali Abbas Qazi, Muhammad Ameer Khan [cite: 1, 5, 6, 7, 8]
*/

const int numCities = 11; // 1-10 usable indices

// Function to find the city with the minimum distance value [cite: 1, 44, 60]
int minimumDist(int dist[], bool Tset[]) {
    int min = INT_MAX, index;
    for (int i = 1; i <= 10; i++) { // Using 1-10 index [cite: 1, 67]
        if (Tset[i] == false && dist[i] <= min) {
            min = dist[i];
            index = i;
        }
    }
    return index;
}

// Recursive function to reconstruct the shortest path using parent array [cite: 1, 39, 61]
void printPath(int parent[], int j, string names[]) {
    if (parent[j] == -1) {
        return;
    }
    printPath(parent, parent[j], names);
    cout << " -> " << names[j];
}

void Dijkstra(int graph[numCities][numCities], int src, int dest, string names[]) {
    int dist[numCities];     // Shortest distance array [cite: 1, 37, 52]
    bool Tset[numCities];    // Visited status array [cite: 1, 38, 52]
    int parent[numCities];   // Path reconstruction array [cite: 1, 39, 56]

    // Step 1: Initialize all distances as Infinite and Tset as false [cite: 1, 41, 59]
    for (int i = 1; i <= 10; i++) {
        dist[i] = INT_MAX;
        Tset[i] = false;
        parent[i] = -1;
    }

    // Step 2: Distance of start node to itself is 0 [cite: 1, 42, 59]
    dist[src] = 0;

    // Step 3: Process nodes [cite: 1, 43, 60]
    for (int count = 1; count <= 10; count++) {
        int u = minimumDist(dist, Tset); // Select unvisited node with min distance [cite: 1, 44]
        Tset[u] = true; // Mark node as visited [cite: 1, 45]

        for (int v = 1; v <= 10; v++) {
            // Update adjacent distances if a shorter path is found [cite: 1, 46, 60]
            if (!Tset[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u; // Track route for path reconstruction [cite: 1, 61]
            }
        }
    }

    // Output formatting matching report requirements [cite: 1, 65, 70]
    cout << "\n=============================================" << endl;
    cout << "          CITY MAP FINDER SYSTEM" << endl;
    cout << "        (Using Dijkstra Algorithm)" << endl;
    cout << "=============================================" << endl;
    cout << "\nStart Location        : " << names[src] << endl;
    cout << "Destination Location  : " << names[dest] << endl;
    
    cout << "\n---------------------------------------------" << endl;
    cout << "Shortest Distances from " << names[src] << endl;
    cout << "---------------------------------------------" << endl;
    cout << left << setw(18) << "City" << "Distance" << endl;
    cout << "---------------------------------------------" << endl;
    
    for (int i = 1; i <= 10; i++) {
        string dStr = (dist[i] == INT_MAX) ? "INF" : to_string(dist[i]) + " km";
        cout << left << setw(18) << names[i] << dStr << endl;
    }
    cout << "---------------------------------------------" << endl;

    cout << "\nShortest Route to Destination:" << endl;
    if (dist[dest] == INT_MAX) {
        cout << "No route exists." << endl;
    } else {
        cout << "Path: " << names[src]; 
        printPath(parent, dest, names); // Display path [cite: 1, 48, 71]
        cout << "\nTotal Distance Required: " << dist[dest] << " km" << endl; // [cite: 1, 48, 72]
    }
    cout << "=============================================" << endl;
}

int main() {
    // City names mapping (Index 1-10) [cite: 1, 64]
    string cities[numCities] = {
        "", "Peshawar", "Islamabad", "Multan", "Lahore", "Rohri", 
        "Dadu", "Gawadar", "Hyderabad", "Quetta", "Karachi"
    };

    // Adjacency Matrix with actual approximate distances (11x11) 
    int graph[numCities][numCities] = {0};
    
    // Peshawar connections
    graph[1][2] = 180; graph[2][1] = 180; // Pes-Isb
    // Islamabad connections
    graph[2][3] = 540; graph[3][2] = 540; // Isb-Mul
    graph[2][4] = 370; graph[4][2] = 370; // Isb-Lhr
    // Multan connections
    graph[3][4] = 350; graph[4][3] = 350; // Mul-Lhr
    graph[3][5] = 400; graph[5][3] = 400; // Mul-Roh
    graph[3][9] = 600; graph[9][3] = 600; // Mul-Que
    // Rohri connections
    graph[5][6] = 230; graph[6][5] = 230; // Roh-Dad
    graph[5][8] = 300; graph[8][5] = 300; // Roh-Hyd
    // Dadu connections
    graph[6][8] = 160; graph[8][6] = 160; // Dad-Hyd
    graph[6][7] = 800; graph[7][6] = 800; // Dad-Gaw
    // Gawadar connections
    graph[7][8] = 650; graph[8][7] = 650; // Gaw-Hyd
    graph[7][9] = 900; graph[9][7] = 900; // Gaw-Que
    // Hyderabad connections
    graph[8][10] = 165; graph[10][8] = 165; // Hyd-Khi
    // Quetta connections
    graph[9][10] = 700; graph[10][9] = 700; // Que-Khi

    int start, target;
    cout << "--- CITY SELECTION MENU ---" << endl;
    for(int i=1; i<=10; i++) cout << i << ". " << cities[i] << endl;
    
    cout << "\nEnter Start Location (1-10): ";
    cin >> start;
    cout << "Enter Destination (1-10): ";
    cin >> target;

    if (start >= 1 && start <= 10 && target >= 1 && target <= 10) {
        Dijkstra(graph, start, target, cities);
    } else {
        cout << "Invalid Input! Enter 1-10." << endl;
    }

    return 0;
}