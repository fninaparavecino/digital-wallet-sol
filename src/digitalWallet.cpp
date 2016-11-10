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
                     vector<int>nodesBatchDest,
                     vector<time_t>nodesStreamDates,
                     vector<int>nodesStreamOrigin,
                     vector<int>nodesStreamDest){
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
    vector<int> dist(graph.size());     // The output value.  dist will hold the shortest
    // distance from src to all nodes
    
    vector<bool> sptSet(graph.size()); // sptSet[i] will true if vertex i is included in shortest
    // path tree or shortest distance from src to all nodes
    
    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < graph.size(); i++)
        dist[i] = INT_MAX, sptSet[i] = false;
    
    // Distance of source vertex from itself is always 0
    int srcPosition = positionInGraph(src, graph);
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
    
    return dist[positionInGraph(dest, graph)];
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
        if (shorthestPath(graph, origin, dest) <=4) {
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
    vector<time_t> nodesBatchDates;
	vector<int> nodesBatchOrigin;
	vector<int> nodesBatchDest;
    if (argV[1]){
        readFile(argV[1], &nodesBatchDates, &nodesBatchOrigin, &nodesBatchDest);
    }
    else{
        cerr << "Error... Argument for batch input file need to be provided as first argument"<< endl;
    }
    
    cout << "Batch elements: "<< nodesBatchDates.size() << endl;

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
    
    //************** FEATURE 1 **************
    {
        wallS0 = getWallTime();
        // process feature1, there was a previous transaction: trusted= true, unverified = false
        vector<bool> feature1 = processFeature1(nodesBatchDates, nodesBatchOrigin, nodesBatchDest,
                                                nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
        //write output files
        writeFile(pathFeature1, feature1);
        wallS1 = getWallTime();
        cout << "Feature1 processed  "<< feature1.size() << " elements, and its job Time: " << (wallS1-wallS0)*1000 <<" ms." << endl;
    }
    //************** FEATURE 2 **************
    {
        wallS0 = getWallTime();
        // process feature2, there is an intersection between the friends of origin node and friends of dest node
        vector<bool> feature2 = processFeature2(nodesBatchDates, nodesBatchOrigin, nodesBatchDest,
                                                nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
        //write output files
        writeFile(pathFeature2, feature2);
        wallS1 = getWallTime();
        cout << "Feature2 processed  "<< feature2.size() << " elements, and its job Time: " << (wallS1-wallS0)*1000 <<" ms." << endl;
    }
    //************** FEATURE 3 **************
    {
        wallS0 = getWallTime();
        // process feature2, shortest path between origin/dest should be less equal to 4
        vector<bool> feature3 = processFeature3(nodesBatchDates, nodesBatchOrigin, nodesBatchDest,
                                                nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
        //write output files
        writeFile(pathFeature3, feature3);
        wallS1 = getWallTime();
        cout << "Feature3 processed  "<< feature3.size() << " elements, and its job Time: " << (wallS1-wallS0)*1000 <<" ms." << endl;
    }
		
}
