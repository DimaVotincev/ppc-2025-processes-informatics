#include "votincev_d_alternating_values/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "util/include/util.hpp"
#include "votincev_d_alternating_values/common/include/common.hpp"

namespace votincev_d_alternating_values {

VotincevDAlternatingValuesSEQ::VotincevDAlternatingValuesSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool VotincevDAlternatingValuesSEQ::ValidationImpl() {
  return !(GetInput().empty());
}

bool VotincevDAlternatingValuesSEQ::PreProcessingImpl() {
  vect_data = GetInput();
  return true;
}

bool VotincevDAlternatingValuesSEQ::RunImpl() {
  // auto start_time = std::chrono::high_resolution_clock::now();

  int all_swaps = 0;
  for (size_t j = 1; j < vect_data.size(); j++) {
    if ((vect_data[j - 1] < 0 && vect_data[j] >= 0) || (vect_data[j - 1] >= 0 && vect_data[j] < 0)) {
      all_swaps++;
    }
  }

  // auto end_time = std::chrono::high_resolution_clock::now();
  // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  // std::cout << "SEQ_was_working:" << duration.count() / 1000000.0 << " seconds" << std::endl;

  GetOutput() = all_swaps;
  // std::cout<< "Seq result: " << GetOutput() << "\n";
  return true;
}

bool VotincevDAlternatingValuesSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace votincev_d_alternating_values
