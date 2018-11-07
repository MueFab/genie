#ifndef SPRING_DECODE_ERU_H_
#define SPRING_DECODE_ERU_H_

#include <vector>
#include <string>
#include <map>
#include "descriptors/spring/util.h"
#include "fileio/fastq_record.h"

namespace spring {

struct subsequences_t {
  std::vector<int64_t> subseq_0_0;
  std::vector<int64_t> subseq_1_0;
  std::vector<int64_t> subseq_3_0;
  std::vector<int64_t> subseq_3_1;
  std::vector<int64_t> subseq_4_0;
  std::vector<int64_t> subseq_4_1;
  std::vector<int64_t> subseq_6_0;
  std::vector<int64_t> subseq_7_0;
  std::vector<int64_t> subseq_8_0;
  std::vector<int64_t> subseq_8_10;
  std::vector<int64_t> subseq_8_11;
  std::vector<int64_t> subseq_8_12;
  std::vector<int64_t> subseq_12_0;
  std::string quality_arr;
  std::vector<int64_t> tokens[128][8]; 
};

std::vector<dsg::input::fastq::FastqRecord> decode_streams(const subsequences_t &subseq, const std::string &ref, bool eru_abs_flag, bool paired_end);

std::string decode_type_U(std::vector<int64_t>::const_iterator &subseq_6_0_it, uint32_t rlen, char int_to_char[]);

std::string decode_ref_AU (const std::vector<int64_t> &subseq_6_0, const std::vector<int64_t> &subseq_7_0);

void decompress(const std::string &temp_dir, const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &ref_descriptorFilesPerAU, uint32_t num_blocks, bool eru_abs_flag, bool paired_end);

}  // namespace spring

#endif  // SPRING_DECODE_ERU_H_
