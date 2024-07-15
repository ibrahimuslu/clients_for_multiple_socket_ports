# C client implementation of multiple socket port 

## Problem scope

Handling 3 sockets at once to collect common data from each other if they serve 

## Solution 

For handling 3 sockets concurrently 3 threads need to be opened and each of them waits for data to be served from servers. A structure need to be defined for common data to be collected. For each thread, a thread data need to created from this structure and produce data by served data and after this data consumed the data structure need to emptied again to be filled in.

A main while loop at 100 ms gathers and consumes these 3 data structure and print stdout and empty it.

While in main loop the data collected could be analyzed and feedback control could be sent to server via control port

# The properties of server with default values are:

1.14 enable=1

1.170 amplitude=5000

1.255 frequency=500


found with a small python script testing properties via connecting and sending data through nc command 

