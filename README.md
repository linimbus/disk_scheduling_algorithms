# Disk Scheduling Algorithms

## Description

FCFS: Schedules requests in the order they arrive.
SSTF: Selects the request with the shortest seek time from the current head position.
CSCAN:
1. First, it moves the head from the inner to the outer track. After reaching the outermost track, it returns to the innermost track and continues scanning.
2. The head changes direction only when there are no more outer tracks to access.
3. The next request to be serviced is the one closest to the current position in the current direction.

LOOK: LOOK and C-LOOK are improved versions of SCAN and C-SCAN. In SCAN and C-SCAN, the head moves to the end of the disk, but in LOOK and C-LOOK, the head only moves as far as the last request in the current direction, avoiding unnecessary movement.

Write a C program to implement FCFS, SSTF, CSCAN, LOOK disk scheduling algorithms.
The first line of stdin input should look like as follows:

```
gcc -o program program.c
./program FCFS 34 do
```

Here the user enters the algorithm they want to use along with the starting position of the disk head and the direction order for movement (do for descending and ao for ascending).
The code should service a disk with 10000 sectors numbered from 0 to 9999.
Next, the user enters sector requests on stdin like: 250 10
Here, 250 indicates the sector to be read, and 10 is the request arrival time. It should start at time 0 and serve requests after they arrive. The formula to calculate the time required to process a request is equal to the distance that the head has to move divided by 10 plus 5 if the disk movement required changing direction.
Note: The solution does not change the behavior because of new requests that come after the decision was made.
Output: total time required to service all requests and total head movement required.
