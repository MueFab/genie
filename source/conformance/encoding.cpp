#include "conformance/encoding.h"

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "conformance/exceptions.h"
#include "conformance/fastq_file_reader.h"
#include "conformance/fastq_record.h"
#include "conformance/genie_gabac_output_stream.h"
#include "conformance/log.h"

extern "C" {
#include "format/DataUnits/DataUnits.h"
}

#include "gabac/gabac.h"


namespace genie {


void encode(const ProgramOptions &programOptions)
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // UREADS DESCRIPTOR GENERATION
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize a FASTQ file reader.
    genie::FastqFileReader fastqFileReader(programOptions.inputFilePath);

    // Read FASTQ records in blocks of 10 records.
    const size_t BLOCK_SIZE = 10;

    std::string decodedUreads;

    while (true) {
        std::vector<genie::FastqRecord> fastqRecords;
        size_t numRecords = fastqFileReader.readRecords(BLOCK_SIZE, &fastqRecords);
        if (numRecords != BLOCK_SIZE) {
            GENIE_LOG_TRACE << "Read only " << numRecords << " records (" << BLOCK_SIZE << " requested)";
        }

        // Iterate through the records.
        for (const auto &fastqRecord : fastqRecords) {
            decodedUreads += fastqRecord.sequence;
        }

        if (numRecords != BLOCK_SIZE) {
            break;
        }
    }

    // Alphabet
    // uint8_t alphabet_ID = 0;
    const std::vector<char> S_0 = { 'A', 'C', 'G', 'T', 'N' };
    const std::map<char, uint8_t> S_0_INVERSE= { { 'A', 0 }, { 'C', 1 }, { 'G', 2 }, { 'T', 3 }, { 'N', 4 } };

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
    const size_t INPUT_DATABLOCK_INITIAL_SIZE = decodedUreads.size();
    const size_t INPUT_DATABLOCK_WORDSIZE = 1;
    const size_t OUTPUT_DATABLOCK_INITIAL_SIZE = 0;
    const size_t OUTPUT_DATABLOCK_WORDSIZE = 1;

    gabac::DataBlock inputDataBlock(decoded_symbols.data(), INPUT_DATABLOCK_INITIAL_SIZE, INPUT_DATABLOCK_WORDSIZE);
    gabac::DataBlock outputDataBlock(OUTPUT_DATABLOCK_INITIAL_SIZE, OUTPUT_DATABLOCK_WORDSIZE);

    // Interface to GABAC library
    gabac::IBufferStream bufferInputStream(&inputDataBlock);
    GenieGabacOutputStream bufferOutputStream;

    const std::string DEFAULT_GABAC_CONF_JSON = "{"
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
    const gabac::EncodingConfiguration GABAC_CONFIG(DEFAULT_GABAC_CONF_JSON);
    const size_t GABAC_BLOCK_SIZE = 0; // 0 means single block (block size is equal to input size)
    std::ostream* const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = { &bufferInputStream, &bufferOutputStream,
                                                    GABAC_BLOCK_SIZE,
                                                    GABC_LOG_OUTPUT_STREAM, gabac::IOConfiguration::LogLevel::TRACE };

    const bool GABAC_DECODING_MODE = false;
    gabac::run(GABAC_IO_SETUP, GABAC_CONFIG, GABAC_DECODING_MODE);

    // Get the GABAC bitstream(s)
    std::vector<gabac::DataBlock> generated_streams;
    bufferOutputStream.flush_blocks(&generated_streams);

    GENIE_LOG_TRACE << "Number of bitstreams: " << generated_streams.size();
    for (auto& bsp : generated_streams) {
        const size_t BLOCK_PAYLOAD_SIZE = bsp.getRawSize();
        char* const BLOCK_PAYLOAD = static_cast<char *> (bsp.getData());
        GENIE_LOG_TRACE << "Block payload size: " << BLOCK_PAYLOAD_SIZE;
        GENIE_LOG_TRACE << "Block payload: ";
        for (size_t i = 0; i < BLOCK_PAYLOAD_SIZE; i++) {
            std::cout << std::hex << "0x" << static_cast<int>(BLOCK_PAYLOAD[i]) << " ";
        }
        std::cout << std::endl;

        // DONE(Fabian): align this with the block payload syntax
        // DONE(Tom): check GABAC's byte order - it's LSB


        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATE PARAMETER SET
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // TODO(Daniel): rename parameterSSSSset
        // TODO(Daniel, Jan, Tom): fill parameter set
        //   - write a separate method
        //   - example code: tests/format/EncodingParameters_tests.cpp [ TEST_F(encodingParametersTest, writeEncodingParametersSingleAlignmentNoComputedTest) ]


        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATE ACCESS UNIT
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        ClassType classType = { .classType = CLASS_TYPE_CLASS_U };
        SequenceID sequenceId = { .sequenceID = 0 };
        SequenceID refSequenceId = { .sequenceID = 0 };
        DataUnitAccessUnit* accessUnit = initDataUnitAccessUnit(
                0, 1, 0, classType, 1, 0, 0, sequenceId, 0, 0, refSequenceId, 0, 0, 0, 0);

        DatasetContainer* datasetContainer = initDatasetContainer();
        // DONE(Daniel): add support for init block from byte array
        DataUnitBlockHeader* blockHeader = initDataUnitBlockHeader(6, BLOCK_PAYLOAD_SIZE);
        Block* block = initBlockWithHeaderPayloadInMemory(
            6,                                    // descriptorId
            BLOCK_PAYLOAD_SIZE,                     // payloadSize
            BLOCK_PAYLOAD,  // payloadInMemory TODO(Daniel): does this need to be non-const? Is the memory modified?
            BLOCK_PAYLOAD_SIZE);                           // size_t payloadInMemorySize)

        bool success = addBlockToDataUnitAccessUnit(accessUnit, block, blockHeader);
        if (!success) {
            GENIE_DIE("addBlockToDataUnitAccessUnit() failed");
        }

        freeDataUnitAccessUnit(accessUnit);


        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // WRITE TO FILE
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // TODO(Jan): copy output_file and input_file classes from GABAC to genie
        // TODO(Jan): get output file name from program options
        // TODO(Daniel): get writeParametersSet() and writeDataUnitAccessUnit() up & running
        // FILE* outputFilePointer = nullptr;
        // writeParametersSet(parameterSet, outputFilePointer);
        // writeDataUnitAccessUnit(accessUnit, false, outputFilePointer);


        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // STUFF FOR LATER DISCUSSIONS
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // TODO(Fabian): copy bit_input_stream and bit_output_stream from GABAC to genie, use std::vector instead of DataBlock again
    }
}


}  // namespace genie
