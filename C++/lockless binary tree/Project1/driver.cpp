#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <cstdlib>
#include "thread_id.h"
#include "LockFreeBST.h"
#include "FineGrainedBST.h"
#include "debug-print.h"

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


//for debug print
std::mutex print_lock;
static std::atomic<unsigned long long> gModifications{0};

//non threaded tests
#include "NormalTests.cpp"

template<typename Cont, typename T>
void InsertItem(Cont& tree, T value, std::ostream& os, bool printTree = true, bool printresult = true)
{
  bool r = tree.insert(value);

  if (!printresult)
    return;

  print_lock.lock();
  //os << "Thread "<< Thread::ID << " Inserting value: " << value;
  os << "Thread "<< Thread::ID << " Inserting value: " << value;
  std::string result = r ? " Success" : " Failure";
  os << "              Result: " << result << std::endl;

  if (printTree)
    tree.Print(os);

  print_lock.unlock();
}

template<typename Cont, typename T>
void RemoveItem(Cont& tree, T value, std::ostream& os, bool printTree = true, bool printresult = true)
{
  bool r = tree.remove(value);

  if (!printresult)
    return;

  print_lock.lock();
  os << "Thread " << Thread::ID << " Removing value: " << value;
  std::string result = r ? " Success" : " Failure";
  os << "              Result: " << result << std::endl;
  
  if(printTree)
    tree.Print(os);

  print_lock.unlock();
}

template<typename Cont, typename T>
void FindItem(Cont& tree, T value, std::ostream& os, bool expected)
{
  bool r = tree.find(value);

  //assert if not expected result
  assert(r == expected);
}

enum OpType
{
  Insert = 0,
  Remove,
  Find
};


template<typename Cont, typename T>
void ChooseOperation(Cont& tree, const std::vector<T>& vec, std::ostream& os, OpType type, bool expected = true)
{
  size_t n = vec.size();
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> index_dist(0, n - 1);
  std::uniform_int_distribution<int> insert_or_remove(0, 1);

  int tid = GetThreadID();
  for (size_t i = 0; i < n; ++i)
  {
    int index = index_dist(gen);
    if (type == Insert)
      InsertItem(tree, vec[index], os, false, false);
    else if (type == Remove)
      RemoveItem(tree, vec[index], os, false, false);
    else if (type == Find)
      FindItem(tree, vec[index], os, expected);
  }
}

template <typename Tree, typename T>
void TestAll(int num_threads, int size)
{
  Debug debug;
  Tree tree;
  std::vector<int> v_int;
  v_int.reserve(size);
  for (int i = 0; i < size; ++i)
    v_int.push_back(i);

  //Insert 
  std::vector<std::thread> threads(num_threads);
  for (auto & thd : threads)
  {
    thd = std::thread{ &ChooseOperation<Tree, T>, std::ref(tree),
      std::ref(v_int), std::ref(debug.oss), Insert, true };
  }
  for (auto & thd : threads)
    thd.join();

  threads.clear();
  GetThreadID(true);

  //Find (should return all true)
  threads.resize(num_threads);
  for (auto & thd : threads)
  {
    thd = std::thread{ &ChooseOperation<Tree, T>, std::ref(tree),
      std::ref(v_int), std::ref(debug.oss), Find, true };
  }
  for (auto & thd : threads)
    thd.join();


  threads.clear();
  GetThreadID(true);

  //Remove
  threads.resize(num_threads);
  for (auto & thd : threads)
  {
    thd = std::thread{ &ChooseOperation<Tree, T>, std::ref(tree),
      std::ref(v_int), std::ref(debug.oss), Remove, true };
  }
  for (auto & thd : threads)
    thd.join();

  threads.clear();
  GetThreadID(true);

  //Find (should return all false)
  threads.resize(num_threads);
  for (auto & thd : threads)
  {
    thd = std::thread{ &ChooseOperation<Tree, T>, std::ref(tree),
      std::ref(v_int), std::ref(debug.oss), Find, false };
  }

  for (auto & thd : threads)
    thd.join();

#if 1
  //print tree
  tree.Print(debug.oss);
#endif

  debug.flush();
}

template<typename Cont, typename T>
void Insert_Or_Remove(Cont& tree, const std::vector<T>& vec, std::ostream& os, unsigned num)
{
  int n = static_cast<int>(vec.size());
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> index_dist(0, n - 1);
  std::uniform_int_distribution<int> insert_or_remove(0, 1);
  
  for (int i = 0; i < n; ++i)
  {
    int index = index_dist(gen);
    bool insert = insert_or_remove(gen) != 0;
    if (insert)
      InsertItem(tree, vec[index], os, false, false);
    else
      RemoveItem(tree, vec[index], os, false, false);
    gModifications.fetch_add(1);
  }
}

#define NUM_SECONDS 10

template<typename Time, typename Tree, typename T>
void time_loop(Time time_point, Tree& tree, const std::vector<T>& vec, std::ostream& os, unsigned n)
{
  GetThreadID();
  while (std::chrono::system_clock::now() < time_point)
  {
    Insert_Or_Remove<Tree, T>(tree, vec, os, n);
    //std::this_thread::sleep_for(std::chrono::nanoseconds{10});
  }
}

//dont use odd number of threads
template<typename Tree, typename T>
void PerformanceTest(int num_threads, int size)
{
  //init
  Tree tree;
  Debug debug;
  std::vector<std::thread> threads(num_threads);
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 random_engine(static_cast<unsigned>(seed));

  //prepare lists
  std::vector<int> v_int;
  v_int.reserve(size);
  for (int i = 0; i < size; ++i)
    v_int.push_back(i);

  std::vector<std::vector<int>> lists;
  lists.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i)
  {
    std::vector<int> copy = v_int;
    std::shuffle(copy.begin(), copy.end(), random_engine);
    lists.push_back(copy);
  }

  //record how long it takes for threads to run
  auto start = std::chrono::system_clock::now();
  auto end = start + std::chrono::seconds{ NUM_SECONDS };
  debug.oss << "Simulating " << num_threads << " threads " << "for " << NUM_SECONDS << " seconds" << std::endl;

  for (int i = 0; i < num_threads; ++i)
  {
    threads[i] = std::thread{&time_loop<decltype(end), Tree, T>, end, std::ref(tree), std::ref(lists[i]), std::ref(debug.oss), num_threads};
  }

  debug.oss << "Time's up. waiting for thread" << std::endl;
  for (auto & thd : threads)
    thd.join();

  debug.oss << "done" << std::endl;
  debug.oss << gModifications.load() << " moves made" << std::endl;

  //tree.CheckSize(debug.oss);

  //std::cout << debug.oss.str();
  debug.flush();
}

void CheckMemLeaks()
{
  _CrtDumpMemoryLeaks();
}


#define LIST_SIZE 100000

int main(int argc, char**argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <num_of_threads> \n";
    return -1;
  }

  //check mem leaks
  std::atexit(CheckMemLeaks);
  //_CrtSetBreakAlloc(209);

  int num = std::atoi(argv[1]);
  //TestAll<FineGrainedBST<int>, int>(num, LIST_SIZE);
  //TestAll<LockFreeBST<int>, int>(num, LIST_SIZE);
  //PerformanceTest<FineGrainedBST<int>, int>(num, LIST_SIZE);
  PerformanceTest<LockFreeBST<int>, int>(num, LIST_SIZE);
}