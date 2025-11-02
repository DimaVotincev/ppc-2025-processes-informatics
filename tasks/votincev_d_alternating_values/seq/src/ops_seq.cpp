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
  vect_data_ = GetInput();
  return true;
}

bool VotincevDAlternatingValuesSEQ::RunImpl() {
  int all_swaps = 0;
  for (size_t j = 1; j < vect_data_.size(); j++) {
    if ((vect_data_[j - 1] < 0 && vect_data_[j] >= 0) || (vect_data_[j - 1] >= 0 && vect_data_[j] < 0)) {
      all_swaps++;
    }
  }

  GetOutput() = all_swaps;
  return true;
}

bool VotincevDAlternatingValuesSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace votincev_d_alternating_values
