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
    size_t blockSize = 10;

    std::string decodedUreads;

    while (true) {
        std::vector<genie::FastqRecord> fastqRecords;
        size_t numRecords = fastqFileReader.readRecords(blockSize, &fastqRecords);
        if (numRecords != blockSize) {
            GENIE_LOG_TRACE << "Read only " << numRecords << " records (" << blockSize << " requested)";
        }

        // Iterate through the records.
        for (const auto &fastqRecord : fastqRecords) {
            decodedUreads += fastqRecord.sequence;
        }

        if (numRecords != blockSize) {
            break;
        }
    }

    // Alphabet
    // uint8_t alphabet_ID = 0;
    std::vector<char> S_0 = { 'A', 'C', 'G', 'T', 'N' };
    std::map<char, uint8_t> S_0_inverse= { { 'A', 0 }, { 'C', 1 }, { 'G', 2 }, { 'T', 3 }, { 'N', 4 } };

    GENIE_LOG_TRACE << "decodedUreads (length: " << decodedUreads.size() << "): " << decodedUreads;

    // Lookup in the alphabet
    std::vector<uint8_t> decoded_symbols;
    for (const auto& decodedUreadsChar : decodedUreads) {
        decoded_symbols.push_back(S_0_inverse[decodedUreadsChar]);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ENTROPY ENCODE
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Construct an input data block for GABAC
    size_t inputDataBlockInitialSize = decodedUreads.size();
    size_t inputDataBlockWordSize = 1;
    gabac::DataBlock inputDataBlock(inputDataBlockInitialSize, inputDataBlockWordSize);
    std::memcpy(inputDataBlock.getData(), decoded_symbols.data(), (inputDataBlock.size() * inputDataBlock.getWordSize()));

    // Interface to GABAC library
    gabac::BufferInputStream bufferInputStream(&inputDataBlock);
    GenieGabacOutputStream bufferOutputStream;

    std::string defaultGabacConf = "{"
                                       "\"word_size\":\"1\","
                                       "\"sequence_transformation_id\":\"0\","
                                       "\"sequence_transformation_parameter\":\"0\","
                                       "\"transformed_sequences\":"
                                       "[{"
                                           "\"lut_transformation_enabled\":\"0\","
                                           "\"lut_transformation_bits\":\"0\","
                                           "\"lut_transformation_order\":\"0\","
                                           "\"diff_coding_enabled\":\"0\","
                                           "\"binarization_id\":\"0\","
                                           "\"binarization_parameters\":[\"3\"],"
                                           "\"context_selection_id\":\"0\""
                                       "}]"
                                   "}";

    size_t gabacBlockSize = 0;
    gabac::IOConfiguration ioconf = { &bufferInputStream, &bufferOutputStream, gabacBlockSize, &std::cout, gabac::IOConfiguration::LogLevel::TRACE };
    gabac::EncodingConfiguration enConf(defaultGabacConf);

    gabac::encode(ioconf, enConf);

    // Get the GABAC bitstream(s)
    size_t outputDataBlockInitialSize = 0;
    size_t outputDataBlockWordSize = 1;
    std::vector<std::pair<size_t, uint8_t *>> data;
    bufferOutputStream.flush(&data);
    GENIE_LOG_TRACE << "Number of bitstreams: " << data.size();
    for (const auto& bsp : data) {
        size_t blockPayloadSize = bsp.first;
        GENIE_LOG_TRACE << "Block payload size: " << blockPayloadSize;
        GENIE_LOG_TRACE << "Block payload: ";
        for (size_t i = 0; i < blockPayloadSize; i++) {
            std::cout << std::hex << "0x" << static_cast<int>(bsp.second[i]) << " ";
        }
        std::cout << std::endl;

        // DONE(Fabian): align this with the block payload syntax
        // TODO(Fabian): free allocated memory
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
        DataUnitBlockHeader* blockHeader = initDataUnitBlockHeader(6, blockPayloadSize);
        Block* block = initBlockWithHeaderPayloadInMemory(
            6,                                    // descriptorId
            blockPayloadSize,                     // payloadSize
            reinterpret_cast<char*>(bsp.second),  // payloadInMemory
            bsp.first);                           // size_t payloadInMemorySize)

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
