#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

int ind = 0;
mutex m;
vector<thread> threads;
condition_variable start_exec;

void print(string s, int size, int threadIndex)
{
  unique_lock<mutex> lock(m);
  for (int i = ind; i < ind + size; i++)
    cout << s[i % s.size()];
  ind += size;
  cout << " - " << threadIndex << endl;
}

int main()
{
  int n, size;
  string s;
  cin >> n >> size;
  getline(cin, s);
  while (!s.size())
    getline(cin, s);
  for (int i = 0; i < n; i++)
  {
    threads.push_back(thread(print, s, size, i));
    threads[i].join();
  }
  start_exec.notify_all();
}