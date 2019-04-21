#ifndef SPRING_DECODE_READ_STREAMS_H_
#define SPRING_DECODE_READ_STREAMS_H_

#include <vector>
#include <string>
#include "descriptors/spring/util.h"

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

struct subsequences_pe_t {
  std::vector<int64_t> subseq_0_0;
  std::vector<int64_t> subseq_1_0;
  std::vector<int64_t> subseq_3_0;
  std::vector<int64_t> subseq_3_1;
  std::vector<int64_t> subseq_4_0;
  std::vector<int64_t> subseq_4_1;
  std::vector<int64_t> subseq_6_0;
  std::vector<int64_t> subseq_7_0;
  std::vector<int64_t> subseq_8_0;
  std::vector<int64_t> subseq_8_1;
  std::vector<int64_t> subseq_8_2;
  std::vector<int64_t> subseq_8_3;
  std::vector<int64_t> subseq_8_4;
  std::vector<int64_t> subseq_8_5;
  std::vector<int64_t> subseq_8_7;
  std::vector<int64_t> subseq_8_8;
  std::vector<int64_t> subseq_12_0;
};

std::vector<std::string> decode_read_streams_se(const subsequences_se_t &subseq);

std::vector<std::string> decode_read_streams_pe(const subsequences_pe_t &subseq);

void decompress_se_reads(const std::string &temp_dir, uint32_t num_blocks);

void decompress_pe_reads(const std::string &temp_dir, uint32_t num_blocks);

}  // namespace spring

#endif  // SPRING_DECODE_READ_STREAMS_H_
