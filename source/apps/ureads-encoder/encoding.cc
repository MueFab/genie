#include "encoding.h"

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "exceptions.h"
#include "fastq-file-reader.h"
#include "fastq-record.h"
#include "genie-gabac-output-stream.h"
#include "log.h"

extern "C" {
#include <format/DataUnits/data-units.h>
#include <format/encoding-parameters.h>
}

#include <gabac/gabac.h>


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

        // uint8_t datasetType = 1;
        // uint8_t alphabetId = 1;
        // uint32_t reads_length = 100;
        // uint8_t number_of_template_segments_minus1 = 1;
        // uint32_t max_au_data_unit_size = 0xffff;
        // bool pos40bits = false;
        // uint8_t qv_depth = 3;
        // uint8_t as_depth = 1;
        // uint8_t numClasses = 2;
        // //Setting the class ids present in the file
        // auto * classIDs_argument = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
        // for(uint8_t class_i=0; class_i<numClasses; class_i++){
        //     classIDs_argument[class_i] = (uint8_t)(class_i+1);
        // }
        // //next variable only for checking
        // auto * classIDs_check = (uint8_t*)malloc(sizeof(uint8_t)*numClasses);
        // for(uint8_t class_i=0; class_i<numClasses; class_i++){
        //     classIDs_check[class_i] = (uint8_t)(class_i+1);
        // }
        // //Setting the read group names
        // uint16_t numGroups = 1;
        // char** rgroupId_argument = (char**)malloc(sizeof(char*)*numGroups);
        // for(uint16_t group_i=0; group_i < numGroups; group_i++){
        //     rgroupId_argument[group_i] = (char*)malloc(sizeof(char)*12);
        //     strncpy(rgroupId_argument[group_i],"testGroupId",12);
        // }
        // //next variable only for checking
        // char** rgroupId_check = (char**)malloc(sizeof(char*)*numGroups);
        // for(uint16_t group_i=0; group_i < numGroups; group_i++){
        //     rgroupId_check[group_i] = (char*)malloc(sizeof(char)*12);
        //     strncpy(rgroupId_check[group_i],"testGroupId",12);
        // }
        // bool splicedReadsFlag = true;
        // bool multipleSignatureFlag = false;
        // uint32_t multipleSignatureBase = 32;
        // uint8_t U_signature_size = 3;
        //
        // //Hard libs to the default qv_coding mode
        // uint8_t *qv_coding_mode = (uint8_t*) malloc(sizeof(uint8_t)*numClasses);
        // for(uint8_t class_i = 0; class_i < numClasses; class_i++){
        //     qv_coding_mode[class_i] = 1;
        // }
        // //Selecting false in qvps means that we use one of the ID preset
        // bool *qvps_flag = (bool*) malloc(sizeof(bool)*numClasses);
        // for(uint8_t class_i = 0; class_i < numClasses; class_i++){
        //     qvps_flag[class_i] = false;
        // }
        // //Instantiation to all nulls
        // Parameter_set_qvpsType **parameter_set_qvps =
        //         (Parameter_set_qvpsType **)calloc(numClasses, sizeof(Parameter_set_qvpsType*));
        //
        // //Selecting the preset ids, in this example we use the class_i as values in order to simplify testing accross
        // //different values, but it does not make sense in a real situation
        // uint8_t *qvps_preset_ID = (uint8_t*) malloc(sizeof(bool)*numClasses);
        // for(uint8_t class_i = 0; class_i < numClasses; class_i++){
        //     qvps_preset_ID[class_i] = class_i;
        // }
        //
        // Encoding_ParametersType* encodingParameters = constructEncodingParametersSingleAlignmentNoComputed(
        //         datasetType,
        //         alphabetId,
        //         reads_length,
        //         number_of_template_segments_minus1,
        //         max_au_data_unit_size,
        //         pos40bits,
        //         qv_depth,
        //         as_depth,
        //         numClasses,
        //         classIDs_argument,
        //         numGroups,
        //         (char**)rgroupId_argument,
        //         splicedReadsFlag,
        //         multipleSignatureFlag,
        //         multipleSignatureBase,
        //         U_signature_size,
        //         qv_coding_mode,
        //         qvps_flag,
        //         parameter_set_qvps,
        //         qvps_preset_ID
        // );
        //
        // //For all descriptors
        // for(uint8_t descriptor_i=0; descriptor_i<18; descriptor_i++){
        //     if(descriptor_i != 11 && descriptor_i != 15){
        //         //General descriptor
        //         if(descriptor_i%2 == 0){
        //             //use this method if the same descriptor configuration must be used accross all classes
        //             //the following call optains a decoder configuration for testing purposes, in a real applicatio
        //             //this must be changed with something more meaningful
        //             DecoderConfigurationTypeCABAC* testingDecoderConf = getTestingDecoderConfigurationTypeCABAC();
        //             setNonClassSpecificDescriptorConfigurationAndEncodingMode(
        //                     encodingParameters,
        //                     descriptor_i,
        //                     testingDecoderConf,
        //                     0
        //             );
        //         }else{
        //             //use this method if the descriptor configuration is different for all classes
        //             for(uint8_t class_i=0; class_i<numClasses; class_i++){
        //                 DecoderConfigurationTypeCABAC* testingDecoderConf = getTestingDecoderConfigurationTypeCABAC();
        //                 setClassSpecificDescriptorConfigurationAndEncodingMode(
        //                         encodingParameters,
        //                         class_i,
        //                         descriptor_i,
        //                         testingDecoderConf,
        //                         0
        //                 );
        //             }
        //         }
        //     }else{
        //         //Token descriptor
        //         Decoder_configuration_tokentype* configurationTokentype = getTestingDecoderConfigurationTokenType();
        //         setNonClassSpecificDescriptorConfigurationAndEncodingMode(
        //                 encodingParameters, descriptor_i, configurationTokentype, 0
        //         );
        //     }
        // }
        //
        //
        //
        // FILE* outputFile = fopen("testFile","wb");
        // ASSERT_TRUE(outputFile != NULL);
        // OutputBitstream outputBitstream;
        // initializeOutputBitstream(&outputBitstream, outputFile);
        //
        // writeEncoding_parameters(encodingParameters, &outputBitstream);
        // writeBuffer(&outputBitstream);
        // fclose(outputFile);
        //
        // FILE* inputFile = fopen("testFile","rb");
        // ASSERT_TRUE(inputFile != NULL);
        // InputBitstream inputBitstream;
        // initializeInputBitstream(&inputBitstream, inputFile);
        // Encoding_ParametersType* encodingParametersType = readEncodingParameters(&inputBitstream);
        //
        // ASSERT_TRUE(encodingParametersType != nullptr);
        //
        //
        // uint8_t datasetType_buffer;
        // uint8_t alphabetId_buffer;
        // uint32_t reads_length_buffer;
        // uint8_t number_of_template_segments_minus1_buffer;
        // uint32_t max_au_data_unit_size_buffer;
        // bool pos40bits_buffer;
        // uint8_t qv_depth_buffer;
        // uint8_t as_depth_buffer;
        // uint8_t numClasses_buffer;
        // uint8_t* classIds_buffer;
        // uint16_t numGroups_buffer;
        // char* rgroupId_buffer;
        // bool splicedReadsFlag_buffer;
        // bool multipleSignatureFlag_buffer;
        // uint32_t multipleSignatureBase_buffer;
        // uint8_t U_signature_size_buffer;
        //
        // EXPECT_EQ(SUCCESS, getDatasetTypeParameters(encodingParametersType, &datasetType_buffer));
        // EXPECT_EQ(datasetType, datasetType_buffer);
        // EXPECT_EQ(SUCCESS, getAlphabetID(encodingParametersType, &alphabetId_buffer));
        // EXPECT_EQ(alphabetId, alphabetId_buffer);
        // EXPECT_EQ(SUCCESS, getReadsLength(encodingParametersType, &reads_length_buffer));
        // EXPECT_EQ(reads_length, reads_length_buffer);
        // EXPECT_EQ(SUCCESS,
        //         getNumberOfTemplateSegmentsMinus1(encodingParametersType, &number_of_template_segments_minus1_buffer)
        // );
        // EXPECT_EQ(number_of_template_segments_minus1, number_of_template_segments_minus1_buffer);
        // EXPECT_EQ(SUCCESS, getMaxAUDataUnitSize(encodingParametersType, &max_au_data_unit_size_buffer));
        // EXPECT_EQ(max_au_data_unit_size, max_au_data_unit_size_buffer);
        // EXPECT_EQ(SUCCESS, getPos40Bits(encodingParametersType, &pos40bits_buffer));
        // EXPECT_EQ(pos40bits, pos40bits_buffer);
        // EXPECT_EQ(SUCCESS, getQVDepth(encodingParametersType, &qv_depth_buffer));
        // EXPECT_EQ(qv_depth, qv_depth_buffer);
        // EXPECT_EQ(SUCCESS, getASDepth(encodingParametersType, &as_depth_buffer));
        // EXPECT_EQ(as_depth, as_depth_buffer);
        // EXPECT_EQ(SUCCESS, getNumClasses(encodingParametersType, &numClasses_buffer));
        // EXPECT_EQ(numClasses, numClasses_buffer);
        // EXPECT_EQ(SUCCESS, getClassIds(encodingParametersType, &numClasses_buffer, &classIds_buffer));
        // EXPECT_EQ(numClasses, numClasses_buffer);
        // for(uint8_t class_i=0; class_i<numClasses; class_i++){
        //     EXPECT_EQ(classIDs_check[class_i],classIds_buffer[class_i]);
        // }
        // EXPECT_EQ(SUCCESS, getNumGroups(encodingParametersType, &numGroups_buffer));
        // EXPECT_EQ(numGroups, numGroups_buffer);
        // for(uint8_t rgroup_i=0; rgroup_i<numGroups; rgroup_i++){
        //     EXPECT_EQ(SUCCESS, getReadGroupId(encodingParametersType, rgroup_i, &rgroupId_buffer));
        //     EXPECT_STREQ(rgroupId_check[rgroup_i], rgroupId_buffer);
        // }
        //
        // EXPECT_EQ(SUCCESS, getSplicedReadsFlag(encodingParametersType, &splicedReadsFlag_buffer));
        // EXPECT_EQ(splicedReadsFlag, splicedReadsFlag_buffer);
        // EXPECT_EQ(SUCCESS, getMultipleAlignments_flag(encodingParametersType, &multipleSignatureFlag_buffer));
        // EXPECT_EQ(multipleSignatureFlag, multipleSignatureFlag_buffer);
        // EXPECT_EQ(SUCCESS, getMultipleSignatureBaseParameters(encodingParametersType, &multipleSignatureBase_buffer));
        // EXPECT_EQ(multipleSignatureBase, multipleSignatureBase_buffer);
        // EXPECT_EQ(SUCCESS, getSignatureSize(encodingParametersType, &U_signature_size_buffer));
        // EXPECT_EQ(U_signature_size, U_signature_size_buffer);
        //
        //
        // DecoderConfigurationTypeCABAC* decoder_configuration_cabac_buffer;
        // EXPECT_EQ(OUT_OF_BOUNDERIES, getCABACDecoderConfiguration(encodingParametersType, 6, 0, &decoder_configuration_cabac_buffer));
        // EXPECT_EQ(OUT_OF_BOUNDERIES, getCABACDecoderConfiguration(encodingParametersType, 0, 19, &decoder_configuration_cabac_buffer));
        // EXPECT_EQ(OUT_OF_BOUNDERIES, getCABACDecoderConfiguration(encodingParametersType, 0, 11, &decoder_configuration_cabac_buffer));
        //
        // EXPECT_EQ(SUCCESS, getCABACDecoderConfiguration(encodingParametersType, 0, 0, &decoder_configuration_cabac_buffer));
        // testDecoderConfigurationCabac(decoder_configuration_cabac_buffer);
        //
        //
        // free(classIDs_check);
        // for(uint16_t group_i=0; group_i < numGroups; group_i++){
        //     free(rgroupId_check[group_i]);
        // }
        // free(rgroupId_check);
        // freeEncodingParameters(encodingParametersType);

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
                BLOCK_PAYLOAD
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
