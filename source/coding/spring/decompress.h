#ifndef SPRING_DECOMPRESS_H_
#define SPRING_DECOMPRESS_H_

#include <vector>
#include <string>
#include <map>
#include "spring/util.h"
#include "fileio/fastq_record.h"

namespace spring {

struct decoded_desc_t {
  std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
  std::string quality_arr;
  std::vector<int64_t> tokens[128][8];
};

std::vector<dsg::input::fastq::FastqRecord> decode_streams(decoded_desc_t &desc, const std::vector<std::array<uint8_t,2>> &subseq_indices, bool paired_end);

bool decompress(const std::string &temp_dir);

}  // namespace spring

#endif  // SPRING_DECOMPRESS_H_
