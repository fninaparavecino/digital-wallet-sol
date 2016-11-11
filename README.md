# digital-wallet-sol
Digital Wallet - Paymo Solution

Let's define the constraints of the problem. It is needed the batch input file to have the origin/dest ID. Although, I have consider the date for comparison purpose, for optimized version date has been ignored. Also, it is not relevant to consider the amount, or if it is positive amount, or the message. Therefore, amount and message are ignored too.

The solution has been implemented using C/C++. You can compile the file using the Makefile in /src folder. You will need g++ 5.0 or higher for compilation process. This test has been tested in Linux Ubuntu 14, and Mac OS X Sierra 10.12.1

For each element in paymo_stream.txt file, we select the "origin" input Id, and the "dest" input id.

Feature1
------
If there was a previous transaction, then trusted. Otherwise, unverified. There are two version for this feature, unoptimized and optimized. The optimized version is set by default since it provides better runtime execution. The unoptimized uses vector as structure to traverse stream data and batch date. The optimized version uses hashTable leveraging unordered_map STL structure from C++, it is highly optimized to run millions of data points. Testing 3M data points in a batch of 4M, it takes less than 2 seconds in a low_end CPU.

Feature2
--------
If there was a previous transaction, then trusted. Otherwise, check the friends of origin, and the friends of dest, and check intersection between the two sets of friends. If there is intersection, then trusted. Otherwise, unverified.

There are also two versions of this feature. Unoptimized uses vector structure. And optimized version uses hashtable leveraging unordered_map<int , vector<int>>, where int is the key, and vector<int> are the transactions performed.

Feature3
--------
We can apply graph structure here, and using Dikjstra algorithm, check if the shortest path between "origin" and "dest" is less than equal to four, then trusted. Otherwise, unverified. There are two versions of this features as well. The unoptimized uses graph structure leveraging vector<node>, where node is struct defined by "value" (key), and vector<int> which represents adj list of edges from "value" to other nodes. I created batch graph using this approach, and implemente Dikjstra algorithm. The time performance is not so good, that is why I decided to move to hashtable leveraging STL unordered_map<int, vector<int>>, where int reprensets the key value, and vector<int> the adj list of edges from key value to other nodes.
