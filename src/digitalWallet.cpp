#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;


time_t convertStringToDateTime(string dateTime)
{
    // Let's consider we are getting all the input in
    // this format: '2014-07-25T20:17:22Z' (T denotes
    // start of Time part, Z denotes UTC zone).
    // A better approach would be to pass in the format as well.
    wstring wsTmp(dateTime.begin(), dateTime.end());
    
    wstring &ws = wsTmp;
    
    const wstring dateTimeFormat(L"%Y-%m-%d %H:%M:%S");
    
    // Create a stream which we will use to parse the string,
    // which we provide to constructor of stream to fill the buffer.
    wistringstream dateStream(ws);
    
    // Create a tm object to store the parsed date and time.
    tm dateTimeInput;
    
    // Now we read from buffer using get_time manipulator
    // and formatting the input appropriately.
    dateStream >> get_time(&dateTimeInput, dateTimeFormat.c_str());
    
    // Convert the tm structure to time_t value and return.
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
        
        //Get Date
        if (!getline( lineStream, field, ',' ))
            break;
        if (field.compare("time")!=0) {
            //cout << field << endl;
            dates->push_back( convertStringToDateTime(field) );
            
            //Get Origin Node
            if (!getline( lineStream, field, ',' ))
                break;
            origin->push_back( stoi(field) );
            
            //Get Dest Node
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
    // seach for the origin -> dest
    for (int i=0; i< nodesBatchDates.size(); i++) {
        if(((nodesBatchOrigin.at(i) == origin) && (nodesBatchDest.at(i) == dest) && (date > nodesBatchDates.at(i))) ||
           ((nodesBatchOrigin.at(i) == dest) && (nodesBatchDest.at(i) == origin) && (date > nodesBatchDates.at(i))))
        {
            cout << "trusted" << endl;
            trusted = true;
        }
    }
    return trusted;
    
}
bool isThereCommonFriends( vector<int> friendsA, vector<int> friendsB){
    bool trusted = false;
    
    // seach for intersection
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
    cout << "Elements to process Features 1: "<< nodesStreamDates.size() << endl;
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
    cout << "Elements to process Features 2: "<< nodesStreamDates.size() << endl;
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

vector<bool> processFeature3(vector<time_t> nodesBatchDates,
                             vector<int> nodesBatchOrigin,
                             vector<int>nodesBatchDest,
                             vector<time_t>nodesStreamDates,
                             vector<int>nodesStreamOrigin,
                             vector<int>nodesStreamDest){
    cout << "Elements to process Features 3: "<< nodesStreamDates.size() << endl;
    vector<bool> feature3;
    int origin, dest;
    time_t date;
    for (int i = 0; i < nodesStreamDates.size(); i++) {
        origin = nodesStreamOrigin.at(i);
        dest = nodesStreamDest.at(i);
        date = nodesStreamDates.at(i);
        
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
    
    cout << "Elements stored in vectors "<< nodesBatchDates.size() << endl;

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
    
    // process feature1, there was a previous transaction: trusted= true, unverified = false
    vector<bool> feature1 = processFeature1(nodesBatchDates, nodesBatchOrigin, nodesBatchDest,
                                            nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
    
    //write output files
    writeFile(pathFeature1, feature1);
    
    // process feature2, there is an intersection between the friends of origin node and friends of dest node
    vector<bool> feature2 = processFeature2(nodesBatchDates, nodesBatchOrigin, nodesBatchDest,
                                            nodesStreamDates, nodesStreamOrigin, nodesStreamDest);
    
    //write output files
    writeFile(pathFeature2, feature2);
		
}