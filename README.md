# digital-wallet-sol
Digital Wallet - Paymo Solution

Let's define the constraint of the problem. It is needed the batch input file to have the origin/dest ID and the date. It is not relevant to consider the amount, or if it is positive amount, or the message. Therefore, amount and message are ignored.

Feature1
------
If there was a previous transaction, then trusted. Otherwise, unverified.

Feature2
--------
It is important to notice that this feature does not include feature1. Therefore, every single transaction will be analyze just considering friend-of-a-friend feature

Feature3
--------
We can apply graph structure here, and if finding path smaller than 4 then trusted, otherwise unverified. 
