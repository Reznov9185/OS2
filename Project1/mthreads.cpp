#include <iostream>
#include <thread>
#include <string.h>
#include <random>
#include <stack>
#include <pthread.h>
#include <time.h>

using namespace std;

int T = 1;        // No of threads (defaults to 1)
int N = 100;      // Dimention N of the NxN 2D array (defaults to 100)
double V = 0.0;    // Value of 2D array element (defaults to be assigned)
double **my_array; // 2D array pointer
int R, C;         // Declare factors for num. of thread
double arr_sum;   // n^2 array sum
double *sum;       // Declare 1D sum array pointer
double *timing;    // Declare 1D array pointer for thread timings
pthread_mutex_t lock;
pthread_t *thread_id;
int step = 0;
struct timespec start_time, end_time; // For thread timing
struct timespec start_time_comp, end_time_comp; // For whole program timing 
uint64_t diff; // For thread timing
uint64_t process_diff; // For n^2 timing
#define BILLION 1000000000L // For timing calc

int find_one_square_like_factor(int number)
{
   stack<int> stack;
   for (int i = 1; i <= number; ++i)
   {
      if (number % i == 0)
      {
         stack.push(i);
      }
   }
   for (int i = 0; i < stack.size() / 2; i++)
   {
      stack.pop();
   }
   return stack.top();
}

int array_sum()
{
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
   for (int i = 0; i < N; i++)
   {
      for (int j = 0; j < N; j++)
      {
         arr_sum += my_array[i][j];
      }
   }
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
   process_diff = BILLION * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
   return 0;
}

void *thread_array_sum(void *arg)
{
   pthread_mutex_lock(&lock);
   clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_time);
   // Each thread computes sum of R, C of 2D array
   int thread_part = step++;
   for (int i = ((thread_part/C) * (N / R)); i < (((thread_part)/C) + 1) * (N / R); i++)
   {
      for(int j=((thread_part/R) * (N / C)); j < (((thread_part)/R) + 1) * (N / C); j++){
         sum[thread_part] += my_array[i][j];
      }
   }
   clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end_time);
   diff = BILLION * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
   timing[thread_part] = diff;
   pthread_mutex_unlock(&lock);
   return NULL;
}

int call_threads()
{
   for (int i = 0; i < T; i++)
   {
      pthread_create(&thread_id[i], NULL, &thread_array_sum, &i);
   }
   return 0;
}

int main(int argc, char **argv)
{
   for (int i = 0; i < argc; i++)
   {
      if (strcmp(argv[i], "-t") == 0 && i + 1 < argc && atoi(argv[i + 1]) > 0)
      {
         T = atoi(argv[i + 1]);
         i++;
      }
      else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc && atoi(argv[i + 1]) > 0)
      {
         N = atoi(argv[i + 1]);
         i++;
      }
      else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc && atoi(argv[i + 1]) > 0)
      {
         V = atof(argv[i + 1]);
         i++;
      }
   }

   // Declare 2D array with size NxN
   my_array = new double *[N];
   for (int i = 0; i < N; i++)
   {
      my_array[i] = new double[N];
   }

   for (int i = 0; i < N; i++)
   {
      for (int j = 0; j < N; j++)
      {
         if (V == 0.0)
         {
            my_array[i][j] = rand() / (RAND_MAX + 1.);
         }
         else
         {
            my_array[i][j] = V;
         }
      }
   }
   // Declare 1D sum array of size N
   sum = new double[T];
   // Declare 1D n^2 array sum
   arr_sum = 0;
   // Declare 1D array for thread timings of size N
   timing = new double[T];


   //All computation starts
   clock_gettime(CLOCK_REALTIME, &start_time_comp);

   // Init factors for num. of thread
   R = find_one_square_like_factor(T);
   C = T / R;
   if (R < C)
   {
      swap(R, C);
   }
   // Init thread entity
   thread_id = new pthread_t[T];

   // Logs
   cout << fixed;
   cout << "T: " << T << "\n";
   cout << "N: " << N << "\n";
   cout << "V: " << V << "\n";
   cout << "R: " << R << ", C: " << C << "\n";
   call_threads();
   for (int i = 0; i < T; i++)
   {
      pthread_join(thread_id[i], NULL);
   }
   pthread_mutex_destroy(&lock);

   // Adding sum of all R,C parts
   int total_sum = 0;
   for (int i = 0; i < T; i++)
   {
      total_sum += sum[i];
   }
   cout << "Multithreaded Array Sum is: " << total_sum << endl;
   array_sum();
   cout << "Array sum: " << arr_sum << "\n";

   if(arr_sum == total_sum){
      cout << "Two sums are equal! Validated." << endl;
   }
   
   // Timing for threads
   for (int i = 0; i < T; i++)
   {
      cout << "Timing for thread["<< i << "]: (in nanosec.) " << timing[i] << "\n";
   }
   // Timing for n^2 process
   cout << "Timing for the n^2 loop time: (in nanosec.) " << process_diff << "\n";

   //All computation ends
   clock_gettime(CLOCK_REALTIME, &end_time_comp);
   cout << "Total program computation time: " << BILLION * (end_time_comp.tv_sec - start_time_comp.tv_sec) + end_time_comp.tv_nsec - start_time_comp.tv_nsec << endl;
   return 0;
}
