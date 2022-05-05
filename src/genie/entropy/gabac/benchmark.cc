/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/benchmark.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include "genie/entropy/gabac/encode-desc-subseq.h"
#include "genie/entropy/gabac/encode-transformed-subseq.h"
#include "genie/entropy/gabac/stream-handler.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

paramcabac::Binarization ConfigSearchBinarization::getBinarization(bool bypass, uint8_t outputbits) const {
    return paramcabac::Binarization(
        lut[binarization_search_idx], bypass,
        paramcabac::BinarizationParameters(
            lut[binarization_search_idx],
            {binarizationParameters[binarization_search_idx].getIndex(binarization_parameter_search_idx)}),
        paramcabac::Context(true, outputbits, outputbits, true));
}

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearchBinarization::ConfigSearchBinarization(const std::pair<int64_t, int64_t>& range)
    : binarization_search_idx(0), binarization_parameter_search_idx(0) {
    {
        auto bits = std::ceil(std::log2(std::max(std::abs(range.first), std::abs(range.second))));
        if (range.first < 0) {
            bits++;
        }
        lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::BI);
        binarizationParameters.emplace_back(bits, bits, 1);
    }
    if (range.first >= 0) {
        if (range.second < 256) {
            lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::TU);
            binarizationParameters.emplace_back(range.second, range.second, 1);
        }
        lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::EG);
        binarizationParameters.emplace_back(0, 0, 1);

        //      lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::TEG);
        //      auto max_param = (std::min(int64_t(64), range.second) / 4) * 4 + 1;

        //      binarizationParameters.emplace_back(1, max_param, std::max(int64_t(1), (max_param - 1) / 4));
    } else {
        lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::SEG);
        binarizationParameters.emplace_back(0, 0, 1);

        //    lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::STEG);
        //     auto max_value = std::max(std::abs(range.first), std::abs(range.second));
        //    auto max_param = (std::min(int64_t(64), max_value) / 4) * 4;

        //     binarizationParameters.emplace_back(0, max_param, std::max(int64_t(1), max_param / 4));
    }
    binarization = SearchSpace<uint8_t>(0, lut.size() - 1, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearchBinarization::increment() {
    binarization_parameter_search_idx++;
    if (binarization_parameter_search_idx == binarizationParameters[binarization_search_idx].size()) {
        binarization_parameter_search_idx = 0;
        binarization_search_idx++;
    }
    if (binarization_search_idx == binarization.size()) {
        binarization_search_idx = 0;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ConfigSearchBinarization::mutate(std::mt19937& rd, std::normal_distribution<>& d) {
    binarization_search_idx = binarization.mutate(binarization_search_idx, d(rd));
    binarization_parameter_search_idx =
        binarizationParameters[binarization_search_idx].mutate(binarization_parameter_search_idx, d(rd));
}

// ---------------------------------------------------------------------------------------------------------------------

int8_t ConfigSearchTranformedSeq::getBinID() {
    if (subsymbol_transform_enabled.getIndex(subsymbol_transform_search_idx) == 0) {
        return 0;
    }
    if (coding_order.getIndex(coding_order_search_idx) == 0) {
        return 0;
    }
    return 2;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearchTranformedSeq::incrementTransform() {
    subsymbol_transform_search_idx++;
    if (subsymbol_transform_search_idx == subsymbol_transform_enabled.size() || (getBinID() == 2 && !LUTValid)) {
        subsymbol_transform_search_idx = 0;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

paramcabac::TransformedSubSeq ConfigSearchTranformedSeq::createConfig(
    const genie::core::GenSubIndex& descriptor_subsequence) {
    int lut[] = {0, 2, 1};
    auto subsymbol_trans = paramcabac::SupportValues::TransformIdSubsym(lut[getBinID()]);

    paramcabac::TransformedSubSeq ret(
        subsymbol_trans,
        paramcabac::SupportValues(output_bits, output_bits, coding_order.getIndex(coding_order_search_idx)),
        binarizations[getBinID()].getBinarization(bypass.getIndex(bypass_search_idx), output_bits),
        descriptor_subsequence);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearchTranformedSeq::increment() {
    if (binarizations[getBinID()].increment()) {
        return true;
    }

    if (incrementTransform()) {
        return true;
    }

    bypass_search_idx++;
    if (bypass_search_idx == bypass.size()) {
        bypass_search_idx = 0;
        coding_order_search_idx++;
    }

    if (coding_order_search_idx == coding_order.size()) {
        coding_order_search_idx = 0;
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearchTranformedSeq::ConfigSearchTranformedSeq(const std::pair<int64_t, int64_t>& range)
    : coding_order(0, 2, 2),
      coding_order_search_idx(0),
      bypass(0, 0, 1),
      bypass_search_idx(0),
      subsymbol_transform_enabled(0, 1, 1),
      subsymbol_transform_search_idx(0) {
    LUTValid =
        (range.first >= std::numeric_limits<uint8_t>::min() && range.second <= std::numeric_limits<uint8_t>::max()) ||
        (range.first >= std::numeric_limits<int8_t>::min() && range.second <= std::numeric_limits<int8_t>::max());
    binarizations.emplace_back(range);  // No-Transform
    binarizations.emplace_back(std::make_pair(-(range.second - range.first), range.second - range.first));  // DIFF
    binarizations.emplace_back(std::make_pair(0, range.second - range.first));                              // LUT

    output_bits = std::ceil(log2(std::max(std::abs(range.first) + 1, std::abs(range.second) + 1)));
    if (range.first < 0) {
        output_bits++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ConfigSearchTranformedSeq::mutate(std::mt19937& rd, std::normal_distribution<>& d) {
    coding_order_search_idx = coding_order.mutate(coding_order_search_idx, d(rd));
    subsymbol_transform_search_idx = subsymbol_transform_enabled.mutate(subsymbol_transform_search_idx, d(rd));
    bypass_search_idx = bypass.mutate(bypass_search_idx, d(rd));
    if (getBinID() == 2 && !LUTValid) {
        subsymbol_transform_search_idx = 0;
    }
    binarizations[getBinID()].mutate(rd, d);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ResultFull::toCSV() {
    std::string ret;
    ret += std::to_string(size) + ";" + std::to_string(milliseconds) + ";" +
           std::to_string(size_t(config.getTransformParameters().getTransformIdSubseq())) + ";";
    for (auto& cfg : config.getTransformSubseqCfgs()) {
        ret += std::to_string(cfg.getSupportValues().getCodingOrder()) + ";" +
               std::to_string(cfg.getSupportValues().getOutputSymbolSize()) + ";" +
               std::to_string(size_t(cfg.getTransformIDSubsym())) + ";" +
               std::to_string(size_t(cfg.getBinarization().getBinarizationID())) + ";";
    }
    for (int i = 0; i < (3 - static_cast<int>(config.getTransformSubseqCfgs().size())); ++i) {
        ret += ";;;;";
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ResultFull benchmark_full(const std::string& input_file, const genie::core::GenSubIndex& desc, float timeweight) {
    std::ifstream input_stream(input_file);
    util::DataBlock sequence(0, 4);
    gabac::StreamHandler::readFull(input_stream, &sequence);

    ResultFull best_result;
    float best_score = INFINITY;
    ConfigSearch config(genie::core::getSubsequence(desc).range);
    do {
        std::vector<util::DataBlock> transformedSubseqs;
        transformedSubseqs.resize(1);
        auto subsequence = sequence;
        transformedSubseqs[0].swap(&subsequence);
        util::Watch timer;
        timer.reset();
        auto cfg = config.createConfig(desc, core::getDescriptor(desc.first).tokentype);
        gabac::doSubsequenceTransform(cfg, &transformedSubseqs);
        size_t total_time = timer.check() * 1000;
        std::vector<ResultTransformed> trans_results;

        for (size_t i = 0; i < transformedSubseqs.size(); ++i) {
            uint64_t max = 0;
            for (size_t j = 0; j < transformedSubseqs[i].size(); ++j) {
                max = std::max(max, transformedSubseqs[i].get(j));
            }
            trans_results.emplace_back(
                optimizeTransformedSequence(config.getTransformedSeqs()[i], desc, transformedSubseqs[i], timeweight));
            auto tmp = trans_results.back().config;
            cfg.setTransformSubseqCfg(i, std::move(tmp));
        }

        size_t total_size = 0;
        for (const auto& r : trans_results) {
            total_size += r.size;
            total_time += r.milliseconds;
        }

        float score = timeweight * total_time + (1 - timeweight) * total_size;
        if (score < best_score) {
            best_score = score;
            best_result.milliseconds = total_time;
            best_result.size = total_size;
            best_result.config = cfg;
            //   std::cout << "New best! Size: " << total_size << ", time: " << total_time << std::endl;
        }
    } while (config.increment());

    std::cout << input_file << ";" << best_result.toCSV() << std::endl;
    return best_result;
}

// ---------------------------------------------------------------------------------------------------------------------

int16_t ConfigSearchTransformation::getParameter() const { return parameter.getIndex(parameter_search_idx); }

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearchTransformation::ConfigSearchTransformation(paramcabac::TransformedParameters::TransformIdSubseq trans_id,
                                                       const std::pair<int64_t, int64_t>& range)
    : parameter_search_idx(0) {
    switch (trans_id) {
        case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            parameter = SearchSpace<int16_t>(0, 0, 1);
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            parameter = SearchSpace<int16_t>(0, 0, 1);
            transformedSeqs.emplace_back(std::make_pair(0, 1));
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            parameter = SearchSpace<int16_t>(64, 64, 1);
            transformedSeqs.emplace_back(std::make_pair(0, 64));
            transformedSeqs.emplace_back(std::make_pair(0, 64));
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            parameter = SearchSpace<int16_t>(255, 255, 1);
            transformedSeqs.emplace_back(std::make_pair(0, 255));
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
            UTILS_DIE("Merge coding unsupported");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<ConfigSearchTranformedSeq>& ConfigSearchTransformation::getTransformedSeqs() { return transformedSeqs; }

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearchTransformation::increment() {
    parameter_search_idx++;
    if (parameter_search_idx == parameter.size()) {
        parameter_search_idx = 0;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ConfigSearchTransformation::mutate(std::mt19937& rd, std::normal_distribution<>& d) {
    parameter_search_idx = parameter.mutate(parameter_search_idx, d(rd));

    for (auto& ts : transformedSeqs) {
        ts.mutate(rd, d);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<ConfigSearchTranformedSeq>& ConfigSearch::getTransformedSeqs() {
    return params[transformation.getIndex(transformation_search_idx)].getTransformedSeqs();
}

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearch::ConfigSearch(const std::pair<int64_t, int64_t>& range) : transformation_search_idx(0) {
    params.emplace_back(paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM, range);
    params.emplace_back(paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING, range);
    params.emplace_back(paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING, range);
    params.emplace_back(paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING, range);
    transformation = SearchSpace<int8_t>(0, 0, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearch::increment() {
    if (params[transformation.getIndex(transformation_search_idx)].increment()) {
        return true;
    }
    transformation_search_idx++;
    if (transformation_search_idx == transformation.size()) {
        transformation_search_idx = 0;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ConfigSearch::mutate(float rate) {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::normal_distribution<> d{0, rate};

    transformation_search_idx = transformation.mutate(transformation_search_idx, d(gen));
    params[transformation_search_idx].mutate(gen, d);
}

// ---------------------------------------------------------------------------------------------------------------------

paramcabac::Subsequence ConfigSearch::createConfig(genie::core::GenSubIndex descriptor_subsequence, bool tokentype) {
    auto t = paramcabac::TransformedParameters(
        paramcabac::TransformedParameters::TransformIdSubseq(transformation.getIndex(transformation_search_idx)),
        params[transformation.getIndex(transformation_search_idx)].getParameter());
    std::vector<paramcabac::TransformedSubSeq> tss;
    for (auto& p : params[transformation.getIndex(transformation_search_idx)].getTransformedSeqs()) {
        tss.emplace_back(p.createConfig(descriptor_subsequence));
    }
    paramcabac::Subsequence ret(std::move(t), descriptor_subsequence.second, tokentype, std::move(tss));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ResultTransformed optimizeTransformedSequence(ConfigSearchTranformedSeq& seq, const genie::core::GenSubIndex& gensub,
                                              util::DataBlock& data, float timeweight) {
    ResultTransformed ret;
    float score = INFINITY;
    do {
        auto cfg = seq.createConfig(gensub);
        auto input = data;
        util::Watch watch;
        watch.reset();
        //      std::cout << "Trying config " << i++ << std::endl;
        gabac::encodeTransformSubseq(cfg, &input);
        auto time = size_t(watch.check() * 1000);
        auto size = input.getRawSize();
        float newscore = timeweight * time + (1.0 - timeweight) * size;
        if (newscore < score) {
            score = newscore;
            ret.milliseconds = time;
            ret.size = size;
            ret.config = cfg;
            //      std::cout << "New best!" << std::endl;
        }
        //       std::cout << "    -> " << time << "ms, " << size << " bytes" << std::endl;
    } while (seq.increment());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
