/*******************
 * digitalWallet : payMo digital wallet challenge
 * Author : Fanny Nina-Paravecino
 * Date   : November 2016
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <limits.h>
#include <cstdint>
#include <time.h>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct node{
    int value;
    vector<int> adj;
};

double getWallTime(){
    struct timeval time;
    if(gettimeofday(&time,NULL)){
        printf("Error getting time\n");
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

time_t convertStringToDateTime(string dateTime)
{
    // convert string to wsString
    wstring wsTmp(dateTime.begin(), dateTime.end());
    wstring &ws = wsTmp;
    
    // set the format of the time format: '2014-07-25 20:17:22'
    const wstring dateTimeFormat(L"%Y-%m-%d %H:%M:%S");
    
    // Create a stream which we will use to parse the string,
    // which we provide to constructor of stream to fill the buffer
    wistringstream dateStream(ws);
    
    // Create a tm object to store the parsed date and time
    tm dateTimeInput;
    
    // Now we read from buffer using get_time manipulator
    // and formatting the input appropriately
    dateStream >> std::get_time(&dateTimeInput, dateTimeFormat.c_str());
    
    // Convert the tm structure to time_t value and return
    return mktime(&dateTimeInput);
}
void readFile(string filePath, vector<time_t> *dates, vector<int>* origin, vector<int>* dest)
{
    ifstream fileInput( filePath );
    int numberElements=0;
    for( string line; getline( fileInput, line ); )
    {
        istringstream lineStream( line );
        string field;
        
        // Get Date
        if (!getline( lineStream, field, ',' ))
            break;
        if (field.compare("time")!=0) {
            dates->push_back( convertStringToDateTime(field) );
            
            // Get Origin Node
            if (!getline( lineStream, field, ',' ))
                break;
            origin->push_back( stoi(field) );
            
            // Get Dest Node
            if (!getline( lineStream, field, ',' ))
                break;
            dest->push_back( stoi(field) );
            
            numberElements++;
        }
        
    }
    
}
//void readFileToHashTable(string filePath, unordered_map<int, vector<int>> *hashTableBatch)
//{
//    ifstream fileInput( filePath );
//    int numberElements=0;
//    vector<int> adj;
//    int origin, dest;
//    for( string line; getline( fileInput, line ); )
//    {
//        istringstream lineStream( line );
//        string field;
//        
//        // Get Line information
//        if (!getline( lineStream, field, ',' ))
//            break;
//        if (field.compare("time")!=0) {
//            // Get Origin Node
//            if (!getline( lineStream, field, ',' ))
//                break;
//            origin = stoi(field);
//            
//            // Get Dest Node
//            if (!getline( lineStream, field, ',' ))
//                break;
//            dest = stoi(field);
//            
//            // add from origin to dest
//            adj = hashTableBatch[origin];
//            adj.push_back(dest);
//            hashTableBatch[origin] = adj;
//            
//            // add from dest to origin
//            adj = hashTableBatch[dest];
//            adj.push_back(origin);
//            hashTableBatch[dest] = adj;
//            
//            numberElements++;
//        }
//        
//    }
//}
bool checkExistedTransaction(time_t date, int origin, int dest,
                             vector<time_t> nodesBatchDates,
                             vector<int> nodesBatchOrigin,
                             vector<int> nodesBatchDest){
    bool trusted = false;
    // Search for the origin -> dest
    for (int i=0; i< nodesBatchDates.size(); i++) {
        if(((nodesBatchOrigin.at(i) == origin) && (nodesBatchDest.at(i) == dest) && (date > nodesBatchDates.at(i))) ||
           ((nodesBatchOrigin.at(i) == dest) && (nodesBatchDest.at(i) == origin) && (date > nodesBatchDates.at(i))))
        {
            trusted = true;
        }
    }
    return trusted;
    
}
bool isThereCommonFriends( vector<int> friendsA, vector<int> friendsB){
    bool trusted = false;
    
    // Search for intersection
    for (int i=0; i< friendsA.size(); i++) {
        for (int j=0; j<friendsB.size(); j++) {
            if (friendsA.at(i) == friendsB.at(j)) {
                trusted = true;
                break;
            }
        }
        if (trusted) {
            break;
        }
    }
    return trusted;
    
}
vector<bool> processFeature1(vector<time_t> nodesBatchDates,
                     vector<int> nodesBatchOrigin,
                     vector<int> nodesBatchDest,
                     vector<time_t> nodesStreamDates,
                     vector<int> nodesStreamOrigin,
                     vector<int> nodesStreamDest){
    vector<bool> feature1;
    int origin, dest;
    time_t date;
    for (int i = 0; i < nodesStreamDates.size(); i++) {
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        date = nodesStreamDates.at(i);
        
        //look for the pair in batch
        feature1.push_back(checkExistedTransaction( date, origin, dest,
                                nodesBatchDates, nodesBatchOrigin, nodesBatchDest));
    }
    return feature1;
}

vector<bool> processFeature1Optimized(unordered_map<int, vector<int>> hashTableBatch,
                     vector<time_t> nodesStreamDates,
                     vector<int> nodesStreamOrigin,
                     vector<int> nodesStreamDest){
    vector<bool> feature1;
    int origin, dest;
    time_t date;
    
//    // convert batch date to hashtable
//    unordered_map<int, vector<int>> hashTableBatch;
//    vector<int> adj;
//    for (unsigned i=0; i<nodesBatchDates.size(); i++){
//    	// add from origin to dest
//		adj = hashTableBatch[nodesBatchOrigin.at(i)];
//		adj.push_back(nodesBatchDest.at(i));
//		hashTableBatch[nodesBatchOrigin.at(i)] = adj;
//    	    	
//    	// add from dest to origin
//		adj = hashTableBatch[nodesBatchDest.at(i)];
//		adj.push_back(nodesBatchOrigin.at(i));
//		hashTableBatch[nodesBatchDest.at(i)] = adj;		
//    }
    // process stream data
    for (unsigned i = 0; i < nodesStreamDates.size(); i++) {
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        date = nodesStreamDates.at(i);
        
        //look for the pair in batch
        vector<int> previousTransactions = hashTableBatch[origin];
        vector<int>::iterator it;
        it = find( previousTransactions.begin(), previousTransactions.end(), dest);
        if (it != previousTransactions.end())
        	feature1.push_back(true);
        else
        	feature1.push_back(false);
    }
    return feature1;
}

vector<bool> processFeature2(vector<time_t> nodesBatchDates,
                             vector<int> nodesBatchOrigin,
                             vector<int>nodesBatchDest,
                             vector<time_t>nodesStreamDates,
                             vector<int>nodesStreamOrigin,
                             vector<int>nodesStreamDest){
    
    vector<bool> feature2;
    int origin, dest;
    time_t date;
    for (int i = 0; i < nodesStreamDates.size(); i++) {
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        date = nodesStreamDates.at(i);
        if (!checkExistedTransaction( date, origin, dest,
                                     nodesBatchDates, nodesBatchOrigin, nodesBatchDest)){

            // create friend's list for each one in batch file
            vector<int> originFriends;
            vector<int> destFriends;
            for (int j=0; j < nodesBatchDates.size(); j++) {
                // look for origin's friends in origin and dest batch
                if ((nodesBatchOrigin.at(j)== origin) && date > nodesBatchDates.at(j))
                    originFriends.push_back(nodesBatchDest.at(j));
                if ((nodesBatchDest.at(j)== origin) && date > nodesBatchDates.at(j))
                    originFriends.push_back(nodesBatchOrigin.at(j));
                // for for dest's friends in origin and dest batch
                if ((nodesBatchOrigin.at(j)== dest) && date > nodesBatchDates.at(j))
                    destFriends.push_back(nodesBatchDest.at(j));
                if ((nodesBatchDest.at(j)== dest) && date > nodesBatchDates.at(j))
                    destFriends.push_back(nodesBatchOrigin.at(j));
            }
            feature2.push_back(isThereCommonFriends(originFriends, destFriends));
        }
        else
            feature2.push_back(true);
    }
    return feature2;
}
vector<bool> processFeature2Optimized(unordered_map<int, vector<int>> hashTableBatch,
                             vector<time_t>nodesStreamDates,
                             vector<int>nodesStreamOrigin,
                             vector<int>nodesStreamDest){
    
	vector<bool> feature2;
	int origin, dest;
	time_t date;
	
//	// convert batch date to hashtable
//	unordered_map<int, vector<int>> hashTableBatch;
//	vector<int> adj;
//	for (unsigned i=0; i<nodesBatchDates.size(); i++){
//		// add from origin to dest
//		adj = hashTableBatch[nodesBatchOrigin.at(i)];
//		adj.push_back(nodesBatchDest.at(i));
//		hashTableBatch[nodesBatchOrigin.at(i)] = adj;
//				
//		// add from dest to origin
//		adj = hashTableBatch[nodesBatchDest.at(i)];
//		adj.push_back(nodesBatchOrigin.at(i));
//		hashTableBatch[nodesBatchDest.at(i)] = adj;		
//	}
    
    for (int i = 0; i < nodesStreamDates.size(); i++) {
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        date = nodesStreamDates.at(i);
        
        //look for the pair in batch
		vector<int> previousTransactionsOrigin = hashTableBatch[origin];
		vector<int> previousTransactionsDest = hashTableBatch[dest];
        
		vector<int>::iterator it;
		it = find( previousTransactionsOrigin.begin(), previousTransactionsOrigin.end(), dest);
        if (it != previousTransactionsOrigin.end()){
			feature2.push_back(true);
        }
		else
		{
			bool friendOfFriend = false;
            
			for (int i=0; i < previousTransactionsOrigin.size(); i++){
				it = find( previousTransactionsDest.begin(), previousTransactionsDest.end(), previousTransactionsOrigin.at(i));
				
				if (it != previousTransactionsDest.end()){
					friendOfFriend = true;;
					break;
				}
			}
			feature2.push_back(friendOfFriend);
		}
    }
    return feature2;
}

void addEdge(int u, int v, vector<node> *graph){
    bool nodeExisted = false;
    
    for (unsigned i = 0; i < graph->size(); i++) {
        if (graph->at(i).value == u ) {
            graph->at(i).adj.push_back(v);
            nodeExisted = true;
            break;
        }
    }
    if (!nodeExisted) {
        node nodeElement;
        nodeElement.value = u;
        nodeElement.adj.push_back(v);
        graph->push_back(nodeElement);
    }
    
}
int positionInGraph(int value, vector< node > graph){
    int position = -1;
    for (unsigned i =0; i < graph.size(); i++) {
        if (value == graph.at(i).value) {
            position = i;
        }
    }
    return position;
}

int minDistance(vector<int> dist, vector<bool> sptSet, int nodesSize)
{
    // Initialize min value
    int min = INT_MAX, min_index;
    
    for (int v = 0; v < nodesSize; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
    
    return min_index;
}

int shorthestPath(vector<node> graph, int src, int dest)
{
	// If "src" or "dest" does not exist in the graph, then there is 
	// no path from it, or to it
	int srcPosition, destPosition;
	srcPosition = positionInGraph(src, graph);
	destPosition = positionInGraph(dest, graph);
	if (srcPosition == -1 || destPosition == -1)
		return INT_MAX;
	
	// The output value.  dist will hold the shortest
	// distance from src to all nodes
	vector<int> dist(graph.size());
	
	// sptSet[i] will true if vertex i is included in shortest
	// path tree or shortest distance from src to all nodes
    vector<bool> sptSet(graph.size()); 
        
    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < graph.size(); i++)
        dist[i] = INT_MAX, sptSet[i] = false;
    
    // Distance of source vertex from itself is always 0
    dist[srcPosition] = 0;
    
    // Find shortest path for all vertices
    for (int count = 0; count < graph.size()-1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in first iteration.
        int u = minDistance(dist, sptSet, graph.size());
        
        // Mark the picked vertex as processed
        sptSet[u] = true;
        
        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < graph.at(u).adj.size(); v++)
            
            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph.at(u).adj.at(v) && dist[u] != INT_MAX
                && dist[u] + 1 < dist[v])
                dist[v] = dist[u] + 1;
    }    
    return dist[destPosition];
}

vector<bool> processFeature3(vector<time_t> nodesBatchDates,
                             vector<int> nodesBatchOrigin,
                             vector<int>nodesBatchDest,
                             vector<time_t>nodesStreamDates,
                             vector<int>nodesStreamOrigin,
                             vector<int>nodesStreamDest){
    
    vector<bool> feature3;
    // create a graph
    vector< node > graph;
    
    for (unsigned i =0; i < nodesBatchDates.size(); i++) {
        addEdge(nodesBatchOrigin.at(i), nodesBatchDest.at(i), &graph);
        addEdge(nodesBatchDest.at(i), nodesBatchOrigin.at(i), &graph);
    }
    
    for (unsigned i=0; i <nodesStreamDates.size(); i++) {
        int origin, dest;
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        if (shorthestPath(graph, origin, dest) <= 4) {
            feature3.push_back(true);
        }
        else
            feature3.push_back(false);
    }
    return feature3;
}

int minDistanceOptimized(unordered_map<int, int> dist, unordered_map<int, bool> sptSet)
{
    // Initialize min value
    int min = INT_MAX;
    int min_index = -1;
    for ( auto it = dist.begin(); it != dist.end(); ++it ){
        if (sptSet[it->first] == false && dist[it->first] <= min)
            min = dist[it->first], min_index = it->first;
    }
    return min_index;
}

int shorthestPathOptimized( int src, int dest, unordered_map<int, vector<int>> graph)
{
	// If "src" or "dest" does not exist in the graph, then there is 
	// no path from it, or to it
    unordered_map<int, vector<int>>::const_iterator isThere = graph.find(src);
    if (isThere == graph.end()) {
        return INT_MAX;
    }
    isThere = graph.find(dest);
    if (isThere == graph.end()) {
        return INT_MAX;
    }
	
	// The output value.  dist will hold the shortest
	// distance from src to all nodes
	unordered_map<int, int> dist;
	
	// sptSet[i] will true if vertex i is included in shortest
	// path tree or shortest distance from src to all nodes
	unordered_map<int, bool> sptSet; 
        
    // Initialize all distances as INFINITE and stpSet[] as false
	for ( auto it = graph.begin(); it != graph.end(); ++it ){
		dist[it->first] = INT_MAX;
		sptSet[it->first] = false;
	}	
    
    // Distance of source vertex from itself is always 0
    dist[src] = 0;
    
    // Find shortest path for all vertices
    for (auto& node: graph)
    {
        
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in first iteration.
        int u = minDistanceOptimized(dist, sptSet);
        // Mark the picked vertex as processed
        sptSet[u] = true;
        
        // Update dist value of the adjacent vertices of the picked vertex.
        vector<int> adj = graph[u];
        for (int v = 0; v < adj.size(); v++)
            
            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!sptSet[adj.at(v)] && adj.at(v) && dist[u] != INT_MAX
                && dist[u] + 1 < dist[adj.at(v)])
                dist[adj.at(v)] = dist[u] + 1;
    }
    return dist[dest];
}
vector<bool> processFeature3Optimized(unordered_map<int, vector<int>> hashTableBatch,
                             vector<time_t>nodesStreamDates,
                             vector<int>nodesStreamOrigin,
                             vector<int>nodesStreamDest){
    vector<bool> feature3;
    
//    // convert batch date to hashtable
//	unordered_map<int, vector<int>> hashTableBatch;
//	vector<int> adj;
//	for (unsigned i=0; i<nodesBatchDates.size(); i++){
//		// add from origin to dest
//		adj = hashTableBatch[nodesBatchOrigin.at(i)];
//		adj.push_back(nodesBatchDest.at(i));
//		hashTableBatch[nodesBatchOrigin.at(i)] = adj;
//				
//		// add from dest to origin
//		adj = hashTableBatch[nodesBatchDest.at(i)];
//		adj.push_back(nodesBatchOrigin.at(i));
//		hashTableBatch[nodesBatchDest.at(i)] = adj;		
//	}
    
    //verify hashTableStructure
//    for (auto& x: hashTableBatch) {
//        std::cout << x.first << ": " << endl;
//        vector<int> adj = x.second;
//        for (int i=0; i<adj.size(); i++) {
//            cout << " " << adj.at(i) << " ";
//        }
//        cout << endl;
//    }
    
    for (unsigned i=0; i < nodesStreamDates.size(); i++) {
        int origin, dest;
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        if (shorthestPathOptimized(origin, dest, hashTableBatch) <= 4) {
            feature3.push_back(true);
        }
        else
            feature3.push_back(false);
    }
    return feature3;
}
void writeFile(string pathFile, vector<bool> feature){
    ofstream ofs;
    ofs.open (pathFile, ofstream::out);
    for (int i=0; i<feature.size(); i++) {
        if (feature.at(i)) {
            ofs << "trusted" << endl;
        }
        else
            ofs << "unverified" << endl;
    }
    
    ofs.close();
}

int main(int argC, char** argV){
    
	//read file of input of batch
    unordered_map<int, vector<int>> hashTableBatch;
    vector<time_t> nodesBatchDates;
	vector<int> nodesBatchOrigin;
	vector<int> nodesBatchDest;
    
    if (argV[1]){
        readFile(argV[1], &nodesBatchDates, &nodesBatchOrigin, &nodesBatchDest);
        vector<int> adj;
        for (unsigned i=0; i<nodesBatchDates.size(); i++){
            // add from origin to dest
            adj = hashTableBatch[nodesBatchOrigin.at(i)];
            adj.push_back(nodesBatchDest.at(i));
            hashTableBatch[nodesBatchOrigin.at(i)] = adj;
    
            // add from dest to origin
            adj = hashTableBatch[nodesBatchDest.at(i)];
            adj.push_back(nodesBatchOrigin.at(i));
            hashTableBatch[nodesBatchDest.at(i)] = adj;		
        }
//        // print hash table
//        for (auto& x: hashTableBatch) {
//            std::cout << x.first << ": " << endl;
//            vector<int> adj = x.second;
//            for (int i=0; i<adj.size(); i++) {
//                cout << " " << adj.at(i) << " ";
//            }
//            cout << endl;
//        }
    }
    else{
        cerr << "Error... Argument for batch input file need to be provided as first argument"<< endl;
    }
    
    cout << "Batch elements: "<< hashTableBatch.size() << endl;

    //read files of input stream
    vector<time_t> nodesStreamDates;
    vector<int> nodesStreamOrigin;
    vector<int> nodesStreamDest;
    
    if (argV[2]){
        readFile(argV[2], &nodesStreamDates, &nodesStreamOrigin, &nodesStreamDest);
    }
    else{
        cerr << "Error... Argument for stream input file need to be provided as second argument"<< endl;
    }
    
    // read path for output file feature1
    string pathFeature1;
    if (argV[3]){
        pathFeature1 = argV[3];
    }
    else{
        cerr << "Error... Arguments for output feature1 file need to be provided as third argument"<< endl;
    }
    
    // read path for output file feature1
    string pathFeature2;
    if (argV[4]){
        pathFeature2 = argV[4];
    }
    else{
        cerr << "Error... Arguments for output feature2 file need to be provided as fourth argument"<< endl;
    }
    
    // read path for output file feature1
    string pathFeature3;
    if (argV[5]){
        pathFeature3 = argV[5];
    }
    else{
        cerr << "Error... Arguments for output feature3 file need to be provided as fifth argument"<< endl;
    }
    
    double wallS0, wallS1;
    
    //************** FEATURE 1 OPTIMIZED**************
    {
        wallS0 = getWallTime();
        // process feature1, there was a previous transaction: trusted= true, unverified = false
        vector<bool> feature1Opt = processFeature1Optimized(hashTableBatch,
                                                nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
        wallS1 = getWallTime();
        cout << "Feature1 processed  "<< feature1Opt.size() << " elements, and its job Time: " << (wallS1-wallS0)*1000 <<" ms." << endl;
        //write output files
        writeFile(pathFeature1, feature1Opt);
    }
    //************** FEATURE 2 OPTIMIZED**************
    {
        wallS0 = getWallTime();
        // process feature2, there is an intersection between the friends of origin node and friends of dest node
        vector<bool> feature2Opt = processFeature2Optimized(hashTableBatch,
                                                nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
        wallS1 = getWallTime();
        cout << "Feature2 processed  "<< feature2Opt.size() << " elements, and its job Time: " << (wallS1-wallS0)*1000 <<" ms." << endl;
        //write output files
        writeFile(pathFeature2, feature2Opt);
    }
    //************** FEATURE 3 OPTIMIZED**************
    {
        wallS0 = getWallTime();
        // process feature3, shortest path between origin/dest should be less equal to 4
        vector<bool> feature3Opt = processFeature3Optimized(hashTableBatch,
                                                nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
        wallS1 = getWallTime();
        cout << "Feature3 processed  "<< feature3Opt.size() << " elements, and its job Time: " << (wallS1-wallS0)*1000 <<" ms." << endl;
        //write output files
        writeFile(pathFeature3, feature3Opt);
    }
		
}
