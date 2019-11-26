#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>
#include "compress-ureads.h"
#include "generate-read-streams.h"
#include "params.h"
#include "reorder-compress-quality-id.h"
#include "spring-gabac.h"
#include "util.h"

#include <format/part2/access_unit.h>
#include <format/part2/clutter.h>
#include <format/part2/parameter_set.h>
#include <util/bitwriter.h>

namespace spring {

void compress_ureads(util::FastqFileReader *fastqFileReader1, util::FastqFileReader *fastqFileReader2,
                     const std::string &temp_dir, compression_params &cp, const std::string &outputFilePath) {
    using namespace format;
    std::ofstream ofstr(outputFilePath);
    util::BitWriter bw(&ofstr);

    std::vector<std::vector<gabac::EncodingConfiguration>> configs = create_default_conf();
    const uint8_t PARAMETER_SET_ID = 0;
    const uint32_t READ_LENGTH = 0;
    const bool PAIRED_END = cp.paired_end;
    const bool QV_PRESENT = cp.preserve_quality;
    ParameterSet ps = createQuickParameterSet(PARAMETER_SET_ID, READ_LENGTH, PAIRED_END, QV_PRESENT,
                                              DataUnit::AuType::U_TYPE_AU, configs, true);
    ps.write(&bw);

    util::FastqFileReader *fastqFileReader[2] = {fastqFileReader1, fastqFileReader2};
    std::string basedir = temp_dir;
    std::string outfileid = basedir + "/id_1";
    std::string outfilequality = basedir + "/quality_1";

    std::vector<util::FastqRecord> fastqRecords[2];

    int number_of_record_segments = cp.paired_end ? 2 : 1;
    uint32_t max_readlen = 0;
    uint64_t num_reads = 0;
    uint32_t num_reads_per_block;
    num_reads_per_block = cp.num_reads_per_block;

    size_t num_threads = cp.num_thr;

    uint64_t num_reads_per_step = (uint64_t)num_threads * num_reads_per_block;
    std::string *id_array_1 = new std::string[num_reads_per_step];
    uint32_t num_blocks_done = 0;

    while (true) {
        bool done = false;
        uint32_t num_reads_read[2] = {0, 0};
        // read lines from the file(s)
        for (int j = 0; j < number_of_record_segments; j++) {
            num_reads_read[j] = fastqFileReader[j]->readRecords(num_reads_per_step, &fastqRecords[j]);
        }
        // basic checks to determine if we are done
        if (cp.paired_end)
            if (num_reads_read[0] != num_reads_read[1])
                throw std::runtime_error("Number of reads in paired files do not match.");
        if (num_reads + num_reads_read[0] + num_reads_read[1] > MAX_NUM_READS) {
            std::cerr << "Max number of reads allowed is " << MAX_NUM_READS << "\n";
            throw std::runtime_error("Too many reads.");
        }
        if (num_reads_read[0] < num_reads_per_step) done = true;

        if (num_reads_read[0] != 0) {
            // parallel portion starts, includes generation of streams and their
            // compression
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for ordered num_threads(num_threads) schedule(dynamic)
#endif
            for (size_t tid = 0; tid < num_threads; tid++) {
                size_t block_num_thr = num_blocks_done + tid;
                bool done_loop = false;
                if (tid * num_reads_per_block >= num_reads_read[0]) done_loop = true;
                uint32_t num_reads_thr =
                    std::min((size_t)num_reads_read[0], (tid + 1) * num_reads_per_block) - tid * num_reads_per_block;
                subseq_data subseqData;
                std::string outfile_name_quality;
                gabac::DataBlock qualityBuffer(0, 1);
                dsg::AcessUnitStreams streams;
                std::string outfile_name_id;
                if (!done_loop) {
                    // check if reads and qualities have equal lengths (and more such
                    // checks)
                    for (int j = 0; j < number_of_record_segments; j++) {
                        for (uint32_t i = tid * num_reads_per_block; i < tid * num_reads_per_block + num_reads_thr;
                             i++) {
                            size_t len = fastqRecords[j][i].sequence.length();
                            if (len == 0) throw std::runtime_error("Read of length 0 detected.");
                            if (len > MAX_READ_LEN_UREADS) {
                                std::cerr << "Max read length in ureads mode is " << MAX_READ_LEN_UREADS
                                          << ", but found read of length " << len << "\n";
                                throw std::runtime_error("Too long read length");
                            }
                            if (cp.preserve_quality && (fastqRecords[j][i].qualityScores.length() != len))
                                throw std::runtime_error("Read length does not match quality length.");
                            if (cp.preserve_id && (fastqRecords[j][i].title.length() == 0) && j == 0)
                                throw std::runtime_error("Identifier of length 0 detected.");
                            if (cp.preserve_id && j == 0) {
                                id_array_1[i] = fastqRecords[j][i].title;
                            }
                        }
                    }

                    // generate and compress subsequences
                    auto raw_data = generate_empty_raw_data();

                    // char_to_int
                    int64_t char_to_int[128];
                    char_to_int[(uint8_t)'A'] = 0;
                    char_to_int[(uint8_t)'C'] = 1;
                    char_to_int[(uint8_t)'G'] = 2;
                    char_to_int[(uint8_t)'T'] = 3;
                    char_to_int[(uint8_t)'N'] = 4;

                    for (uint32_t i = tid * num_reads_per_block; i < tid * num_reads_per_block + num_reads_thr; i++) {
                        for (int j = 0; j < number_of_record_segments; j++) {
                            size_t rlen = fastqRecords[j][i].sequence.length();
                            max_readlen = std::max((size_t)max_readlen, rlen);
                            raw_data[7][0].push_back(rlen - 1);  // rlen
                            for (size_t k = 0; k < rlen; k++) {
                                raw_data[6][0].push_back(
                                    char_to_int[(uint8_t)fastqRecords[j][i].sequence[k]]);  // ureads
                                if (cp.preserve_quality)
                                    raw_data[14][2].push_back((uint8_t)fastqRecords[j][i].qualityScores[k] -
                                                              33);  // quality
                            }
                        }
                        if (cp.paired_end) raw_data[8][0].push_back(0);  // pair (in same record)
                    }

                    std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams =
                        create_default_streams();
                    for (size_t descriptor = 0; descriptor < format::NUM_DESCRIPTORS; ++descriptor) {
                        if (descriptor != 11) {  // rname
                            for (size_t subdescriptor = 0;
                                 subdescriptor < format::getDescriptorProperties()[descriptor].number_subsequences;
                                 ++subdescriptor) {
                                gabac_compress(configs[descriptor][subdescriptor], &raw_data[descriptor][subdescriptor],
                                               &generated_streams[descriptor][subdescriptor]);
                            }
                        }
                    }
                    if (cp.preserve_id) {
                        generate_read_id_tokens(id_array_1 + tid * num_reads_per_block, num_reads_thr, raw_data[15]);
                        for (int i = 0; i < 128; i++) {
                            for (int j = 0; j < 6; j++) {
                                gabac_compress(configs[15][0], &raw_data[15][6 * i + j],
                                               &generated_streams[15][6 * i + j]);
                            }
                        }
                    }

                    uint32_t ACCESS_UNIT_ID = block_num_thr;
                    uint32_t num_reads_au = cp.paired_end ? num_reads_thr * 2 : num_reads_thr;
                    AccessUnit au = createQuickAccessUnit(
                        ACCESS_UNIT_ID, PARAMETER_SET_ID, num_reads_au, DataUnit::AuType::U_TYPE_AU,
                        DataUnit::DatasetType::NON_ALIGNED, &generated_streams, num_reads_thr);

#ifdef GENIE_USE_OPENMP
#pragma omp ordered
#endif
                    { au.write(&bw); }
                }  // if (!done_loop)
            }  // omp parallel for

            num_reads += num_reads_read[0] + num_reads_read[1];
        }  // if (num_reads_read[0] != 0)
        if (done) break;
        num_blocks_done += num_threads;
    }  // while(true)

    delete[] id_array_1;

    if (num_reads == 0) throw std::runtime_error("No reads found.");

    cp.num_reads = num_reads;
    cp.max_readlen = max_readlen;

    std::cout << "Max Read length: " << cp.max_readlen << "\n";
    std::cout << "Total number of reads: " << cp.num_reads << "\n";
    return;
}

}  // namespace spring
