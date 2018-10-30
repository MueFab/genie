#ifndef SPRING_DECODE_READ_STREAMS_H_
#define SPRING_DECODE_READ_STREAMS_H_

#include <vector>
#include <string>
#include "algorithms/SPRING/util.h"

namespace spring {

struct subsequences_se_t {
  std::vector<int64_t> subseq_0_0;
  std::vector<int64_t> subseq_1_0;
  std::vector<int64_t> subseq_3_0;
  std::vector<int64_t> subseq_3_1;
  std::vector<int64_t> subseq_4_0;
  std::vector<int64_t> subseq_4_1;
  std::vector<int64_t> subseq_6_0;
  std::vector<int64_t> subseq_7_0;
  std::vector<int64_t> subseq_12_0;
};

std::vector<std::string> decode_read_streams_se(const subsequences_se_t &subseq);

void decompress_se_reads(const std::string &temp_dir, uint32_t num_blocks);

}  // namespace spring

#endif  // SPRING_DECODE_READ_STREAMS_H_
