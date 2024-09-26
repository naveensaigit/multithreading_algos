#include <iostream>
#include <thread>
#include <mutex>
#include <barrier>
#include <semaphore>
#include <vector>
#include <map>
#include <functional>

using namespace std;

string seating = "";
int cab_no = 0, rep = 0, dem = 0;
mutex m;
condition_variable cv;

void seated(char ch)
{
  cout << "Seated " << ch << endl;
  seating += ch;
}

void drive()
{
  cout << "Cab " << ++cab_no << " started with " << seating << endl;
}

void republican()
{
  unique_lock<mutex> lock(m);
  cv.wait(lock, [&]
          { return dem == 0 && rep < 2; });
  seated('R');
  rep++;
  if (rep == 2)
  {
    rep = 0;
  }
  if (seating.size() == 4)
  {
    drive();
    seating = "";
  }
  lock.unlock();
  cv.notify_all();
}

void democrat()
{
  unique_lock<mutex> lock(m);
  cv.wait(lock, [&]
          { return rep == 0 && dem < 2; });
  seated('D');
  dem++;
  if (dem == 2)
  {
    dem = 0;
  }
  if (seating.size() == 4)
  {
    drive();
    seating = "";
  }
  lock.unlock();
  cv.notify_all();
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