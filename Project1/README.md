
se of this series of assignments is to learn how parallel programming can be used to take advantage of multicore processing resources, and how to evaluate performance.

The basic problem for this assignment is to compute a total sum of a 2D NxN array (of floats), and also compute row sums for the array.

1. Use pthreads to create a threaded C/C++ program that performs the required computations. The main program thread (automatically created when you run the program) should not perform any array summation computations, only computation threads should, which are all created using pthreads.

2. Your program should accept the command line argument "-t #" which is T, the number of computation threads to use. T should default to 1 if no argument is given.

3. Your program should accept the command line argument "-s #" which is N, the size of the array. The array is an NxN 2D array. N defaults to 100.

4. Your program should accept the command line argument "-v #" which is V, a float value that every array element should be initialized to. This will give you an easy mechanism to test your computations. If not provided, the array should be initialized to random float values between 0 and 100.

5. Each computation thread must compute the sum for a sub-portion of the array which is "as square as possible". Let C and R be the most alike integer factors of T, where R >= C and T=C*R. With this, C is the number of column partitions, and R is the number of row partitions. Example 1: if N=60 and T=6, C=2 and R=3, so each thread would sum an array with 60/2=30 columns and 60/3=20 rows. Example 2: if N=70 and T=7 (a prime), then C=1 and R=7, so each thread would sum an array of 70 columns and 10 rows. With R>=C the preference here is to keep row data grouped larger. You must verify that N is divisible by both C and R with no remainder, and output an error message and quit if it is not.

6. The main thread must dynamically allocate at least three arrays: the NxN data array, a 1D N-element array of row sums, and a 1D N-element array of thread timings. The main thread will also need a variable for the total array sum, and possibly other variables accessible to all threads (like mutexes). Each computation thread must add its own partial array sum and partial row sum into the appropriate summation variables. Locking must be used to ensure correct summation updates.

7. Each thread must time its own computation using clock_gettime() and enter this time into the appropriate timing data element. Thread timings should be measured using the CLOCK_THREAD_CPUTIME_ID clock.

8. The main thread must sum the row sums and make sure they match the overall array sum, and it must print out the overall array sum and the thread timing information. The main thread should also time the entire application (starting after array initialization) using "wall clock time", which is the CLOCK_REALTIME clock of clock_gettime() and print this out as well.

9. For this part, once you have a working program, run your program on an array size of N=4000 for 1, 2, 4, and 8 threads, and include in your submission the outputs for all of these runs, in one text file.

10. You must include in your submission a Makefile that correctly builds your program on CS department grad/ugrad lab machines. Your submission must only include source files, the Makefile, and the text from (9); do not include any object code files or executable code files, or library source code that is not your own. Your program must not rely on any 3rd-party libraries that are not available on CS department machines.
