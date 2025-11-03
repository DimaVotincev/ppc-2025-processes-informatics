#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "util/include/func_test_util.hpp"
#include "votincev_d_alternating_values/common/include/common.hpp"
#include "votincev_d_alternating_values/mpi/include/ops_mpi.hpp"
#include "votincev_d_alternating_values/seq/include/ops_seq.hpp"

namespace votincev_d_alternating_values {

class VotincevDAlternatigValuesRunFuncTestsProcesses : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  // считываем/генерируем данные
  void SetUp() override {
    int vect_size = expected_res_ + 1;  // чтобы чередований было ровно expected_res_
    std::vector<double> vect_data;
    int swapper = 1;
    for (int i = 0; i < vect_size; i++) {
      vect_data.push_back(i * swapper);  // 0 -1 2 -3 4 ...
      swapper *= -1;
    }
    input_data_ = vect_data;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_res_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_res_ = 10;
};

namespace {

TEST_P(VotincevDAlternatigValuesRunFuncTestsProcesses, CountSwapsFromGenerator) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 1> kTestParam = {"myDefaultTest"};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<VotincevDAlternatingValuesMPI, InType>(
                                               kTestParam, PPC_SETTINGS_votincev_d_alternating_values),
                                           ppc::util::AddFuncTask<VotincevDAlternatingValuesSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_votincev_d_alternating_values));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    VotincevDAlternatigValuesRunFuncTestsProcesses::PrintFuncTestName<VotincevDAlternatigValuesRunFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(CountSwapsFromGeneratorr, VotincevDAlternatigValuesRunFuncTestsProcesses, kGtestValues,
                         kPerfTestName);

}  // namespace

}  // namespace votincev_d_alternating_values
