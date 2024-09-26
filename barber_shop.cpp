#include <iostream>

#include <thread>
#include <mutex>
#include <semaphore>

#include <memory>
#include <vector>

using namespace std;

vector<int> customers;
mutex barber_mtx, iomtx;
condition_variable cv;
counting_semaphore<10> seats{10};
binary_semaphore customer_ready{0}, barber_finished{0};

void barber()
{
  while (1)
  {
    customer_ready.acquire();

    iomtx.lock();
    cout << "Barber started with haircut for Customer " << customers.back() << endl;
    iomtx.unlock();

    this_thread::sleep_for(chrono::seconds(1));

    iomtx.lock();
    cout << "Barber finished with haircut for Customer " << customers.back() << endl;
    iomtx.unlock();

    barber_finished.release();
  }
}

void customer(int id)
{
  iomtx.lock();
  cout << "Customer " << id << " entered the shop" << endl;
  iomtx.unlock();

  if (seats.try_acquire())
  {
    iomtx.lock();
    cout << "Customer " << id << " waiting inside shop" << endl;
    iomtx.unlock();

    unique_lock<mutex> lock(barber_mtx);
    customers.push_back(id);
    customer_ready.release();
    barber_finished.acquire();
    seats.release();
  }
  else
  {
    iomtx.lock();
    cout << "Customer " << id << " left because seats are full" << endl;
    iomtx.unlock();
  }
}

int main()
{
  vector<thread> threads;

  thread barb(barber);

  for (int i = 0; i < 100; i++)
  {
    thread t(customer, i);
    threads.push_back(std::move(t));
    this_thread::sleep_for(chrono::milliseconds(300));
  }

  for (auto &t : threads)
    t.join();

  cout << "Customers who got their hair cut done:" << endl;
  for (auto customer : customers)
    cout << customer << " ";
  cout << endl;

  exit(0);
}