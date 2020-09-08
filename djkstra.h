// C program for Dijkstra's single 
// source shortest path algorithm. 
// The program is for adjacency matrix 
// representation of the graph. 
using namespace std;
#include <bits/stdc++.h> 
#include <unistd.h>
#include <iostream>
#include <limits.h> 

// Number of vertices 
// in the graph 
// #define V 9 

// A utility function to find the 
// vertex with minimum distance 
// value, from the set of vertices 
// not yet included in shortest 
// path tree 
int minDistance(int dist[], 
				bool sptSet[]) 
{ 
	
	// Initialize min value 
	int min = INT_MAX, min_index; 

	for (int v = 0; v < V; v++) 
		if (sptSet[v] == false && 
				dist[v] <= min) 
			min = dist[v], min_index = v; 

	return min_index; 
} 

// Function to print shortest 
// path from source to j 
// using parent array 



// Funtion that implements Dijkstra's 
// single source shortest path 
// algorithm for a graph represented 
// using adjacency matrix representation 
int* dijkstra(int graph[V][V], int src) 
{ 
	
	// The output array. dist[i] 
	// will hold the shortest 
	// distance from src to i 
	int dist[V]; 

	// sptSet[i] will true if vertex 
	// i is included / in shortest 
	// path tree or shortest distance 
	// from src to i is finalized 
	bool sptSet[V]; 

	// Parent array to store 
	// shortest path tree 
	static int parent[V]; 

	// Initialize all distances as 
	// INFINITE and stpSet[] as false 
	for (int i = 0; i < V; i++) 
	{ 
		parent[0] = -1; 
		dist[i] = INT_MAX; 
		sptSet[i] = false; 
	} 

	// Distance of source vertex 
	// from itself is always 0 
	dist[src] = 0; 

	// Find shortest path 
	// for all vertices 
	for (int count = 0; count < V - 1; count++) 
	{ 
		// Pick the minimum distance 
		// vertex from the set of 
		// vertices not yet processed. 
		// u is always equal to src 
		// in first iteration. 
		int u = minDistance(dist, sptSet); 

		// Mark the picked vertex 
		// as processed 
		sptSet[u] = true; 

		// Update dist value of the 
		// adjacent vertices of the 
		// picked vertex. 
		for (int v = 0; v < V; v++) 

			// Update dist[v] only if is 
			// not in sptSet, there is 
			// an edge from u to v, and 
			// total weight of path from 
			// src to v through u is smaller 
			// than current value of 
			// dist[v] 
			if (!sptSet[v] && graph[u][v] && 
				dist[u] + graph[u][v] < dist[v]) 
			{ 
				parent[v] = u; 
				dist[v] = dist[u] + graph[u][v]; 
			} 
	} 

	// print the constructed 
	// distance array 
	return parent;
} 

// // Driver Code 
// int main() 
// { 
	
// 	int graph[9][9];
// 	int* parent;
//     std::memset(graph, 0, sizeof(graph[0][0]) * V * V);

//     int place_cube[3][3] = {{0,0,0},{1,1,0},{0,0,0}};

// 	for(int i=0;i<3;i++){
//         for(int j=0;j<(3-1);j++){
//             if(place_cube[i][j]==0 && place_cube[i][j+1]==0){
// 				int p = (3*i) + j;
// 				int q = (3*i) + j + 1;
//                 graph[p][q] = 1;
//                 graph[q][p] = 1;
//             }            
//         }
//     }

//     for(int i=0;i<(3-1);i++){
//         for(int j=0;j<3;j++){
//             if(place_cube[i][j]==0 && place_cube[i+1][j]==0){
//                 int p = (3*i) + j;
// 				int q = (3*(i+1)) + j;
//                 graph[p][q] = 1;
//                 graph[q][p] = 1;
//             }            
//         }
//     }
    

// 	parent = dijkstra(graph, 0); 
// 	cout<<parent[6];
// 	return 0; 
// } 
