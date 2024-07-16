# C client implementation of multiple socket port 

## The Problem scope:
Handling 3 sockets with 3 different ports at one client to collect common data from each other is one part of the problem. Handled in "client1.c" And the second part of the problem is to update data to 4 control socket according to data incoming from a port. Handled in "client2.c"

## Solution:

For handling 3 sockets concurrently 3 threads need to be opened and each of them waits for data to be served from servers. Each socket update time could be different while reading. A structure need to be defined for common data to be collected. For each thread, a thread data need to created from this structure. Thread will produce data from server and after this data consumed in the main loop data output need to emptied with "--" again to be filled in with served data.


A main while loop at 100 ms in client1 and 20ms in client2 gathers and consumes these 3 data structure and print stdout and empty it.

While in main loop the data collected could be analyzed and feedback control could be sent to server via control port

This is the first idea as a solution in my mind. I know that it works but i would like to go to fundamentals and go back and read again "Unix Network Programming" to refresh my knowledge and check if i can find any simpler solution. There is another solution with "select"  or "poll" but they both could be more complicated and not covers further problems could be occur such as if one of ports the frequency could change or the data could be received at the same time so can be missed. 
## The properties of server control protocol with default values are:

1.14 enable=1

1.170 amplitude=5000

1.255 frequency=500

found with a small python script testing properties via connecting and sending data through nc command 

## Analysis of outputs:

The shape of output1 is alike to sinusoidal vawe and output 2 is more like triangle vawe hovewer output3 is square vawe. 
We can get the frequency of output1 and output2 from control port. 
The output1 at first 2Hz and amplitude is 5.0
The output2 at first 4Hz and amplitude is 5.0

## Building clients

```
make all 
```
command builds both clients 

```
make client1
make client2
``` 

commands could be used to build each seperately

no external library used other than standart libraries


