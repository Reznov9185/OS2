#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <thread>
#include <string.h>
#include <random>
#include <stack>
#include <pthread.h>
#include <time.h>
#include <mutex>

using namespace std;

// Required global variables
string filename;
queue<string> traffic;
int queue_size = 0;
pthread_mutex_t m_lock;
pthread_t *thread_id;

class HauntingHouse
{
public:
   HauntingHouse() : name("") {}

   HauntingHouse(string n, mutex *left, mutex *right) : name(n), leftDoor(left), rightDoor(right) {}

   void enter()
   {
      doAction("enter");
   }

   void exit()
   {
      if (leftDoor && rightDoor)
      {
         // create unique_locks, but don't lock yet
         unique_lock<mutex> left(*leftDoor, defer_lock);
         unique_lock<mutex> right(*rightDoor, defer_lock);

         // lock both unique_locks (without a deadlock)
         lock(left, right);

         doAction("exit");

         // unclock
         left.unlock();
         right.unlock();
      }
   }

   void doAction(string actionName)
   {
      // report start
      pthread_mutex_lock(&m_lock);
      cout << name << " request to cross house: " << actionName << endl;
      pthread_mutex_unlock(&m_lock);

      // do action
      pthread_mutex_lock(&m_lock);
      this_thread::sleep_for(chrono::milliseconds(250));
      cout << name << " waiting" << endl;
      pthread_mutex_unlock(&m_lock);

      // report finish
      pthread_mutex_lock(&m_lock);
      cout << name << " finished action: " << actionName << endl;
      pthread_mutex_unlock(&m_lock);
   }

   // run actions
   void run()
   {
      if (leftDoor && rightDoor)
      {
         for (int i = 0; i < 3; i++)
         {
            enter();
            exit();
         }
      }
   }

private:
   string name;
   mutex *leftDoor;
   mutex *rightDoor;
};

void printq(queue<string> data)
{
   queue<string> q = data;
   while (!q.empty())
   {
      cout << q.front() << ' ';
      q.pop();
   }
   cout << '\n';
}

int main(int argc, char **argv)
{
   for (int i = 0; i < argc; i++)
   {
      if (i == 1)
         filename = argv[1];
   }

   ifstream file(filename);
   string data = "";
   while (getline(file, data, ','))
   {
      if (!data.empty())
         traffic.push(data);
   }
   file.close();

   int max_no_of_visitors = traffic.size();

   // create forks
   vector<shared_ptr<mutex>> forks;
   for (int i = 0; i < max_no_of_visitors; i++)
   {
      forks.push_back(make_shared<mutex>());
   }

   // create haunting house
   vector<shared_ptr<HauntingHouse>> haunted_houses;
   for (int i = 0; i < max_no_of_visitors; i++)
   {
      string name = "Visitor " + to_string(i) + ": ";
      traffic.pop();
      haunted_houses.push_back(
          make_shared<HauntingHouse>(
              name,
              forks[i].get(),
              forks[(i + 1) % max_no_of_visitors].get()));
   }

   vector<thread> th;
   for (int i = 0; i < max_no_of_visitors; ++i)
   {
      th.push_back(thread(&HauntingHouse::run, haunted_houses[i]));
   }

   for (auto &t : th)
   {
      t.join();
   }
   pthread_mutex_destroy(&m_lock);

   return 0;
}
