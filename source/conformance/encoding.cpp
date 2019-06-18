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
    const size_t inputDataBlockInitialSize = decodedUreads.size();
    const size_t INPUT_DATABLOCK_WORDSIZE = 1;
    const size_t OUTPUT_DATABLOCK_INITIAL_SIZE = 0;
    const size_t OUTPUT_DATABLOCK_WORDSIZE = 1;

    gabac::DataBlock inputDataBlock(decoded_symbols.data(), inputDataBlockInitialSize, INPUT_DATABLOCK_WORDSIZE);
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
    for (const auto& bsp : generated_streams) {
        const size_t BLOCK_PAYLOAD_SIZE = bsp.getRawSize();
        const char* const BLOCK_PAYLOAD = static_cast<const char *> (bsp.getData());
        GENIE_LOG_TRACE << "Block payload size: " << BLOCK_PAYLOAD_SIZE;
        GENIE_LOG_TRACE << "Block payload: ";
        for (size_t i = 0; i < BLOCK_PAYLOAD_SIZE; i++) {
            std::cout << std::hex << "0x" << static_cast<int>(static_cast<unsigned char>(BLOCK_PAYLOAD[i])) << " ";
        }
        std::cout << std::endl;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATE PARAMETER SET
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // TODO

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATE ACCESS UNIT
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        const uint32_t ACCESS_UNIT_ID = 0;
        const uint8_t NUM_BLOCKS = 1;
        const uint16_t PARAMETER_SET_ID = 0;
        const ClassType CLASS_TYPE = { .classType = CLASS_TYPE_CLASS_U };
        const uint32_t READS_COUNT = 1;
        const uint16_t MM_THRESHOLD = 0;
        const uint32_t MM_COUNT = 0;
        const SequenceID SEQUENCE_ID = { .sequenceID = 0 };
        const uint64_t REFERENCE_START_POS = 0;
        const uint64_t REFERENCE_END_POS = 0;
        const SequenceID REFSEQUENCE_ID = { .sequenceID = 0 };
        const uint64_t AU_START_POSITION = 0;
        const uint64_t AU_END_POSITION = 0;
        const uint64_t EXTENDED_AU_START_POSITION = 0;
        const uint64_t EXTENDED_AU_END_POSITION = 0;

        DataUnitAccessUnit* const accessUnit = initDataUnitAccessUnit(
                ACCESS_UNIT_ID,
                NUM_BLOCKS,
                PARAMETER_SET_ID,
                CLASS_TYPE,
                READS_COUNT,
                MM_THRESHOLD,
                MM_COUNT,
                SEQUENCE_ID,
                REFERENCE_START_POS,
                REFERENCE_END_POS,
                REFSEQUENCE_ID,
                AU_START_POSITION,
                AU_END_POSITION,
                EXTENDED_AU_START_POSITION,
                EXTENDED_AU_END_POSITION
         );

        // DatasetContainer* const datasetContainer = initDatasetContainer();
        const uint8_t DESCRIPTOR_ID = 6;
        DataUnitBlockHeader* blockHeader = initDataUnitBlockHeader(DESCRIPTOR_ID, BLOCK_PAYLOAD_SIZE);
        Block* const block = initBlockWithHeaderPayloadInMemory(
                DESCRIPTOR_ID,
                BLOCK_PAYLOAD_SIZE,
                BLOCK_PAYLOAD,
                BLOCK_PAYLOAD_SIZE
        );

        bool success = addBlockToDataUnitAccessUnit(accessUnit, block, blockHeader);
        if (!success) {
            GENIE_DIE("addBlockToDataUnitAccessUnit() failed");
        }

        freeDataUnitAccessUnit(accessUnit);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // WRITE TO FILE
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // TODO
    }

    // PLAN
    // #  name     ETA          description
    // 1) Fabian   2019-06-19   get classes InputFile and OutputFile up and running
    // 2) Daniel   2019-06-21   get encoding parameters generation function template up and running
    // 3) Jan      2019-06-25   fill encoding parameters
    // 4) Fabian   2019-06-28   x-check w/ ref SW
}


}  // namespace genie
