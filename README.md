# digital-wallet-sol
Digital Wallet - Paymo Solution

Let's define the constraint of the problem. It is needed the batch input file to have the origin/dest ID and the date. It is not relevant to consider the amount, or if it is positive amount, or the message. Therefore, amount and message are ignored.

The solution has been implemented using cpp. You can compile the file using the Makefile in /src folder. You will need g++ 5.0 or bigger for compilation process. 

For each element in paymo_stream.txt file, we select the "origin" input Id, and the "dest" input id.

Feature1
------
If there was a previous transaction, then trusted. Otherwise, unverified.

Feature2
--------
If there was a previous transaction, then trusted. Otherwise, check the friends of origin, and the frinds of dest, and check intersection between the two set of friends. If there is intersection, then trusted. Otherwise, unverified.

Feature3
--------
We can apply graph structure here, and using Disjktra algorithm, check if the shortest path between "origin" and "dest" is less than equal to four, then trusted. Otherwise, unverified.
