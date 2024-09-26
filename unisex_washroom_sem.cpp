#include <iostream>

#include <thread>
#include <mutex>
#include <semaphore>

#include <vector>
#include <unordered_set>

using namespace std;

mutex m;
counting_semaphore<3> m_sem{0}, f_sem{3};
unordered_set<string> washroom;

void showWashroom()
{
  for (auto &it : washroom)
    cout << it << " ";
  cout << endl;
}

void enterWashroom(string gender, int id)
{
  unique_lock<mutex> lock(m);
  washroom.insert(gender + to_string(id));
  showWashroom();
}

void exitWashroom(string gender, int id)
{
  unique_lock<mutex> lock(m);
  washroom.erase(gender + to_string(id));
  showWashroom();
}

void male(int id)
{
  m_sem.acquire();
  enterWashroom("M", id);
  this_thread::sleep_for(chrono::seconds(1));
  exitWashroom("M", id);
  if (washroom.size())
    m_sem.release();
  else
  {
    f_sem.release(3);
    this_thread::sleep_for(chrono::milliseconds(10));
    m_sem.release(3);
  }
}

void female(int id)
{
  f_sem.acquire();
  enterWashroom("F", id);
  this_thread::sleep_for(chrono::seconds(1));
  exitWashroom("F", id);
  if (washroom.size())
    f_sem.release();
  else
  {
    m_sem.release(3);
    this_thread::sleep_for(chrono::milliseconds(10));
    f_sem.release(3);
  }
}

int main()
{
  string requestSeq = "MFMFMFMFMFMF";
  vector<thread> threads;

  for (int i = 0; i < requestSeq.size(); i++)
  {
    thread t(requestSeq[i] == 'M' ? male : female, i);
    threads.push_back(std::move(t));
  }

  for (auto &t : threads)
    t.join();

  cout << endl;
}