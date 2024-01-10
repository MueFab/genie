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

#include "genie/core/record/variant_genotype/record.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/core/record/annotation_access_unit/typeddata.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"


#include "genie/variantsite/VariantSiteParser.h"
#include "genie/variantsite/ParameterSetComposer.h"

#include "genie/util/string-helpers.h"

namespace util_tests {

std::string exec(const std::string& cmd) {
#ifdef WIN32
    FILE* pipe = _popen(cmd.c_str(), "r");
#else
    FILE* pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe) {
        return "<exec(" + cmd + ") failed>";
    }

    const int bufferSize = 256;
    char buffer[bufferSize];
    std::string result;

    while (!feof(pipe)) {
        if (fgets(buffer, bufferSize, pipe) != nullptr) {
            result += buffer;
        }
    }
#ifdef WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    genie::util::rtrim(result);

    return result;
}

}  // namespace util_tests

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


std::tuple<genie::genotype::GenotypeParameters, genie::genotype::EncodingBlock> genotypeEncoding(
    uint32_t blockSize, std::vector<genie::core::record::VariantGenotype> recs) {
    genie::genotype::EncodingOptions opt = {
        blockSize,                                   // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    return genie::genotype::encode_block(opt, recs);
}
std::tuple<genie::likelihood::LikelihoodParameters, genie::likelihood::EncodingBlock> likelihoodEncoding(
    uint32_t blockSize, std::vector<genie::core::record::VariantGenotype> recs) {
    bool TRANSFORM_MODE = true;
    genie::likelihood::EncodingOptions opt = {
        blockSize,       // block_size
        TRANSFORM_MODE,  // transform_flag;
    };
    genie::likelihood::EncodingBlock block;
    genie::likelihood::extract_likelihoods(opt, block, recs);
    transform_likelihood_mat(opt, block);
    genie::likelihood::UInt32MatDtype recon_likelihood_mat;

    {
        uint32_t nrows = (uint32_t)block.idx_mat.shape(0);
        uint32_t ncols = (uint32_t)block.idx_mat.shape(1);

        if (block.dtype_id == genie::core::DataType::UINT16) {
            for (size_t i = 0; i < nrows; i++) {
                for (size_t j = 0; j < ncols; j++) {
                    EXPECT_LE(block.idx_mat(i, j), block.nelems);
                }
            }
        }
    }

    genie::likelihood::serialize_mat(block.idx_mat, block.dtype_id, block.nrows, block.ncols, block.serialized_mat);
    genie::likelihood::serialize_arr(block.lut, block.nelems, block.serialized_arr);
    block.serialized_mat.seekp(0, std::ios::end);

    genie::entropy::lzma::LZMAEncoder lzmaEncoder;
    std::stringstream compressedData;
    lzmaEncoder.encode(block.serialized_arr, compressedData);

    block.serialized_arr.str("");
    block.serialized_arr << compressedData.rdbuf();
    compressedData.str("");

    lzmaEncoder.encode(block.serialized_mat, compressedData);
    block.serialized_mat.str("");
    block.serialized_mat << compressedData.rdbuf();

    genie::likelihood::LikelihoodParameters parameters(static_cast<uint8_t>(recs.at(0).getNumberOfLikelihoods()),
                                                       TRANSFORM_MODE, block.dtype_id);
    return std::make_tuple(parameters, block);
}

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

    auto genotypeData = genotypeEncoding(BLOCK_SIZE, recs);
    auto likelihoodData = likelihoodEncoding(BLOCK_SIZE, recs);
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
    if (false) {
        std::string name = filepath + "_AAU";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            AAU_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            AAU_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
    if (false) {
        std::string name = filepath + "_APS";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            APS_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            APS_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
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



class SiteConformanceTest : public ::testing::TestWithParam<std::string> {
protected:
    // Do any necessary setup for your tests here
    SiteConformanceTest() = default;

    ~SiteConformanceTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_P(SiteConformanceTest,SiteConformancetests) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filename = GetParam();
    std::string filepath = gitRootDir + filename ;
    std::string infofilename = "/data/records/1.3.5.header100.gt_only.vcf.infotags.json";
    std::ifstream infoFieldsFile;
    infoFieldsFile.open(gitRootDir + infofilename, std::ios::in);
    std::stringstream infoFields;
    if (infoFieldsFile.is_open()) {
        infoFields << infoFieldsFile.rdbuf();
        infoFieldsFile.close();
    }
    std::ifstream inputfile;
    inputfile.open(filepath, std::ios::in | std::ios::binary);
    uint64_t defaultTileSize = 0;
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSize);
    if (inputfile.is_open()) inputfile.close();

    auto info = parser.getAttributes().getInfo();
    auto& tile_descriptorStream = parser.getDescriptors().getTiles();
    auto& tile_attributeStream = parser.getAttributes().getTiles();
    std::vector<genie::core::AnnotDesc> descrList;
    for (auto& tile : tile_descriptorStream) descrList.push_back(tile.first);

    genie::variant_site::ParameterSetComposer encodeParameters;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        encodeParameters.setParameterSet(descrList, info, defaultTileSize);

    uint8_t AG_class = 1;
    uint8_t AT_ID = 0;

    genie::variant_site::AccessUnitComposer accessUnit;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit(parser.getNrOfTiles());
    uint64_t rowIndex = 0;
    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<genie::core::AnnotDesc, std::stringstream> desc;

        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attr;
        for (auto& attrtile : tile_attributeStream) {
            attr[attrtile.first] = attrtile.second.getTypedTile(i);
        }

        accessUnit.setAccessUnit(desc, attr, info, annotationParameterSet, annotationAccessUnit.at(i), AG_class, AT_ID,
            rowIndex);
        rowIndex += defaultTileSize;
    }

    std::ofstream testfile;
    testfile.open(filepath + "_output.bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filepath + "_output.txt", std::ios::out);
    genie::core::Writer txtwriter(&txtfile, true);

    uint8_t tile = 0;
    for (auto& aau : annotationAccessUnit) {
        std::ofstream aaubin;
        std::ofstream aautxt;
        genie::core::record::data_unit::Record AAU_dataUnit(aau);
        AAU_dataUnit.write(testwriter);
        AAU_dataUnit.write(txtwriter);
         tile++;
    }
    testfile.close();
    txtfile.close();
}

INSTANTIATE_TEST_CASE_P(testallsiteConformance, SiteConformanceTest,
    ::testing::Values("/data/records/conformance/1.3.5.bgz.CASE01.site",
        "/data/records/conformance/1.3.11.bgz.CASE03.site",
        "/data/records/conformance/1.3.11.bgz.CASE04.site"));