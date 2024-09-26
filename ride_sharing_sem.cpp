#include <iostream>
#include <thread>
#include <mutex>
#include <barrier>
#include <semaphore>
#include <vector>
#include <map>
#include <functional>

using namespace std;

int cab_no = 0, rep = 0, dem = 0;
mutex m;
counting_semaphore<4> r{0}, d{0};
barrier<> b{4};

void drive(int repSeated, int demSeated)
{
  cout << "Cab " << ++cab_no << " started with ";
  for (int i = 0; i < repSeated; i++)
    cout << "R";
  for (int i = 0; i < demSeated; i++)
    cout << "D";
  cout << endl;
}

void republican()
{
  bool rideLeader = false;
  unique_lock<mutex> lock(m);
  rep++;
  int repSeated = 0, demSeated = 0;

  if (rep == 4)
  {
    r.release(3);
    rep -= 4;
    rideLeader = true;
    repSeated = 4;
  }
  else if (rep == 2 && dem == 2)
  {
    r.release();
    d.release(2);
    rep -= 2;
    dem -= 2;
    rideLeader = true;
    repSeated = demSeated = 2;
  }
  else
  {
    lock.unlock();
    r.acquire();
  }

  b.arrive_and_wait();

  if (rideLeader)
  {
    drive(repSeated, demSeated);
    lock.unlock();
  }
}

void democrat()
{
  bool rideLeader = false;
  unique_lock<mutex> lock(m);
  dem++;
  int repSeated = 0, demSeated = 0;

  if (dem == 4)
  {
    d.release(3);
    dem -= 4;
    rideLeader = true;
    demSeated = 4;
  }
  else if (dem == 2 && rep == 2)
  {
    d.release();
    r.release(2);
    dem -= 2;
    rep -= 2;
    rideLeader = true;
    repSeated = demSeated = 2;
  }
  else
  {
    lock.unlock();
    d.acquire();
  }

  b.arrive_and_wait();

  if (rideLeader)
  {
    drive(repSeated, demSeated);
    lock.unlock();
  }
}

int main()
{
  string requestSeq;
  getline(cin, requestSeq);
  while (!requestSeq.size())
  {
    getline(cin, requestSeq);
  }
  vector<thread> threads;

  for (int i = 0; i < requestSeq.size(); i++)
  {
    thread t(requestSeq[i] == 'R' ? republican : democrat);
    threads.push_back(move(t));
  }

  for (auto &t : threads)
    t.join();

  cout << endl;
}