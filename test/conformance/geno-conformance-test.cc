#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <vector>

#include <codecs/include/mpegg-codecs.h>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"
#include "genie/genotype/ParameterSetComposer.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/core/record/annotation_access_unit/typeddata.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"

#include "helpers.h"


class GenotypeConformanceTest : public ::testing::TestWithParam<std::string> {
 protected:
    // Do any necessary setup for your tests here
    GenotypeConformanceTest() = default;

    ~GenotypeConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};


#define WRITETESTFILES



TEST_P(GenotypeConformanceTest, GenoConformanceTests) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = GetParam();
    std::string filepath = gitRootDir + filename;

    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();
    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 3);

    uint32_t BLOCK_SIZE = 200;
    bool TRANSFORM_MODE = true;

    genie::likelihood::EncodingOptions likelihood_opt = {
    BLOCK_SIZE,       // block_size
    TRANSFORM_MODE,  // transform_flag;
    };

    genie::genotype::EncodingOptions genotype_opt = {
    BLOCK_SIZE,                                   // block_size;
    genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
    genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
    false,                                       // transpose_mat;
    genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
    genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
    genie::core::AlgoID::JBIG                    // codec_ID;
    };


    auto genotypeData = genie::genotype::encode_block(genotype_opt, recs);
    auto likelihoodData = genie::likelihood::encode_block(likelihood_opt, recs);
    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters =
        std::get<genie::genotype::GenotypeParameters>(genotypeData);
    auto datablock = std::get<genie::genotype::EncodingBlock>(genotypeData);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, genotypeParameters,
                                   std::get<genie::likelihood::LikelihoodParameters>(likelihoodData), recs.size());

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);

    //--------------------------------------------------
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributesInfo =
        datablock.attributeInfo;

    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;
    for (auto formatdata : datablock.attributeData) {
        auto& info = attributesInfo[formatdata.first];
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(std::min(BLOCK_SIZE, static_cast<uint32_t>(recs.size())));
        arrayDims.push_back(recs.at(0).getNumSamples());
        arrayDims.push_back(recs.at(0).getFormatCount());
        attributeTDStream[formatdata.first].set(info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
                                                arrayDims);
        attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
        genie::genotype::GenotypePayload genotypePayload(datablock, genotypeParameters);
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
        genotypePayload.write(writer);
    }

    descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
    {
        genie::likelihood::LikelihoodPayload payload(std::get<genie::likelihood::LikelihoodParameters>(likelihoodData),
                                                     std::get<genie::likelihood::EncodingBlock>(likelihoodData));
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
        payload.write(writer);
    }

    // add LINK_ID default values
    for (auto i = 0u; i < BLOCK_SIZE && i < recs.size(); ++i) {
        char val = static_cast<char>(0xFF);
        descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_access_unit::Record annotationAccessUnit;

    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributesInfo, annotationParameterSet,
                                     annotationAccessUnit, AG_class, AT_ID, 0);
    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

#ifdef WRITETESTFILES
    {
        std::string name = filepath + "_output";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            APS_dataUnit.write(writer);
            AAU_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            APS_dataUnit.write(txtWriter);
            AAU_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
#endif
}
INSTANTIATE_TEST_CASE_P(testallGenoConformance, GenotypeConformanceTest,
                        ::testing::Values("/data/records/conformance/1.3.5.bgz.CASE01.geno",
                                          "/data/records/conformance/1.3.11.bgz.CASE03.geno",
                                          "/data/records/conformance/1.3.11.bgz.CASE04.geno"));


