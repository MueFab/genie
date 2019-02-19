#include "conformance/encoding.h"

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "conformance/exceptions.h"
#include "conformance/fastq_file_reader.h"
#include "conformance/fastq_record.h"
#include "conformance/log.h"

#include "format/DataUnits/DataUnits.h"

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
    gabac::BufferOutputStream bufferOutputStream;

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

    // Get the GABAC bitstream
    size_t outputDataBlockInitialSize = 0;
    size_t outputDataBlockWordSize = 1;
    gabac::DataBlock outputDataBlock(outputDataBlockInitialSize, outputDataBlockWordSize);
    bufferOutputStream.flush(&outputDataBlock);
    GENIE_LOG_TRACE << "Block payload size: " << outputDataBlock.size();

    // TODO(Fabian): align this with the block payload syntax
    // TODO(Tom): check GABAC's byte order
    size_t blockPayloadSize = outputDataBlock.size() * outputDataBlock.getWordSize();
    auto *blockPayload = static_cast<uint8_t *>(malloc(blockPayloadSize));
    std::memcpy(blockPayload, static_cast<uint8_t*>(outputDataBlock.getData()), blockPayloadSize);

    // GENIE_LOG_TRACE << "Block payload: ";
    // for (size_t i = 0; i < blockPayloadSize; i++) {
    //     std::cout << std::hex << "0x" << static_cast<int>(blockPayload[i]) << " ";
    // }
    // std::cout << std::endl;


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // CREATE PARAMETER SET
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // TODO(Daniel): rename parameterSSSSset
    // TODO(Daniel): fill parameter set
    ParametersSet* parameterSet = initParametersSet(0, 0);


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // CREATE ACCESS UNIT
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    DataUnitAccessUnit* accessUnit = initDataUnitAccessUnit(
            0, 1, 0, 6, 1, 0, 0, 0, 0, 0, 0, 0);

    DatasetContainer* datasetContainer = initDatasetContainer();
    // TODO(Daniel): add support for init block from byte array
    Block* block = initBlock(datasetContainer, nullptr);
    DataUnitBlockHeader* blockHeader = initDataUnitBlockHeader(6, blockPayloadSize);

    bool success = addBlockToDataUnitAccessUnit(accessUnit, block, blockHeader);
    if (!success) {
        GENIE_DIE("addBlockToDataUnitAccessUnit() failed");
    }

    freeDataUnitAccessUnit(accessUnit);


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // WRITE TO FILE
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // TODO(Fabian): copy bit_input_stream and bit_output_stream from GABAC to genie, use std::vector instead of DataBlock again,
    // TODO(Fabian): copy output_file and input_file classes from GABAC to genie

    // TODO(Jan): program options, get output file pointer (FILE*)
    //FILE* outputFilePointer = nullptr;
    //writeParametersSet(parameterSet, outputFilePointer);
    //writeDataUnitAccessUnit(accessUnit, false, outputFilePointer);
}


}  // namespace genie