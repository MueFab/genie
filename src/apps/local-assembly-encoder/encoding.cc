#include "encoding.h"

#include <map>
#include <util/log.h>
#include <util/sam-file-reader.h>
#include <util/sam-record.h>
#include "util/exceptions.h"
#include "full-local-assembly-decoder.h"
#include "full-local-assembly-encoder.h"
#include "genie-gabac-output-stream.h"

#include <fstream>
#include <util/bitwriter.h>
#include <format/part2/parameter_set.h>
#include <format/part2/access_unit.h>
#include <format/part2/clutter.h>

namespace lae {

    std::vector<std::vector<gabac::EncodingConfiguration>> create_default_conf() {
        const std::vector<size_t> SEQUENCE_NUMS = {2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 3, 2, 1, 1};
        const std::string DEFAULT_GABAC_CONF_JSON =
                "{"
                "\"word_size\": 4,"
                "\"sequence_transformation_id\": 0,"
                "\"sequence_transformation_parameter\": 0,"
                "\"transformed_sequences\":"
                "[{"
                "\"lut_transformation_enabled\": false,"
                "\"diff_coding_enabled\": false,"
                "\"binarization_id\": 0,"
                "\"binarization_parameters\":[32],"
                "\"context_selection_id\": 0"
                "}]"
                "}";
        std::vector<std::vector<gabac::EncodingConfiguration>> ret;
        for (size_t i = 0; i < SEQUENCE_NUMS.size(); ++i) {
            ret.emplace_back();
            for (size_t j = 0; j < SEQUENCE_NUMS[i]; ++j) {
                ret[i].emplace_back(DEFAULT_GABAC_CONF_JSON);
            }
        }
        return ret;
    }

    std::vector<std::vector<std::vector<gabac::DataBlock>>> create_default_streams() {
        std::vector<std::vector<std::vector<gabac::DataBlock>>> ret(format::NUM_DESCRIPTORS);
        for(size_t descriptor = 0; descriptor < format::NUM_DESCRIPTORS; ++descriptor) {
            ret[descriptor].resize(format::getDescriptorProperties()[descriptor].number_subsequences);
        }
        return ret;
    }


    std::vector<std::vector<gabac::DataBlock>> translateToSimpleArray(StreamContainer* container) {
        std::vector<std::vector<gabac::DataBlock>> ret(format::NUM_DESCRIPTORS);
        for(size_t descriptor = 0; descriptor < format::NUM_DESCRIPTORS; ++descriptor) {
            ret[descriptor].resize(format::getDescriptorProperties()[descriptor].number_subsequences);
        }

        ret[uint8_t (format::GenomicDescriptor::pos)][0].swap(&container->pos_0);
        ret[uint8_t (format::GenomicDescriptor::rcomp)][0].swap(&container->rcomp_0);
        ret[uint8_t (format::GenomicDescriptor::flags)][0].swap(&container->flags_0);
        ret[uint8_t (format::GenomicDescriptor::flags)][1].swap(&container->flags_1);
        ret[uint8_t (format::GenomicDescriptor::flags)][2].swap(&container->flags_2);
        ret[uint8_t (format::GenomicDescriptor::mmpos)][0].swap(&container->mmpos_0);
        ret[uint8_t (format::GenomicDescriptor::mmpos)][1].swap(&container->mmpos_1);
        ret[uint8_t (format::GenomicDescriptor::mmtype)][0].swap(&container->mmtype_0);
        ret[uint8_t (format::GenomicDescriptor::mmtype)][1].swap(&container->mmtype_1);
        ret[uint8_t (format::GenomicDescriptor::mmtype)][2].swap(&container->mmtype_2);
        ret[uint8_t (format::GenomicDescriptor::clips)][0].swap(&container->clips_0);
        ret[uint8_t (format::GenomicDescriptor::clips)][1].swap(&container->clips_1);
        ret[uint8_t (format::GenomicDescriptor::clips)][2].swap(&container->clips_2);
        ret[uint8_t (format::GenomicDescriptor::clips)][3].swap(&container->clips_3);
        ret[uint8_t (format::GenomicDescriptor::pair)][0].swap(&container->pair_0);
        ret[uint8_t (format::GenomicDescriptor::pair)][1].swap(&container->pair_1);
        ret[uint8_t (format::GenomicDescriptor::mscore)][0].swap(&container->mscore_0);
        ret[uint8_t (format::GenomicDescriptor::rlen)][0].swap(&container->rlen_0);
        ret[uint8_t (format::GenomicDescriptor::rtype)][0].swap(&container->rtype_0);
        ret[uint8_t (format::GenomicDescriptor::ureads)][0].swap(&container->ureads_0);

        return ret;
    }

    void compress(const gabac::EncodingConfiguration& conf, gabac::DataBlock* in, std::vector<gabac::DataBlock>* out) {

        // Interface to GABAC library
        gabac::IBufferStream bufferInputStream(in);
        GenieGabacOutputStream bufferOutputStream;

        const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
        std::ostream *const GABC_LOG_OUTPUT_STREAM = &std::cout;
        const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                       GABC_LOG_OUTPUT_STREAM,
                                                       gabac::IOConfiguration::LogLevel::TRACE};

        const bool GABAC_DECODING_MODE = false;
        gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);
        bufferOutputStream.flush_blocks(out);
    }

    void pack(const ProgramOptions &programOptions, std::unique_ptr<StreamContainer> container, uint32_t read_length,
              uint32_t readNum, bool paired) {
        using namespace format;
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // ENTROPY ENCODE
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::vector<std::vector<gabac::DataBlock>>> generated_streams = create_default_streams();
        std::vector<std::vector<gabac::EncodingConfiguration>> configs = create_default_conf();
        std::vector<std::vector<gabac::DataBlock>> raw_data = translateToSimpleArray(container.get());

        if(read_length != 0) {
            container->rlen_0.clear();
        }

        for(size_t descriptor = 0; descriptor < format::NUM_DESCRIPTORS; ++descriptor) {
            for(size_t subdescriptor = 0; subdescriptor < format::getDescriptorProperties()[descriptor].number_subsequences; ++subdescriptor) {
                compress(configs[descriptor][subdescriptor], &raw_data[descriptor][subdescriptor], &generated_streams[descriptor][subdescriptor]);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATE Part 2 units
        /////////////////////////////////////////////////////////////////////////////////////////////////////


        std::ofstream ofstr(programOptions.outputFilePath);
        util::BitWriter bw(&ofstr);

        const uint8_t PARAMETER_SET_ID = 0;
        const uint32_t READ_LENGTH = read_length;
        const bool QV_PRESENT = false;

        ParameterSet ps = createQuickParameterSet(PARAMETER_SET_ID, READ_LENGTH, paired, QV_PRESENT, programOptions.type, configs, false);
        auto crps = make_unique<ParameterSetCrps>(ParameterSetCrps::CrAlgId::LOCAL_ASSEMBLY);
        crps->setCrpsInfo(make_unique<CrpsInfo>(0, 1000));
        ps.setCrps(std::move(crps));
        ps.write(&bw);

        const uint32_t ACCESS_UNIT_ID = 0;
        AccessUnit au = createQuickAccessUnit(ACCESS_UNIT_ID, PARAMETER_SET_ID, readNum, programOptions.type, DataUnit::DatasetType::ALIGNED, &generated_streams);
        au.write(&bw);
    }

    void encode(const ProgramOptions &programOptions) {
        LOG_DEBUG << "Encoding";
        LOG_INFO << "Input file: " << programOptions.inputFilePath;

        util::SamFileReader samFileReader(programOptions.inputFilePath);

        size_t blockSize = 10000;
        bool singleEnd = false;
        const uint32_t SEQUENCE_BUFFER_SIZE = 1000;
        FullLocalAssemblyEncoder encoder(SEQUENCE_BUFFER_SIZE, false);
        uint32_t record_counter = 0;
        uint32_t read_length = std::numeric_limits<uint32_t>::max();
        while (true) {
            // Read a block of SAM records
            std::list<util::SamRecord> samRecords;
            samFileReader.readRecords(blockSize, &samRecords);
            std::list<util::SamRecord> samRecordsCopy(samRecords);
            LOG_TRACE << "Read " << samRecords.size() << " SAM record(s)";
            for (const auto &samRecord : samRecords) {
                record_counter++;
                // Search for mate
                std::string rnameSearchString;
                if (samRecord.rnext == "=") {
                    rnameSearchString = samRecord.rname;
                } else {
                    rnameSearchString = samRecord.rnext;
                }
                samRecordsCopy.erase(samRecordsCopy.begin());  // delete current record from the search space
                bool foundMate = false;
                for (auto it = samRecordsCopy.begin(); it != samRecordsCopy.end(); ++it) {
                    if (it->rname == rnameSearchString && it->pos == samRecord.pnext) {
                        // LOG_TRACE << "Found mate";
                        encoder.addPair(samRecord, *it);
                        record_counter++;
                        if (read_length != 0) {
                            if (read_length == std::numeric_limits<uint32_t>::max()) {
                                read_length = samRecord.seq.length();
                            } else {
                                if (samRecord.seq.length() != read_length) {
                                    read_length = 0;
                                }
                            }
                        }
                        if (read_length != 0) {
                            if (read_length == std::numeric_limits<uint32_t>::max()) {
                                read_length = it->seq.length();
                            } else {
                                if (it->seq.length() != read_length) {
                                    read_length = 0;
                                }
                            }
                        }
                        foundMate = true;
                        samRecordsCopy.erase(it);
                        break;
                    }
                }
                if (!foundMate) {
                    // LOG_TRACE << "Did not find mate";
                    singleEnd = true;
                    encoder.add(samRecord);
                    if (read_length != 0) {
                        if (read_length == std::numeric_limits<uint32_t>::max()) {
                            read_length = samRecord.seq.length();
                        } else {
                            if (samRecord.seq.length() != read_length) {
                                read_length = 0;
                            }
                        }
                    }
                }

                // Break if everything was processed
                if (samRecordsCopy.empty()) {
                    break;
                }
            }

            // Break if less than blockSize records were read from the SAM file
            if (samRecords.size() < blockSize) {
                break;
            }
        }

#if 0
        FullLocalAssemblyDecoder decoder(encoder.pollStreams(), SEQUENCE_BUFFER_SIZE, true);
        util::SamRecord s;
        util::SamRecord s2;
        if (singleEnd) {
            for (uint32_t i = 0; i < record_counter; ++i) {
                decoder.decode(&s);
            }
        } else {
            for (uint32_t i = 0; i < record_counter / 2; ++i) {
                decoder.decodePair(&s, &s2);
            }
        }
#else
        pack(programOptions, encoder.pollStreams(), read_length, record_counter, !singleEnd);
#endif

    }

}  // namespace lae
