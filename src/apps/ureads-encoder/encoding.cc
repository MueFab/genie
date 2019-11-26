#include "encoding.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "fastq-file-reader.h"
#include "fastq-record.h"
#include "format/part2/parameter_set/qv_coding_config_1/qv_coding_config_1.h"
#include "genie-gabac-output-stream.h"
#include "log.h"
#include "util/exceptions.h"

#include <format/part2/raw_reference.h>
#include <gabac/gabac.h>

#include "format/part2/access_unit.h"
#include "format/part2/clutter.h"
#include "format/part2/parameter_set.h"
#include "util/bitwriter.h"

namespace genie {

std::vector<std::vector<gabac::EncodingConfiguration>> create_default_conf() {
    const std::vector<size_t> SEQUENCE_NUMS = {2, 1, 3, 2, 3, 4, 1, 1, 8, 1, 5, 2, 1, 1, 1, 2, 1, 1};
    const std::string DEFAULT_GABAC_CONF_JSON =
        "{"
        "\"word_size\": 1,"
        "\"sequence_transformation_id\": 0,"
        "\"sequence_transformation_parameter\": 0,"
        "\"transformed_sequences\":"
        "[{"
        "\"lut_transformation_enabled\": false,"
        "\"diff_coding_enabled\": false,"
        "\"binarization_id\": 0,"
        "\"binarization_parameters\":[3],"
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

std::vector<std::vector<gabac::DataBlock>> create_default_streams() {
    std::vector<std::vector<gabac::DataBlock>> ret;
    for (size_t i = 0; i < format::NUM_DESCRIPTORS; ++i) {
        ret.emplace_back();
    }
    return ret;
}

void encode(const ProgramOptions& programOptions) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // UREADS DESCRIPTOR GENERATION
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize a FASTQ file reader.
    genie::FastqFileReader fastqFileReader(programOptions.inputFilePath);

    // Read FASTQ records in blocks of 10 records.
    const size_t BLOCK_SIZE = 10;

    std::string decodedUreads;

    size_t readNum = 0;
    size_t readSize = 0;

    while (true) {
        std::vector<genie::FastqRecord> fastqRecords;
        size_t numRecords = fastqFileReader.readRecords(BLOCK_SIZE, &fastqRecords);
        readNum += numRecords;
        if (numRecords != BLOCK_SIZE) {
            GENIE_LOG_TRACE << "Read only " << numRecords << " records (" << BLOCK_SIZE << " requested)";
        }

        // Iterate through the records.
        for (const auto& fastqRecord : fastqRecords) {
            decodedUreads += fastqRecord.sequence;
            readSize = fastqRecord.sequence.length();
        }

        if (numRecords != BLOCK_SIZE) {
            break;
        }
    }

    // Alphabet
    // uint8_t alphabet_ID = 0;
    const std::vector<char> S_0 = {'A', 'C', 'G', 'T', 'N'};
    const std::map<char, uint8_t> S_0_INVERSE = {{'A', 0}, {'C', 1}, {'G', 2}, {'T', 3}, {'N', 4}};

    GENIE_LOG_TRACE << "decodedUreads (length: " << decodedUreads.size() << "): " << decodedUreads;

    // Lookup in the alphabet
    std::vector<uint8_t> decoded_symbols;
    for (const auto& decodedUreadsChar : decodedUreads) {
        decoded_symbols.push_back(S_0_INVERSE.at(decodedUreadsChar));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ENTROPY ENCODE
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Construct an input data block for GABAC
    const size_t inputDataBlockInitialSize = decodedUreads.size();
    const size_t INPUT_DATABLOCK_WORDSIZE = 1;
    const size_t OUTPUT_DATABLOCK_INITIAL_SIZE = 0;
    const size_t OUTPUT_DATABLOCK_WORDSIZE = 1;

    gabac::DataBlock inputDataBlock(decoded_symbols.data(), inputDataBlockInitialSize, INPUT_DATABLOCK_WORDSIZE);
    gabac::DataBlock outputDataBlock(OUTPUT_DATABLOCK_INITIAL_SIZE, OUTPUT_DATABLOCK_WORDSIZE);

    // Interface to GABAC library
    gabac::IBufferStream bufferInputStream(&inputDataBlock);
    GenieGabacOutputStream bufferOutputStream;

    const size_t UREADS_DESC_ID = 6;
    std::vector<std::vector<gabac::EncodingConfiguration>> configs = create_default_conf();
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream* const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&bufferInputStream, &bufferOutputStream, GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::TRACE};

    const bool GABAC_DECODING_MODE = false;
    gabac::run(GABAC_IO_SETUP, configs[UREADS_DESC_ID].front(), GABAC_DECODING_MODE);

    // Get the GABAC bitstream(s)
    std::vector<std::vector<gabac::DataBlock>> generated_streams = create_default_streams();
    generated_streams[UREADS_DESC_ID].emplace_back();
    bufferOutputStream.flush_blocks(&generated_streams[UREADS_DESC_ID]);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // CREATE Part 2 units
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    using namespace format;
    std::ofstream ofstr(programOptions.outputFilePath);
    util::BitWriter bw(&ofstr);

    const uint8_t PARAMETER_SET_ID = 0;
    const uint32_t READ_LENGTH = readSize;
    const bool PAIRED_END = false;
    const bool QV_PRESENT = false;

    RawReference r;
    r.addSequence(make_unique<RawReferenceSequence>(0, 10, make_unique<std::string>("AAT")));
    r.write(&bw);

    ParameterSet ps = createQuickParameterSet(PARAMETER_SET_ID, READ_LENGTH, PAIRED_END, QV_PRESENT, DataUnit::AuType::U_TYPE_AU, configs, false);
    ps.write(&bw);

    ParameterSet ps2 = createQuickParameterSet(1, READ_LENGTH, PAIRED_END, QV_PRESENT, DataUnit::AuType::U_TYPE_AU, configs, false);
    ps2.write(&bw);

    const uint32_t ACCESS_UNIT_ID = 0;
  //  AccessUnit au = createQuickAccessUnit(ACCESS_UNIT_ID, PARAMETER_SET_ID, readNum, DataUnit::AuType::U_TYPE_AU, DataUnit::DatasetType::NON_ALIGNED, &generated_streams);
 //   au.write(&bw);

    GENIE_LOG_TRACE << "Number of bitstreams: " << generated_streams.size();
}

}  // namespace genie
