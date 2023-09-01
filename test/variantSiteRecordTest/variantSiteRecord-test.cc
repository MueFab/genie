#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include "codecs/include/mpegg-codecs.h"
#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/variant_site/VariantSiteParser.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/util/bitreader.h"

class VariantSiteRecordTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    VariantSiteRecordTests() = default;

    ~VariantSiteRecordTests() override = default;

    // Use SetUp instead of the constructor in the following cases:
    // - In the body of a constructor (or destructor), it's not possible to
    //   use the ASSERT_xx macros. Therefore, if the set-up operation could
    //   cause a fatal test failure that should prevent the test from running,
    //   it's necessary to use a CHECK macro or to use SetUp() instead of a
    //   constructor.
    // - If the tear-down operation could throw an exception, you must use
    //   TearDown() as opposed to the destructor, as throwing in a destructor
    //   leads to undefined behavior and usually will kill your program right
    //   away. Note that many standard libraries (like STL) may throw when
    //   exceptions are enabled in the compiler. Therefore you should prefer
    //   TearDown() if you want to write portable tests that work with or
    //   without exceptions.
    // - The googletest team is considering making the assertion macros throw
    //   on platforms where exceptions are enabled (e.g. Windows, Mac OS, and
    //   Linux client-side), which will eliminate the need for the user to
    //   propagate failures from a subroutine to its caller. Therefore, you
    //   shouldn't use googletest assertions in a destructor if your code
    //   could run on such a platform.
    // - In a constructor or destructor, you cannot make a virtual function
    //   call on this object. (You can call a method declared as virtual, but
    //   it will be statically bound.) Therefore, if you need to call a method
    //   that will be overridden in a derived class, you have to use
    //   SetUp()/TearDown().

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(VariantSiteRecordTests, readFilefrombin) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    std::string path = "./Testfiles/exampleMGrecs/";
    std::string filename = "ALL.chrX.10000";
    std::ifstream inputfile;
    inputfile.open(path + filename + ".site", std::ios::in | std::ios::binary);

    if (inputfile.is_open()) {
        genie::util::BitReader reader(inputfile);
        std::ofstream outputfile(path + filename + "_site.txt");
        do {
            genie::core::record::variant_site::Record variant_site_record(reader);
            variant_site_record.write(outputfile);
        } while (inputfile.peek() != EOF);
        inputfile.close();
        outputfile.close();
    }

    EXPECT_EQ(0, 0);
}
TEST_F(VariantSiteRecordTests, fixedValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    uint64_t variant_index = 1;
    uint16_t seq_ID = 1;
    uint64_t pos = 1;
    uint8_t strand = 1;
    uint8_t ID_len = 2;
    std::string ID = "UH";
    uint8_t description_len = 3;
    std::string description = "Poe";
    uint32_t ref_len = 1;
    std::string ref = "K";
    uint8_t alt_count = 5;

    std::vector<uint32_t> alt_len{3, 2, 5, 1, 2};
    std::vector<std::string> altern{"TCC", "AG", "TCGNT", "G", "NN"};

    uint32_t depth = 1;
    uint32_t seq_qual = 13241;
    uint32_t map_qual = 0;
    uint32_t map_num_qual_0 = 1313;
    uint8_t filters_len = 0;
    std::string filters = "";
    uint8_t info_count = 3;
    std::vector<genie::core::record::variant_site::Info_tag> info_tag;
    genie::core::record::variant_site::Info_tag infoTag;
    infoTag.info_array_len = 1;
    infoTag.info_tag = "AA";
    infoTag.info_tag_len = 2;
    infoTag.info_type = genie::core::DataType::CHAR;  // char
    infoTag.infoValue.resize(infoTag.info_array_len);
    infoTag.infoValue[0].push_back(63);
    info_tag.push_back(infoTag);

    infoTag.info_tag = "CD";
    infoTag.info_type = genie::core::DataType::UINT16;  // unsigned int
    infoTag.infoValue[0][0] = 24;
    infoTag.infoValue[0].push_back(0);
    info_tag.push_back(infoTag);

    infoTag.info_tag = "EF";
    infoTag.info_type = genie::core::DataType::STRING;  // string
    infoTag.infoValue[0][1] = 'A';
    infoTag.infoValue[0][1] = 'B';
    infoTag.infoValue[0].push_back('C');
    info_tag.push_back(infoTag);

    uint8_t linked_record = 0;
    uint8_t link_name_len = 0;
    std::string link_name = "";
    uint8_t reference_box_ID = 0;
    genie::core::record::variant_site::Record variant_site_record(
        variant_index, seq_ID, pos, strand, ID_len, ID, description_len, description, ref_len, ref, alt_count, alt_len,
        altern, depth, seq_qual, map_qual, map_num_qual_0, filters_len, filters, info_count, info_tag, linked_record,
        link_name_len, link_name, reference_box_ID);

    std::stringstream output;
    genie::core::Writer writer(&output);
    variant_site_record.write(writer);
    genie::util::BitReader reader(output);
    genie::core::record::variant_site::Record variant_site_record_check(reader);
    std::stringstream checkOut;
    genie::core::Writer writerCheck(&checkOut);
    variant_site_record_check.write(writerCheck);

    EXPECT_EQ(variant_site_record.getAlt(), variant_site_record_check.getAlt());
    EXPECT_EQ(variant_site_record.getInfoCount(), variant_site_record_check.getInfoCount());
    for (auto i = 0; i < variant_site_record.getInfoCount(); ++i) {
        EXPECT_EQ(variant_site_record.getInfoTag()[i].info_type, variant_site_record_check.getInfoTag()[i].info_type);
        EXPECT_EQ(variant_site_record.getInfoTag()[i].infoValue, variant_site_record_check.getInfoTag()[i].infoValue)
            << " index: " << std::to_string(i) << "\n";
        EXPECT_EQ(variant_site_record.getInfoTag()[i].info_array_len,
                  variant_site_record_check.getInfoTag()[i].info_array_len);
        EXPECT_EQ(variant_site_record.getInfoTag()[i].info_tag, variant_site_record_check.getInfoTag()[i].info_tag);
    }
    EXPECT_EQ(output.str(), checkOut.str());
}

class ParameterSetEncoder {
 public:
    genie::core::record::annotation_parameter_set::Record setParameterSet(
        std::map<genie::core::AnnotDesc, std::stringstream>& encodedDescriptors,
        std::map<std::string, genie::core::record::variant_site::AttributeData>& info) {
        //----------------------------------------------------//
        // default values
        bool attributeContiguity = false;
        bool twoDimensional = false;
        bool columnMajorTileOrder = false;
        uint8_t ATCoordSize = 3;
        bool variable_size_tiles = false;
        uint8_t AG_class = 1;
        uint8_t AT_ID = 0;
        uint8_t parameter_set_ID = 1;
        uint8_t AT_alphabet_ID = 0;
        bool AT_pos_40_bits_flag = false;
        uint8_t n_aux_attribute_groups = 0;
        bool attribute_dependent_tiles = false;
        uint16_t n_add_tile_structures = 0;
        bool symmetry_mode = false;
        bool symmetry_minor_diagonal = false;
        genie::core::AlgoID encoding_mode_ID = genie::core::AlgoID::BSC;

        uint64_t n_tiles = 1;
        std::vector<std::vector<uint64_t>> start_index;
        std::vector<std::vector<uint64_t>> end_index;
        std::vector<uint64_t> tile_size(1, 10000);

        std::vector<uint16_t> nAttributes;
        std::vector<std::vector<uint16_t>> attribute_IDs;
        std::vector<uint8_t> nDescriptors;
        std::vector<std::vector<uint8_t>> descriptor_IDs;
        genie::core::record::annotation_parameter_set::TileStructure default_tile_structure(
            ATCoordSize, twoDimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
        std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;
        std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configurations;

        genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration(
            ATCoordSize, AG_class, attributeContiguity, twoDimensional, columnMajorTileOrder, symmetry_mode,
            symmetry_minor_diagonal, attribute_dependent_tiles, default_tile_structure, n_add_tile_structures,
            nAttributes, attribute_IDs, nDescriptors, descriptor_IDs, additional_tile_structure);
        tile_configurations.push_back(tileConfiguration);

        uint8_t n_filter = 0;
        std::vector<uint8_t> filter_ID_len;
        std::vector<std::string> filter_ID;
        std::vector<uint16_t> desc_len;
        std::vector<std::string> description;

        uint8_t n_features_names = 0;
        std::vector<uint8_t> feature_name_len;
        std::vector<std::string> feature_name;

        uint8_t n_ontology_terms = 0;
        std::vector<uint8_t> ontology_term_name_len;
        std::vector<std::string> ontology_term_name;

        genie::entropy::bsc::BSCParameters bscParameters(MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN,
                                                         MPEGG_BSC_BLOCKSORTER_BWT, MPEGG_BSC_CODER_QLFC_STATIC);
        genie::entropy::zstd::ZSTDParameters zstdParameters;
        genie::entropy::lzma::LZMAParameters lzmaParameters;

        auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();
        auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();
        auto ZSTDalgorithmParameters = zstdParameters.convertToAlgorithmParameters();

        uint8_t n_compressors = 3;
        uint8_t compressor_ID = 1;
        uint8_t n_compressor_steps = 1;
        std::vector<uint8_t> compressor_step_ID{0};
        std::vector<genie::core::AlgoID> BSCalgorithm_ID{genie::core::AlgoID::BSC};
        std::vector<genie::core::AlgoID> LZMAalgorithm_ID{genie::core::AlgoID::LZMA};
        std::vector<genie::core::AlgoID> ZSTDalgorithm_ID{genie::core::AlgoID::ZSTD};
        std::vector<bool> use_default_pars{true};
        std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
        std::vector<uint8_t> n_in_vars{0};
        std::vector<std::vector<uint8_t>> in_var_ID{{0}};
        std::vector<std::vector<uint8_t>> prev_step_ID;
        std::vector<std::vector<uint8_t>> prev_out_var_ID;
        std::vector<uint8_t> n_completed_out_vars{0};
        std::vector<std::vector<uint8_t>> completed_out_var_ID;

        genie::core::record::annotation_parameter_set::CompressorParameterSet BSCcompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, BSCalgorithm_ID, use_default_pars,
            algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
            completed_out_var_ID);
        compressor_ID++;
        genie::core::record::annotation_parameter_set::CompressorParameterSet LZMAcompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, LZMAalgorithm_ID, use_default_pars,
            algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
            completed_out_var_ID);
        compressor_ID++;
        genie::core::record::annotation_parameter_set::CompressorParameterSet ZSTDcompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, ZSTDalgorithm_ID, use_default_pars,
            algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
            completed_out_var_ID);
        std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
            BSCcompressorParameterSet, LZMAcompressorParameterSet, ZSTDcompressorParameterSet};

        genie::core::record::annotation_parameter_set::GenotypeParameters genotype_parameters;
        genie::core::record::annotation_parameter_set::LikelihoodParameters likelihood_parameters;
        genie::core::record::annotation_parameter_set::ContactMatrixParameters contact_matrix_parameters;

        std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration;
        uint8_t n_descriptors = static_cast<uint8_t>(encodedDescriptors.size());
        for (auto it = encodedDescriptors.begin(); it != encodedDescriptors.end(); ++it) {
            genie::core::AnnotDesc DescrID = it->first;
            if (DescrID == genie::core::AnnotDesc::STRAND)
                encoding_mode_ID = genie::core::AlgoID::LZMA;
            else if (DescrID == genie::core::AnnotDesc::REFERENCE)
                encoding_mode_ID = genie::core::AlgoID::ZSTD;
            else
                encoding_mode_ID = genie::core::AlgoID::BSC;

        //    encoding_mode_ID = genie::core::AlgoID::BSC;

            genie::core::record::annotation_parameter_set::DescriptorConfiguration descrConf(
                DescrID, encoding_mode_ID, genotype_parameters, likelihood_parameters, contact_matrix_parameters,
                BSCalgorithmParameters);

            descriptor_configuration.push_back(descrConf);
        }

        std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set;
        uint8_t n_attributes = static_cast<uint8_t>(info.size());
        for (auto it = info.begin(); it != info.end(); ++it) {
            genie::core::arrayType deftype;
            uint16_t attrID = info[it->first].getAttributeID();
            std::string attribute_name = info[it->first].getAttributeName();
            uint8_t attribute_name_len = static_cast<uint8_t>(attribute_name.length());
            genie::core::DataType attribute_type = info[it->first].getAttributeType();
            uint8_t attribute_num_array_dims = info[it->first].getArrayLength();
            std::vector<uint8_t> attribute_array_dims;
            if (info[it->first].getArrayLength() == 1) {
                attribute_num_array_dims = 0;
            } else {
                attribute_num_array_dims = 1;
                attribute_array_dims.push_back(info[it->first].getArrayLength());
            }
            std::vector<uint8_t> attribute_default_val =
                deftype.toArray(attribute_type, deftype.getDefaultValue(attribute_type));
            bool attribute_miss_val_flag = true;
            bool attribute_miss_default_flag = false;
            std::vector<uint8_t> attribute_miss_val = attribute_default_val;
            std::string attribute_miss_str = "";
            uint8_t n_steps_with_dependencies = 0;
            std::vector<uint8_t> dependency_step_ID;
            std::vector<uint8_t> n_dependencies;
            std::vector<std::vector<uint8_t>> dependency_var_ID;
            std::vector<std::vector<bool>> dependency_is_attribute;
            std::vector<std::vector<uint16_t>> dependency_ID;

            genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet(
                attrID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
                attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
                attribute_miss_val, attribute_miss_str, 1, n_steps_with_dependencies, dependency_step_ID,
                n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);
            attribute_parameter_set.push_back(attributeParameterSet);
        }

        genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters(
            n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
            n_ontology_terms, ontology_term_name_len, ontology_term_name, n_descriptors, descriptor_configuration,
            n_compressors, compressor_parameter_set, n_attributes, attribute_parameter_set);

        genie::core::record::annotation_parameter_set::Record annotationParameterSet(
            parameter_set_ID, AT_ID, AT_alphabet_ID, ATCoordSize, AT_pos_40_bits_flag, n_aux_attribute_groups,
            tile_configurations, annotation_encoding_parameters);
        return annotationParameterSet;
    }
};

#define COMPRESSED true

TEST_F(VariantSiteRecordTests, readFileRunParser) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    using DescriptorID = genie::core::AnnotDesc;
    std::string path = "./Testfiles/exampleMGrecs/";
    std::string filename = "ALL.chrX.10000";
    std::ifstream inputfile;
    inputfile.open(path + filename + ".site", std::ios::in | std::ios::binary);

    filename = "ALL.chrX.phase1_release_v3.20101123.snps_indels_svs.genotypes.vcf.infotags.json";
    std::ifstream infoFieldsFile;
    infoFieldsFile.open(path + filename, std::ios::in);
    std::stringstream infoFields;
    if (infoFieldsFile.is_open()) {
        infoFields << infoFieldsFile.rdbuf();
        infoFieldsFile.close();
    }
    std::map<DescriptorID, std::stringstream> outputstream;
    std::map<std::string, genie::core::record::variant_site::AttributeData> info;
    std::map<std::string, std::stringstream> attributeStream;
    genie::core::record::variant_site::VariantSiteParser parser(inputfile, outputstream, info, attributeStream,
                                                                infoFields);
    if (inputfile.is_open()) {
        EXPECT_GE(outputstream[DescriptorID::ALTERN].str().size(), 52);
        EXPECT_EQ(parser.getNumberOfRows(), 10000);
        inputfile.close();
    }

    EXPECT_EQ(attributeStream.size(), 22);
    for (auto it = attributeStream.begin(); it != attributeStream.end(); ++it)
        EXPECT_GE(it->second.str().size(), 10000) << it->first;

    ParameterSetEncoder encodeParameters;

    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(outputstream, info);

    //----------------------------------------------------//
#if COMPRESSED

    std::map<DescriptorID, std::stringstream> encodedDescriptors;
    genie::entropy::bsc::BSCEncoder bscEncoder;
    genie::entropy::lzma::LZMAEncoder lzmaEncoder;
    genie::entropy::zstd::ZSTDEncoder zstdEncoder;
    const auto& temp = annotationParameterSet.getAnnotationEncodingParameters().getDescriptorConfigurations();
    for (auto encodingpar : temp) {
        switch (encodingpar.getEncodingModeID()) {
            case genie::core::AlgoID::BSC:
                bscEncoder.encode(outputstream[encodingpar.getDescriptorID()],
                                  encodedDescriptors[encodingpar.getDescriptorID()]);
                break;
            case genie::core::AlgoID::LZMA:
                lzmaEncoder.encode(outputstream[encodingpar.getDescriptorID()],
                                   encodedDescriptors[encodingpar.getDescriptorID()]);
                break;
            case genie::core::AlgoID::ZSTD:
                zstdEncoder.encode(outputstream[encodingpar.getDescriptorID()],
                                   encodedDescriptors[encodingpar.getDescriptorID()]);
                break;

            default:
                break;
        }
        if (encodingpar.getDescriptorID() == genie::core::AnnotDesc::STRAND) {
            std::cerr << "+";
        }
    }

    std::map<std::string, std::stringstream> encodedAttributes;
    for (auto it = attributeStream.begin(); it != attributeStream.end(); ++it) {
        bscEncoder.encode(it->second, encodedAttributes[it->first]);
        std::stringstream check;
        bscEncoder.decode(encodedAttributes[it->first], check);
        EXPECT_EQ(check.str(), it->second.str());
    }
#endif

    //----------------------------------------------------//
    // default values
    bool attributeContiguity = false;
    bool twoDimensional = false;
    bool columnMajorTileOrder = false;
    uint8_t ATCoordSize = 3;
    bool variable_size_tiles = false;
    uint8_t AT_subtype = 1;
    uint8_t AG_class = 1;
    uint64_t n_tiles_per_col = 1;
    uint64_t n_tiles_per_row = 1;
    uint64_t tile_index_1 = 0;
    bool tile_index_2_exists = false;
    uint64_t tile_index_2 = 0;
    bool is_attribute = false;
    uint16_t attribute_ID = 0;
    auto descriptor_ID = genie::core::AnnotDesc::STARTPOS;
    uint8_t AT_ID = 0;
    genie::core::record::annotation_access_unit::AnnotationType AT_type =
        genie::core::record::annotation_access_unit::AnnotationType::VARIANTS;
    uint8_t numChrs = 0;

    //----------------------------------------------------//
    {
        std::vector<genie::core::record::annotation_access_unit::Block> blocks;
#if COMPRESSED
        for (auto it = encodedDescriptors.begin(); it != encodedDescriptors.end(); ++it) {
#else
        for (auto it = outputstream.begin(); it != outputstream.end(); ++it) {
#endif
            std::vector<uint8_t> data;
            for (auto readbyte : it->second.str()) data.push_back(readbyte);
            genie::core::record::annotation_access_unit::BlockVectorData blockInfo(it->first, 0, data);
            genie::core::record::annotation_access_unit::Block blockn;
            blockn.set(blockInfo);

            blocks.push_back(blockn);
        }

#if COMPRESSED
        for (auto it = encodedAttributes.begin(); it != encodedAttributes.end(); ++it) {
#else
        for (auto it = attributeStream.begin(); it != attributeStream.end(); ++it) {
#endif
            auto attributeID = info[it->first].getAttributeID();
            std::vector<uint8_t> data;
            for (auto readbyte : it->second.str()) data.push_back(readbyte);
            genie::core::record::annotation_access_unit::BlockVectorData blockInfo(DescriptorID::ATTRIBUTE, attributeID,
                                                                                   data);
            genie::core::record::annotation_access_unit::Block blockn;
            blockn.set(blockInfo);
            blocks.push_back(blockn);
        }
        uint64_t n_blocks = static_cast<uint64_t>(blocks.size());
        genie::core::record::annotation_access_unit::AnnotationAccessUnitHeader annotationAccessUnitHeadertot(
            attributeContiguity, twoDimensional, columnMajorTileOrder, variable_size_tiles, ATCoordSize, is_attribute,
            attribute_ID, descriptor_ID, n_tiles_per_col, n_tiles_per_row, n_blocks, tile_index_1, tile_index_2_exists,
            tile_index_2);
        genie::core::record::annotation_access_unit::Record annotationAccessUnittot(
            AT_ID, AT_type, AT_subtype, AG_class, annotationAccessUnitHeadertot, blocks, attributeContiguity,
            twoDimensional, columnMajorTileOrder, ATCoordSize, variable_size_tiles, n_blocks, numChrs);

#if COMPRESSED
        std::string subname = "Compressed";
#else
        std::string subname = "NotCompressed";
#endif
        std::string name = "TestFiles/Complete" + subname;

        auto apsByteSize = annotationParameterSet.getSize() / 8;
        auto aauByteSize = annotationAccessUnittot.getSize() / 8;

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);

            writer.write(3, 8);
            writer.write_reserved(10);
            writer.write(apsByteSize+5, 22);
            annotationParameterSet.write(writer);

            writer.write(4, 8);
            writer.write_reserved(3);
            writer.write(aauByteSize+5, 29);
            annotationAccessUnittot.write(writer);
            testfile.close();
        }
        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);

            txtWriter.write(3, 8);
            txtWriter.write_reserved(10);
            txtWriter.write(apsByteSize+5, 22);
            annotationParameterSet.write(txtWriter);

            txtWriter.write(4, 8);
            txtWriter.write_reserved(3);
            txtWriter.write(aauByteSize+5, 29);
            annotationAccessUnittot.write(txtWriter);
            txtfile.close();
        }
    }
}
