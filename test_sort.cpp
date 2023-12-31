#include <algorithm>
#include <execution>
#include <iostream>

#define FORCE_STLEXT
#include "stlext.h"
#include "test.h"

int main(int argc, char** argv) {
  srand(0);
  size_t n = 1e8;
  if (argc > 1) n = std::atoi(argv[1]);
  std::vector<float> values(n);

  int method = 1, n_test = 1;
  if (argc > 2) method = std::atoi(argv[2]);
  if (argc > 3) n_test = std::atoi(argv[3]);

  std::cout << "testing n = " << n / 1e6 << "M with "
            << std::thread::hardware_concurrency() << " threads with ";
  if (method == 0) std::cout << "serial sort";
  if (method == 1) std::cout << "parasort";
  if (method == 2) std::cout << "c++17 parallel sort";
  std::cout << std::endl;

  double total_time = 0.0;
  for (int iter = 0; iter < n_test; iter++) {
    for (size_t i = 0; i < n; i++)
      values[i] = double(rand()) / double(RAND_MAX);

    Timer timer;
    timer.start();
    if (method == 0) {
#if defined(__GNUC__) and not defined(__clang__)
      std::sort(values.begin(), values.end(), [](float a, float b){ return a > b; },  __gnu_parallel::sequential_tag());
#else
      std::sort(values.begin(), values.end(),
                [](float a, float b) { return a > b; });
#endif
    } else if (method == 1) {
      std::parasort(values.begin(), values.end(),
                    [](float a, float b) { return a > b; });
    } else if (method == 2) {
#ifdef __cpp_lib_parallel_algorithm
      std::sort(std::execution::par, values.begin(), values.end(),
                [](float a, float b) { return a > b; });
#else
      std::cerr << "need parallel algorithms" << std::endl;
      assert(false);
#endif
    }
    timer.stop();
    total_time += timer.seconds();
    for (size_t i = 1; i < n; i++) {
      assert(values[i - 1] >= values[i]);
    }
  }
  std::cout << "average time (ms):\n\t" << 1000 * total_time / n_test << std::endl;
  return 0;
}
