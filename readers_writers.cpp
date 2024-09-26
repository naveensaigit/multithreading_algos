#include <iostream>
#include <thread>
#include <mutex>
#include <barrier>
#include <semaphore>
#include <vector>
#include <map>
#include <functional>

using namespace std;

int read_ct = 0, write_ct = 0, content = -1;
mutex m;
condition_variable cv;

void read()
{
  unique_lock<mutex> lock(m);
  read_ct++;
  cout << "Reading content: " << content << endl;
  read_ct--;
  lock.unlock();
  cv.notify_all();
}

void write(int i)
{
  unique_lock<mutex> lock(m);
  cv.wait(lock, [&]
          { return !read_ct && !write_ct; });
  write_ct++;
  content = i;
  cout << "Write content: " << content << endl;
  write_ct--;
  lock.unlock();
}

int main()
{
  string requestSeq = "RRWRRWRWWWWRWRWRWWWRWRWWWRRR";
  vector<thread> threads;

  for (int i = 0; i < requestSeq.size(); i++)
  {
    if (requestSeq[i] == 'R')
    {
      thread t(read);
      threads.push_back(std::move(t));
    }
    else
    {
      thread t(write, i);
      threads.push_back(std::move(t));
    }
  }

  for (auto &t : threads)
    t.join();

  cout << endl;
}