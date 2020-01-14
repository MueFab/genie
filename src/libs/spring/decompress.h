#ifndef SPRING_DECOMPRESS_H_
#define SPRING_DECOMPRESS_H_

#include <genie/stream-saver.h>
#include <map>
#include <string>
#include <vector>
#include "util.h"

namespace genie {
namespace spring {

struct decoded_desc_t {
    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
    std::string quality_arr;
    std::vector<int64_t> tokens[128][6];
};

/*void decode_streams(decoded_desc_t &dec, bool paired_end, bool preserve_quality, bool preserve_id, bool combine_pairs,
                    std::vector<format::fastq::FastqRecord> matched_records[2],
                    std::vector<format::fastq::FastqRecord> unmatched_records[2], std::vector<uint32_t> &mate_au_id,
                    std::vector<uint32_t> &mate_record_index);

void decode_streams_ureads(decoded_desc_t &dec, bool paired_end, bool preserve_quality, bool preserve_id,
                           std::vector<format::fastq::FastqRecord> matched_records[2]);

bool decompress(const std::string &temp_dir, dsg::StreamSaver *ld, int num_thr, bool combine_pairs);

void write_fastq_record_to_ostream(std::ostream &out, format::fastq::FastqRecord &fastqRecord, bool preserve_quality);

void read_fastq_record_from_ifstream(std::ifstream &in, format::fastq::FastqRecord &fastqRecord, bool
preserve_quality);*/

}  // namespace spring
}  // namespace genie
#endif  // SPRING_DECOMPRESS_H_
