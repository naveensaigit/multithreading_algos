#include <iostream>
#include <thread>
#include <mutex>
#include <deque>

using namespace std;

deque<int> q;
mutex m;
condition_variable cond;
int MAX_BUFFER_SIZE = 50, MAX_PRODUCE = 500;

void producer()
{
  int count = 0;
  while (count < MAX_PRODUCE)
  {
    unique_lock<mutex> lock(m);
    cond.wait(lock, []()
              { return q.size() < MAX_BUFFER_SIZE; });
    q.push_back(++count);
    cout << "Produced: " << count << endl;
    lock.unlock();
    cond.notify_one();
  }
  cout << "Finished with producing!" << endl;
}

void consumer()
{
  int val = -1;
  while (val < MAX_PRODUCE)
  {
    unique_lock<mutex> lock(m);
    cond.wait(lock, []()
              { return !q.empty(); });
    val = q.front();
    q.pop_front();
    cout << "Consumed: " << val << endl;
    lock.unlock();
    cond.notify_one();
  }
  cout << "Finished with consuming!" << endl;
}

int main()
{
  thread t1(producer);
  thread t2(consumer);

  t1.join();
  t2.join();
}