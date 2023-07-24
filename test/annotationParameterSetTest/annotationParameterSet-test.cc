#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include "RandomRecordFillIn.h"
#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#define GENERATE_TEST_FILES false

class AnnotationParameterSetTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AnnotationParameterSetTests() = default;

    ~AnnotationParameterSetTests() override = default;

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
        srand(static_cast<unsigned int>(time(NULL)));
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(AnnotationParameterSetTests, annotationParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::Record record;

    EXPECT_EQ(record.getParameterSetID(), (uint8_t)0);
    EXPECT_EQ(record.getATID(), 0);
    EXPECT_EQ(record.getATAlphbetID(), 0);
    EXPECT_EQ(record.getATCoordSize(), 0);
    EXPECT_FALSE(record.isATPos$0Bits());
    EXPECT_EQ(record.getNumberOfAuxAttributeGroups(), 0);
}

TEST_F(AnnotationParameterSetTests, AnnotationParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSetCheck;

    annotationParameterSet = RandomContactMatrixParameters.randomAnnotationParameterSet();

    std::stringstream InOut;
    genie::core::Writer strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    annotationParameterSet.write(strwriter);
    strwriter.flush();
    annotationParameterSetCheck.read(strreader);
    std::stringstream testOut;
    genie::core::Writer teststrwriter(&testOut);
    annotationParameterSetCheck.write(teststrwriter);

    EXPECT_EQ(annotationParameterSet.getATAlphbetID(), annotationParameterSetCheck.getATAlphbetID());
    EXPECT_EQ(annotationParameterSet.getATID(), annotationParameterSetCheck.getATID());
    EXPECT_EQ(annotationParameterSet.getATAlphbetID(), annotationParameterSetCheck.getATAlphbetID());
    EXPECT_EQ(annotationParameterSet.getATCoordSize(), annotationParameterSetCheck.getATCoordSize());
    EXPECT_EQ(annotationParameterSet.isATPos$0Bits(), annotationParameterSetCheck.isATPos$0Bits());

    EXPECT_EQ(annotationParameterSet.getTileConfigurations().size(),
              annotationParameterSetCheck.getTileConfigurations().size());

    EXPECT_EQ(InOut.str(), testOut.str());
    auto size = annotationParameterSet.getSize();
    if (size % 8 != 0) size += (8 - size % 8);
    EXPECT_EQ(InOut.str().size(), size / 8);

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/AnnotationParameterSet_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::core::Writer writer(&outputfile);
        annotationParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        genie::core::Writer txtWriter(&txtfile, true);
        annotationParameterSet.write(txtWriter);
        txtfile.close();
    }
#endif
}

TEST_F(AnnotationParameterSetTests, annotationParameterSetForvariantSite) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    // annotation_parameter_set parameters
    uint8_t parameter_set_ID = 1;
    uint8_t AT_ID = 1;
    uint8_t AT_alphabet_ID = 1;
    uint8_t AT_coord_size = 3;
    bool AT_pos_40_bits_flag = false;
    uint8_t n_aux_attribute_groups = 0;
    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tileConfigurationArray;
    {
        // default_tile_structure parameters
        bool variable_size_tiles = false;
        uint64_t n_tiles = 15;
        std::vector<std::vector<uint64_t>> start_index;
        std::vector<std::vector<uint64_t>> end_index;
        std::vector<uint64_t> tile_size;
        for (auto i = 0; i < n_tiles; ++i) tile_size.push_back(static_cast<uint64_t>(i) * 1111);
        bool two_dimensional = false;
        genie::core::record::annotation_parameter_set::TileStructure default_tile_structure(
            AT_coord_size, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);

        // tile configuration parameters
        uint8_t AG_class = 1;
        bool attribute_contiguity = false;
        bool column_major_tile_order = false;
        uint8_t symmetry_mode = false;
        bool symmetry_minor_diagonal = false;

        bool attribute_dependent_tiles = false;

        uint16_t n_add_tile_structures = 0;
        std::vector<uint16_t> n_attributes;
        std::vector<std::vector<uint16_t>> attribute_ID;
        std::vector<uint8_t> n_descriptors;
        std::vector<std::vector<uint8_t>> descriptor_ID;
        std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;

        genie::core::record::annotation_parameter_set::TileConfiguration tile_configuration(
            AT_coord_size, AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
            symmetry_minor_diagonal, attribute_dependent_tiles, default_tile_structure, n_add_tile_structures,
            n_attributes, attribute_ID, n_descriptors, descriptor_ID, additional_tile_structure);

        for (auto i = 0; i <= n_aux_attribute_groups; ++i) tileConfigurationArray.push_back(tile_configuration);
    }

    // annotation_encoding_parameters
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

    uint8_t n_descriptors = 15;
    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration;
    uint8_t n_compressors = 0;
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set;

    uint8_t n_attributes = 20;
    std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set;

    using DescriptorID = genie::core::record::annotation_parameter_set::DescriptorID;

    // descriptor_configuration parameters
    std::vector<DescriptorID> descriptorIDlist{
        DescriptorID::SEQUENCEID,  DescriptorID::STARTPOS,   DescriptorID::STRAND,         DescriptorID::NAME,
        DescriptorID::DESCRIPTION, DescriptorID::REFERENCE,  DescriptorID::ALTERN,         DescriptorID::DEPTH,
        DescriptorID::SEQQUALITY,  DescriptorID::MAPQUALITY, DescriptorID::MAPNUMQUALITY0, DescriptorID::FILTER,
        DescriptorID::LINKID,      DescriptorID::LINKNAME,   DescriptorID::ATTRIBUTE};
    for (auto item : descriptorIDlist) {
        DescriptorID descriptor_ID = item;
        uint8_t encoding_mode_ID = 3;
        genie::core::record::annotation_parameter_set::GenotypeParameters genotype_parameters;
        genie::core::record::annotation_parameter_set::LikelihoodParameters likelihood_parameters;
        genie::core::record::annotation_parameter_set::ContactMatrixParameters contact_matrix_parameters;

        uint8_t n_pars = 4;
        std::vector<uint8_t> par_ID{1, 2, 3, 4};
        std::vector<uint8_t> par_type{4, 4, 4, 4};
        std::vector<uint8_t> par_num_array_dims{0, 0, 0, 0};
        std::vector<std::vector<uint8_t>> par_array_dims;
        // std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val{
        //    {{{{0}}}}, {{{{1}}}}, {{{{2}}}}, {{{{3}}}}};
        std::vector<uint8_t> val1{10};
        std::vector<uint8_t> val2{11};
        std::vector<uint8_t> val3{12};
        std::vector<uint8_t> val4{13};
        std::vector<std::vector<uint8_t>> val11{val1, val2, val3, val4};
        std::vector<std::vector<std::vector<uint8_t>>> val12(1, val11);
        std::vector<std::vector<std::vector<std::vector<uint8_t>>>> val13(1, val12);
        std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(1, val13);

        genie::core::record::annotation_parameter_set::AlgorithmParameters algorithm_patarmeters(
            n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

        genie::core::record::annotation_parameter_set::DescriptorConfiguration descriptor(
            descriptor_ID, encoding_mode_ID, genotype_parameters, likelihood_parameters, contact_matrix_parameters,
            algorithm_patarmeters);
        descriptor_configuration.push_back(descriptor);
    }

    // attribute_parameter_set parrameters
    std::map<std::string, int> infoName{{"LDAF", 11}, {"AVGPOST", 11}, {"RSQ", 11},    {"ERATE", 11},  {"THETA", 11},
                                        {"CIEND", 7}, {"CIPOS", 7},    {"END", 7},     {"HOMLEN", 7},  {"HOMSEQ", 0},
                                        {"SVLEN", 7}, {"SVTYPE", 0},   {"AC", 7},      {"AN", 7},      {"AA", 0},
                                        {"AF", 11},   {"AMR_AF", 11},  {"ASN_AF", 11}, {"AFR_AF", 11}, {"EUR_AF", 11},
                                        {"VT", 0},    {"SNPSOURCE", 0}};
    uint16_t ID = 0;

    for (auto itemName : infoName) {
        uint16_t attribute_ID = ID;
        ID++;
        uint8_t attribute_name_len = static_cast<uint8_t>(itemName.first.length());
        std::string attribute_name = itemName.first;
        uint8_t attribute_type = static_cast<uint8_t>(itemName.second);
        uint8_t attribute_num_array_dims = 0;
        std::vector<uint8_t> attribute_array_dims;

        genie::core::arrayType typeval;

        std::vector<uint8_t> attribute_default_val;
        if (attribute_type != 0)
            attribute_default_val.resize((static_cast<size_t>(typeval.getDefaultBitsize(attribute_type)) + 7) / 8);

        bool attribute_miss_val_flag = true;
        bool attribute_miss_default_flag = false;
        genie::core::arrayType def;
        uint64_t defaultValue = def.getDefaultValue(static_cast<uint8_t>(itemName.second));
        std::vector<uint8_t> attribute_miss_val;
        auto size = def.getDefaultBitsize(static_cast<uint8_t>(itemName.second));
        while (size > 0) {
            uint8_t value = static_cast<uint8_t>(defaultValue & 0xFF);
            attribute_miss_val.push_back(value);
            defaultValue = defaultValue >> 8;
            size -= 8;
        }

        std::string attribute_miss_str;
        uint8_t compressor_ID = 0;

        uint8_t n_steps_with_dependencies = 0;
        std::vector<uint8_t> dependency_step_ID;
        std::vector<uint8_t> n_dependencies;
        std::vector<std::vector<uint8_t>> dependency_var_ID;
        std::vector<std::vector<bool>> dependency_is_attribute;
        std::vector<std::vector<uint16_t>> dependency_ID;

        genie::core::record::annotation_parameter_set::AttributeParameterSet attributeSet(
            attribute_ID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
            attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
            attribute_miss_val, attribute_miss_str, compressor_ID, n_steps_with_dependencies, dependency_step_ID,
            n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);
        attribute_parameter_set.push_back(attributeSet);
    }

    genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters(
        n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
        n_ontology_terms, ontology_term_name_len, ontology_term_name, n_descriptors, descriptor_configuration,
        n_compressors, compressor_parameter_set, n_attributes, attribute_parameter_set);

    genie::core::record::annotation_parameter_set::Record annotationParameterSet(
        parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size, AT_pos_40_bits_flag, n_aux_attribute_groups,
        tileConfigurationArray, annotation_encoding_parameters);

#if 1
    std::string name = "TestFiles/AnnotationParameterSet_site";

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::core::Writer writer(&outputfile);
        annotationParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        genie::core::Writer txtWriter(&txtfile, true);
        annotationParameterSet.write(txtWriter);
        txtfile.close();
    }

    genie::core::record::annotation_parameter_set::Record annotationParameterSetCheck;
    std::ifstream inputfile;
    inputfile.open(name + ".bin", std::ios::binary);
    if (inputfile.is_open()) {
        genie::util::BitReader reader(inputfile);
        annotationParameterSetCheck.read(reader);
        inputfile.close();

        EXPECT_EQ(annotationParameterSet.getATCoordSize(), annotationParameterSetCheck.getATCoordSize());
    }
#endif
}
