/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <iostream>

#ifdef GENIE_LIKELIHOOD_BACKEND_XTENSOR
#include <xtensor/xmath.hpp>
#include <xtensor/xoperation.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xview.hpp>
#endif

#include <codecs/include/mpegg-codecs.h>
#include "genie/core/constants.h"
#include "genie/core/record/variant/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"
#include "helpers.h"

#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"
#include "genie/backend/backend.h"

namespace genie::likelihood {

// Helper to generate synthetic records
std::vector<core::record::VariantGenotype> create_synthetic_records(size_t num_records, size_t num_samples, size_t num_likelihoods) {
    std::vector<core::record::VariantGenotype> recs;
    recs.reserve(num_records);
    for (size_t i = 0; i < num_records; ++i) {
        recs.emplace_back();
        recs.back().SetSampleCount((uint32_t)num_samples);
        std::vector<std::vector<uint32_t>> likelihoods;
        for (size_t s = 0; s < num_samples; ++s) {
            std::vector<uint32_t> sample_gl;
            for (size_t l = 0; l < num_likelihoods; ++l) {
                sample_gl.push_back((uint32_t)((i * num_samples * num_likelihoods + s * num_likelihoods + l) % 255));
            }
            likelihoods.push_back(sample_gl);
        }
        recs.back().SetLikelihoods(std::move(likelihoods));
    }
    return recs;
}

TEST(Likelihood, SyntheticRoundTrip) {
    size_t num_records = 100;
    size_t num_samples = 2;
    size_t num_likelihoods = 3;
    auto recs = create_synthetic_records(num_records, num_samples, num_likelihoods);

    LikelihoodParameters params;
    LikelihoodPayload payload;
    
    // Encode
    encode_likelihood(recs, params, payload, 256, true);

    // Decode
    std::vector<core::record::VariantGenotype> decoded_recs(num_records);
    for(auto& r : decoded_recs) {
        r.SetSampleCount((uint32_t)num_samples);
        r.SetNumberOfLikelihoods((uint8_t)num_likelihoods);
    }
    decode_likelihood(params, payload, decoded_recs);

    ASSERT_EQ(recs.size(), decoded_recs.size());
    for (size_t i = 0; i < recs.size(); ++i) {
        auto& orig = recs[i].GetLikelihoods();
        auto& decoded = decoded_recs[i].GetLikelihoods();
        ASSERT_EQ(orig.size(), decoded.size());
        for (size_t s = 0; s < orig.size(); ++s) {
            ASSERT_EQ(orig[s], decoded[s]);
        }
    }
}

TEST(Likelihood, ParseLikelihood) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/variant/1.3.5.header100.gl_only.vcf.geno";
    std::vector<core::record::VariantGenotype> recs;

    uint32_t BLOCK_SIZE = 100;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    if (!recs.empty()) recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    EncodingOptions opt = {
        BLOCK_SIZE,  // block_size
        true,        // transform_flag;
    };

    LikelihoodEncodingBlock block{};
    extract_likelihoods(opt, block, recs);

    auto& likelihood_mat = block.likelihood_mat;

    ASSERT_EQ(backend::get_mat_shape(likelihood_mat, 0), BLOCK_SIZE);
    ASSERT_EQ(backend::get_mat_shape(likelihood_mat, 1), 1092 * 3);
    ASSERT_EQ(backend::get_mat_element(likelihood_mat, 0, 4), 3197737370);
    ASSERT_EQ(backend::get_mat_element(likelihood_mat, 0, 8), 3241567846);
}

TEST(Likelihood, RoundTripNoTransform) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/variant/1.3.5.header100.gl_only.vcf.geno";
    std::vector<core::record::VariantGenotype> recs;

    uint32_t BLOCK_SIZE = 100;
    bool TRANSFORM_MODE = false;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    if (!recs.empty()) recs.pop_back();

    ASSERT_EQ(recs.size(), 100);
    EncodingOptions opt = {
        BLOCK_SIZE,      // block_size
        TRANSFORM_MODE,  // transform_flag;
    };

    LikelihoodEncodingBlock block{};
    extract_likelihoods(opt, block, recs);
    auto original_mat = backend::clone_mat(block.likelihood_mat);

    transform_likelihood_mat(opt, block);

    ASSERT_TRUE(static_cast<uint32_t>(block.dtype_id) == static_cast<uint32_t>(core::DataType::UINT32));
    ASSERT_TRUE(backend::mats_equal(original_mat, block.likelihood_mat));
}

TEST(Likelihood, RoundTripTransform) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/variant/1.3.5.header100.gl_only.vcf.geno";
    std::vector<core::record::VariantGenotype> recs;

    uint32_t BLOCK_SIZE = 100;
    bool TRANSFORM_MODE = true;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    util::BitReader bitreader(reader);
    while (bitreader.IsStreamGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    if (!recs.empty()) recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    EncodingOptions opt = {
        BLOCK_SIZE,      // block_size
        TRANSFORM_MODE,  // transform_flag;
    };

    LikelihoodEncodingBlock block{};
    extract_likelihoods(opt, block, recs);
    auto original_mat = backend::clone_mat(block.likelihood_mat);

    transform_likelihood_mat(opt, block);
    backend::clear_mat(block.likelihood_mat);

    inverse_transform_likelihood_mat(opt, block);

    ASSERT_TRUE(static_cast<uint32_t>(block.dtype_id) == static_cast<uint32_t>(core::DataType::UINT16));
    ASSERT_TRUE(backend::mats_equal(original_mat, block.likelihood_mat));
}

TEST(Likelihood, DISABLED_GenerateGoldenMaster) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string goldenFile = gitRootDir + "/test/gold/likelihood/ref_payload.bin";

    size_t num_records = 100;
    size_t num_samples = 2;
    size_t num_likelihoods = 3;
    auto recs = create_synthetic_records(num_records, num_samples, num_likelihoods);

    LikelihoodParameters params;
    LikelihoodPayload payload;
    encode_likelihood(recs, params, payload, 256, false);

    std::ofstream writer(goldenFile, std::ios::binary);
    util::BitWriter bitwriter(&writer);
    payload.write(bitwriter);
    bitwriter.FlushBits();
    writer.close();

    std::cout << "Generated Golden Master at: " << goldenFile << std::endl;
}

TEST(Likelihood, CrossBackend_GoldenMaster) {

    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string goldenFile = gitRootDir + "/test/gold/likelihood/ref_payload.bin";

    std::ifstream goldenReader(goldenFile, std::ios::binary);
    if (goldenReader.fail()) {
        std::cout << "Skipping Golden Master test (file not found)" << std::endl;
        return;
    }
    std::string goldenStr((std::istreambuf_iterator<char>(goldenReader)), std::istreambuf_iterator<char>());
    std::vector<uint8_t> goldenBytes(goldenStr.begin(), goldenStr.end());

    size_t num_records = 100;
    size_t num_samples = 2;
    size_t num_likelihoods = 3;
    auto recs = create_synthetic_records(num_records, num_samples, num_likelihoods);

    LikelihoodParameters params;
    LikelihoodPayload payload;
    encode_likelihood(recs, params, payload, 256, false);


    std::stringstream buffer;
    util::BitWriter bitwriter(&buffer);
    payload.write(bitwriter);
    bitwriter.FlushBits();

    std::string generatedStr = buffer.str();
    std::vector<uint8_t> generatedBytes(generatedStr.begin(), generatedStr.end());

    ASSERT_EQ(goldenBytes.size(), generatedBytes.size()) << "Payload size mismatch!";
    for (size_t i = 0; i < goldenBytes.size(); ++i) {
        ASSERT_EQ(goldenBytes[i], generatedBytes[i]) << "Content mismatch at index " << i;
    }
}

} // namespace genie::likelihood
