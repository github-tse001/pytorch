#include <ATen/ATen.h>
#include <ATen/Parallel.h>
#include <ATen/test/test_assert.h>
#include <thread>


// This checks whether threads can see the global
// numbers of threads set and also whether the scheduler
// will throw an exception when multiple threads call
// their first parallel construct.
void test(int given_num_threads) {
  auto t = at::ones({1000 * 1000}, at::CPU(at::kFloat));
  if (given_num_threads >= 0) {
    ASSERT(at::get_num_threads() == given_num_threads);
  } else {
    ASSERT(at::get_num_threads() == -1);
  }
  auto t_sum = t.sum();
  for (int i = 0; i < 1000; i ++) {
    t_sum = t_sum + t.sum();
  }
}

int main() {
  at::manual_seed(123);

  test(-1);
  std::thread t1(test, -1);
  t1.join();
  at::set_num_threads(4);
  std::thread t2(test, 4);
  std::thread t3(test, 4);
  std::thread t4(test, 4);
  t4.join();
  t3.join();
  t2.join();
  at::set_num_threads(5);
  test(5);

  return 0;
}
