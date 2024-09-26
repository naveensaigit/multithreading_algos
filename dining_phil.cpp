#include <iostream>

#include <thread>
#include <mutex>
#include <semaphore>

#include <memory>
#include <vector>

using namespace std;

int N = 5;
mutex iomtx;

class CountingSemaphore
{
private:
  int mxval, currval;
  mutex mtx;
  condition_variable cv;

public:
  CountingSemaphore()
  {
    mtx.lock();
    mxval = 1;
    currval = 1;
    mtx.unlock();
  }

  CountingSemaphore(int mx, int initial)
  {
    mtx.lock();
    mxval = mx;
    currval = initial;
    mtx.unlock();
  }

  void acquire()
  {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this]
            { return currval > 0; });
    currval--;
  }

  void release()
  {
    unique_lock<mutex> lock(mtx);
    if (currval < mxval)
      currval++;
    else
    {
      throw "Release called when semaphore was already at max value!";
    }
    cv.notify_all();
  }
};

CountingSemaphore *forks = new CountingSemaphore[5];
counting_semaphore<4> maxDiners{4};

void philosopher(int id)
{
  while (1)
  {
    maxDiners.acquire();
    int left = id, right = (id - 1 + N) % N;
    forks[left].acquire();
    forks[right].acquire();

    iomtx.lock();
    cout << "Philosopher " << id << " started eating" << endl;
    iomtx.unlock();

    this_thread::sleep_for(chrono::seconds(1));

    iomtx.lock();
    cout << "Philosopher " << id << " ended eating" << endl;
    iomtx.unlock();

    forks[right].release();
    forks[left].release();
    maxDiners.release();
  }
}

int main()
{
  vector<thread> threads;

  for (int i = 0; i < N; i++)
  {
    thread t(philosopher, i);
    threads.push_back(std::move(t));
  }

  for (auto &t : threads)
    t.join();

  delete[] forks;
}