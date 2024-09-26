#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_set>

using namespace std;

int m_ct = 0, f_ct = 0;
mutex m;
condition_variable m2f, f2m;
unordered_set<string> washroom;

void showWashroom()
{
  for (auto &it : washroom)
    cout << it << " ";
  cout << endl;
}

void enterWashroom(string gender, int id)
{
  washroom.insert(gender + to_string(id));
  showWashroom();
}

void exitWashroom(string gender, int id)
{
  washroom.erase(gender + to_string(id));
  showWashroom();
}

void male(int id)
{
  unique_lock<mutex> lock(m);
  f2m.wait(lock, [&]
           { return !f_ct && m_ct < 3; });
  m_ct++;
  enterWashroom("M", id);
  lock.unlock();
  this_thread::sleep_for(chrono::seconds(1));
  lock.lock();
  m_ct--;
  exitWashroom("M", id);
  lock.unlock();
  m2f.notify_all();
  this_thread::sleep_for(chrono::milliseconds(5));
  f2m.notify_all();
}

void female(int id)
{
  unique_lock<mutex> lock(m);
  m2f.wait(lock, [&]
           { return !m_ct && f_ct < 3; });
  f_ct++;
  enterWashroom("F", id);
  lock.unlock();
  this_thread::sleep_for(chrono::seconds(1));
  lock.lock();
  f_ct--;
  exitWashroom("F", id);
  lock.unlock();
  f2m.notify_all();
  this_thread::sleep_for(chrono::milliseconds(5));
  m2f.notify_all();
}

int main()
{
  string requestSeq = "MFMFMFMF";
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