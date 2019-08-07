#ifndef SPRING_DECOMPRESS_H_
#define SPRING_DECOMPRESS_H_

#include <vector>
#include <string>
#include <map>
#include <genie/stream-saver.h>
#include "util.h"
#include <utils/fastq-record.h>

namespace spring {

    struct decoded_desc_t {
        std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
        std::string quality_arr;
        std::vector<int64_t> tokens[128][8];
    };

    void decode_streams(decoded_desc_t &dec, bool paired_end, bool preserve_quality, bool preserve_id, bool combine_pairs, std::vector<utils::FastqRecord> matched_records[2], std::vector<utils::FastqRecord> unmatched_records[2], std::vector<uint32_t> &mate_au_id, std::vector<uint32_t> &mate_record_index);

    void decode_streams_ureads(decoded_desc_t &dec, bool paired_end, bool preserve_quality, bool preserve_id, std::vector<utils::FastqRecord> matched_records[2]);

    bool decompress(const std::string &temp_dir, dsg::StreamSaver *ld, bool combine_pairs = false);

    void write_fastq_record_to_ostream(std::ostream &out, utils::FastqRecord &fastqRecord, bool preserve_quality);

}  // namespace spring

#endif  // SPRING_DECOMPRESS_H_
