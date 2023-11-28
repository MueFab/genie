/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <codecs/include/mpegg-codecs.h>
#include <gtest/gtest.h>
#include <fstream>
#include <vector>
//#include <xtensor/xmath.hpp>
//#include <xtensor/xoperation.hpp>
//#include <xtensor/xrandom.hpp>
//#include <xtensor/xview.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

//#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
//#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
//#include "genie/genotype/genotype_parameters.h"
//#include "genie/genotype/genotype_payload.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/likelihood/likelihood_coder.h"
// #include "genie/genotype/ParameterSetComposer.h"
//#include "genie/variantsite/AccessUnitComposer.h"

TEST(Likelihood, ParseLikelihood) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gl_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    uint32_t BLOCK_SIZE = 100;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::likelihood::EncodingOptions opt = {
        BLOCK_SIZE,    // block_size
        1,      // transform_flag;
    };

    genie::likelihood::EncodingBlock block{};
    genie::likelihood::extract_likelihoods(opt, block, recs);

    auto& likelihood_mat = block.likelihood_mat;

    ASSERT_EQ(likelihood_mat.dimension(), 2);
    ASSERT_EQ(likelihood_mat.shape(0), BLOCK_SIZE);
    ASSERT_EQ(likelihood_mat.shape(1), 1092 * 3);
    ASSERT_EQ(likelihood_mat(0, 4), 3197737370); // TODO (Yeremia): Check whats the value at this position
    ASSERT_EQ(likelihood_mat(0, 8), 3241567846); // TODO (Yeremia): Check whats the value at this position
}

TEST(Likelihood, RoundTripNoTransform) {

}

TEST(Likelihood, RoundTripTransform) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gl_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    uint32_t BLOCK_SIZE = 100;
    bool TRANSFORM_MODE = true;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::likelihood::EncodingOptions opt = {
        BLOCK_SIZE,     // block_size
        TRANSFORM_MODE, // transform_flag;
    };

    genie::likelihood::EncodingBlock block{};
    auto& likelihood_mat = block.likelihood_mat;
    genie::likelihood::extract_likelihoods(opt, block, recs);

    transform_likelihood_mat(opt, block);
    genie::likelihood::UInt32MatDtype recon_likelihood_mat;

    genie::likelihood::inverse_transform_lut(recon_likelihood_mat, block.lut, block.idx_mat);


    ASSERT_TRUE(block.dtype_id == genie::core::DataType::UINT16);
    ASSERT_TRUE(xt::all(xt::equal(likelihood_mat, recon_likelihood_mat)));
}

TEST(Likelihood, RoundTripEncode) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gl_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    uint32_t BLOCK_SIZE = 100;
    bool TRANSFORM_MODE = true;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::likelihood::EncodingOptions opt = {
        BLOCK_SIZE,     // block_size
        TRANSFORM_MODE, // transform_flag;
    };

    genie::likelihood::EncodingBlock block{};
//    auto& likelihood_mat = block.likelihood_mat;
    genie::likelihood::extract_likelihoods(opt, block, recs);

    transform_likelihood_mat(opt, block);
    genie::likelihood::UInt32MatDtype recon_likelihood_mat;

//    genie::likelihood::inverse_transform_lut(recon_likelihood_mat, block.lut, block.idx_mat);
    std::stringstream serialized_mat;
    std::stringstream serialized_arr;

    genie::likelihood::serialize_mat(
        block.idx_mat,
        block.dtype_id,
        block.nrows,
        block.ncols,
        block.serialized_mat
    );
}