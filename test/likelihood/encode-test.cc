/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
//#include <xtensor/xmath.hpp>
//#include <xtensor/xoperation.hpp>
//#include <xtensor/xrandom.hpp>
//#include <xtensor/xview.hpp>
#include <codecs/include/mpegg-codecs.h>
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/likelihood/likelihood_coder.h"

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

TEST(Likelihood, RoundTripTransformEncode) {
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
    genie::likelihood::extract_likelihoods(opt, block, recs);

    transform_likelihood_mat(opt, block);
    genie::likelihood::UInt32MatDtype recon_likelihood_mat;

    genie::likelihood::serialize_mat(
        block.idx_mat,
        block.dtype_id,
        block.nrows,
        block.ncols,
        block.serialized_mat
    );

    genie::likelihood::serialize_arr(
        block.lut,
        block.nelems,
        block.serialized_arr
    );

    block.serialized_mat.seekp(0, std::ios::end);
    ASSERT_EQ(
        (size_t) block.serialized_mat.tellp(),
        block.nrows * block.ncols * 2
    );

    block.serialized_arr.seekp(0, std::ios::end);
    ASSERT_EQ(
        (size_t) block.serialized_arr.tellp(),
        block.lut.size() * 4
    );

    const std::string& serialized_arr_str = block.serialized_arr.str();
    size_t serialized_arr_len = (size_t) block.serialized_arr.tellp();
    auto* serialized_arr_payload = (unsigned char*)calloc(serialized_arr_len, sizeof(unsigned char));
    auto* serialized_arr_ptr = serialized_arr_str.c_str();
    std::memcpy(serialized_arr_payload, serialized_arr_ptr, serialized_arr_len);

    uint8_t* compressed_data;
    size_t compressed_data_len;

    mpegg_lzma_compress_default(
        &compressed_data,
        &compressed_data_len,
        serialized_arr_payload,
        serialized_arr_len
    );

    uint8_t* recon_data;
    size_t recon_data_len;

    mpegg_lzma_decompress(
        &recon_data,
        &recon_data_len,
        compressed_data,
        compressed_data_len
    );

    ASSERT_EQ(serialized_arr_len, recon_data_len);
    for (size_t i = 0; i < recon_data_len; i++) {
        ASSERT_EQ(
            (uint8_t) *(serialized_arr_payload + i),
            (uint8_t) *(recon_data + i)
        ) << "Index " << i;
    }

}