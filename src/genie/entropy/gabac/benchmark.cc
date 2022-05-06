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

paramcabac::Binarization ConfigSearchBinarization::getBinarization(bool bypass, uint8_t outputbits,
                                                                   uint8_t subsymsize) const {
    return paramcabac::Binarization(
        lut[binarization_search_idx], bypass,
        paramcabac::BinarizationParameters(
            lut[binarization_search_idx],
            {binarizationParameters[binarization_search_idx].getIndex(binarization_parameter_search_idx)}),
        paramcabac::Context(true, outputbits, subsymsize, true));
}

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearchBinarization::ConfigSearchBinarization(const std::pair<int64_t, int64_t>& range, uint8_t split_size)
    : binarization_search_idx(0), binarization_parameter_search_idx(0) {
    {
        if (split_size == 0) {
            auto bits = std::ceil(std::log2(std::max(std::abs(range.first), std::abs(range.second))));
            if (range.first < 0) {
                bits++;
            }
            lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::BI);
            binarizationParameters.emplace_back(bits, bits, 1);
        }
    }
    if (range.first >= 0) {


        if (split_size != 0) {
            lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::SUTU);
            binarizationParameters.emplace_back(split_size, split_size, 1);
        } else {
            if (range.second < 256) {
                lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::TU);
                binarizationParameters.emplace_back(range.second, range.second, 1);
            }
            lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::EG);
            binarizationParameters.emplace_back(0, 0, 1);
        }

        //      lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::TEG);
        //      auto max_param = (std::min(int64_t(64), range.second) / 4) * 4 + 1;

        //      binarizationParameters.emplace_back(1, max_param, std::max(int64_t(1), (max_param - 1) / 4));
    } else {
        if (split_size != 0) {
            lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::SSUTU);
            binarizationParameters.emplace_back(split_size, split_size, 1);
        } else {
            lut.emplace_back(genie::entropy::paramcabac::BinarizationParameters::BinarizationId::SEG);
            binarizationParameters.emplace_back(0, 0, 1);
        }

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

int8_t ConfigSearchTranformedSeq::getSubsymbolTransID() {
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
    if (subsymbol_transform_search_idx == subsymbol_transform_enabled.size() ||
        (getSubsymbolTransID() == 2 && !LUTValid)) {
        subsymbol_transform_search_idx = 0;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

paramcabac::TransformedSubSeq ConfigSearchTranformedSeq::createConfig(
    const genie::core::GenSubIndex& descriptor_subsequence, bool original) {
    int lut[] = {0, 2, 1};
    auto subsymbol_trans = paramcabac::SupportValues::TransformIdSubsym(lut[getSubsymbolTransID()]);

    auto symbol_size = output_bits;
    if (!split_in_binarization.getIndex(split_in_binarization_idx)) {
        symbol_size = output_bits / getSplitRatio();
    }

    paramcabac::TransformedSubSeq ret(
        subsymbol_trans,
        paramcabac::SupportValues(output_bits, symbol_size, coding_order.getIndex(coding_order_search_idx)),
        binarizations[split_size.getIndex(split_size_idx)][split_in_binarization.getIndex(split_in_binarization_idx)][getSubsymbolTransID()].getBinarization(
            bypass.getIndex(bypass_search_idx), output_bits, symbol_size),
        descriptor_subsequence, original);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearchTranformedSeq::increment() {
    // Increment binarization for current split and transformation
    if (binarizations[split_size.getIndex(split_size_idx)][split_in_binarization.getIndex(split_in_binarization_idx)][getSubsymbolTransID()].increment()) {
        return true;
    }

    // Increment subsymbol tranformation
    if (incrementTransform()) {
        return true;
    }

    split_in_binarization_idx++;
    if (split_in_binarization_idx == split_in_binarization.size()) {
        split_in_binarization_idx = 0;
        split_size_idx++;
    }

    if (split_size_idx == split_size.size()) {
        split_size_idx = 0;
        bypass_search_idx++;
    }

    // Increment bypass setting
    if (bypass_search_idx == bypass.size()) {
        bypass_search_idx = 0;
        coding_order_search_idx++;
    }

    // If coding order cannot be incremented, all settings have been explored
    if (coding_order_search_idx == coding_order.size()) {
        coding_order_search_idx = 0;
        return false;
    }

    // Skip split binarizations for coding order > 0
    while ((coding_order.getIndex(coding_order_search_idx) > 0 &&
            split_in_binarization.getIndex(split_in_binarization_idx)) ||
           (output_bits % getSplitRatio()) ||
           ((coding_order.getIndex(coding_order_search_idx) == 2 && output_bits / getSplitRatio() > 8) ||
            (coding_order.getIndex(coding_order_search_idx) == 1 && output_bits / getSplitRatio() > 16))) {
        if (!increment()) {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearchTranformedSeq::ConfigSearchTranformedSeq(const std::pair<int64_t, int64_t>& range)
    : split_size(0, 3, 1),
      split_size_idx(0),
      split_in_binarization(0, 1, 1),
      split_in_binarization_idx(0),
      coding_order(0, 2, 1),
      coding_order_search_idx(0),
      bypass(0, 0, 1),
      bypass_search_idx(0),
      subsymbol_transform_enabled(0, 1, 1),
      subsymbol_transform_search_idx(0) {
    // LUT only allowed for value range < 8 bit
    LUTValid =
        (range.first >= std::numeric_limits<uint8_t>::min() && range.second <= std::numeric_limits<uint8_t>::max()) ||
        (range.first >= std::numeric_limits<int8_t>::min() && range.second <= std::numeric_limits<int8_t>::max());

    // Necessary bits
    output_bits = std::ceil(log2(std::max(std::abs(range.first) + 1, std::abs(range.second) + 1)));
    if (range.first < 0) {
        output_bits++;
    }
    // Split sizes 2^0 to 2^4
    for (int i = 0; i < 4; ++i) {
        binarizations.emplace_back();
        // Split at binarization level (1) or as subsymbol (0)
        for (int j = 0; j < 2; ++j) {
            binarizations.back().emplace_back();

            uint64_t splitsize = 1 << i;
            uint64_t codingsize = output_bits / splitsize;

            if (j == 0) {
                codingsize = 0;
            }
            binarizations.back().back().emplace_back(range, codingsize);  // No-Transform
            binarizations.back().back().emplace_back(
                std::make_pair(-(range.second - range.first), range.second - range.first),
                codingsize);  // DIFF
            binarizations.back().back().emplace_back(std::make_pair(0, range.second - range.first),
                                                     codingsize);  // LUT
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ConfigSearchTranformedSeq::mutate(std::mt19937& rd, std::normal_distribution<>& d) {
    coding_order_search_idx = coding_order.mutate(coding_order_search_idx, d(rd));
    split_size_idx = split_size.mutate(split_size_idx, d(rd));
    subsymbol_transform_search_idx = subsymbol_transform_enabled.mutate(subsymbol_transform_search_idx, d(rd));
    bypass_search_idx = bypass.mutate(bypass_search_idx, d(rd));
    if (getSubsymbolTransID() == 2 && !LUTValid) {
        subsymbol_transform_search_idx = 0;
    }
    binarizations[split_size.getIndex(split_size_idx)][split_in_binarization.getIndex(split_in_binarization_idx)][getSubsymbolTransID()].mutate(rd, d);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ResultFull::toCSV(const std::string& filename) {
    std::string ret;
    std::string desc;
    desc += "File;Compressed size;Compression time;Transformation;TransformationParam;";
    ret += std::to_string(size) + ";" + std::to_string(milliseconds) + ";" +
           std::to_string(size_t(config.getTransformParameters().getTransformIdSubseq())) + ";" +
           std::to_string(size_t(config.getTransformParameters().getParam())) + ";";
    for (auto& cfg : config.getTransformSubseqCfgs()) {
        desc += "Coding order;Symbol size;Subsymbol size;Subsymbol transformation;Binarization;BinParam;";
        ret += std::to_string(cfg.getSupportValues().getCodingOrder()) + ";" +
               std::to_string(cfg.getSupportValues().getOutputSymbolSize()) + ";" +
               std::to_string(cfg.getSupportValues().getCodingSubsymSize()) + ";" +
               std::to_string(size_t(cfg.getTransformIDSubsym())) + ";" +
               std::to_string(size_t(cfg.getBinarization().getBinarizationID())) + ";";
        if (cfg.getBinarization().getBinarizationID() == paramcabac::BinarizationParameters::BinarizationId::TU) {
            ret += std::to_string(size_t(cfg.getBinarization().getCabacBinarizationParameters().getCMax())) + ";";
        } else if (cfg.getBinarization().getBinarizationID() ==
                   paramcabac::BinarizationParameters::BinarizationId::SUTU) {
            ret +=
                std::to_string(size_t(cfg.getBinarization().getCabacBinarizationParameters().getSplitUnitSize())) + ";";
        } else {
            ret += ";";
        }
    }
    for (int i = 0; i < (3 - static_cast<int>(config.getTransformSubseqCfgs().size())); ++i) {
        desc += "Coding order;Symbol size;Subsymbol size;Subsymbol transformation;Binarization;BinParam;";
        ret += ";;;;;;";
    }
    return desc + "\n" + filename + ";" + ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ResultFull benchmark_full(const std::string& input_file, const genie::core::GenSubIndex& desc, float timeweight) {
    std::ifstream input_stream(input_file);
    util::DataBlock sequence(0, core::range2bytes(core::getSubsequence(desc).range));
    gabac::StreamHandler::readFull(input_stream, &sequence);

    ResultFull best_result;
    float best_score = INFINITY;
    ConfigSearch config(genie::core::getSubsequence(desc).range);
    do {
        std::cerr << "Optimizing transformation " << (int)config.getTransform() << "..." << std::endl;
        // Execute transformation
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

        // Optimze transformed sequences independently
        for (size_t i = 0; i < transformedSubseqs.size(); ++i) {
            std::cerr << "Optimizing subsequence " << i << "..." << std::endl;
            trans_results.emplace_back(optimizeTransformedSequence(config.getTransformedSeqs()[i], desc,
                                                                   transformedSubseqs[i], timeweight,
                                                                   i == transformedSubseqs.size() - 1));
            auto tmp = trans_results.back().config;
            cfg.setTransformSubseqCfg(i, std::move(tmp));
        }

        // Sum up compressed sizes and times
        size_t total_size = 0;
        for (const auto& r : trans_results) {
            total_size += r.size;
            total_time += r.milliseconds;  // Transformation time already added above
        }

        // Calculate weighted score
        float score = timeweight * total_time + (1 - timeweight) * total_size;

        // Found new best score
        if (score < best_score) {
            best_score = score;
            best_result.milliseconds = total_time;
            best_result.size = total_size;
            best_result.config = cfg;
        }
    } while (config.increment());

    std::cout << best_result.toCSV(input_file) << std::endl;
    return best_result;
}

// ---------------------------------------------------------------------------------------------------------------------

int16_t ConfigSearchTransformation::getParameter() const { return parameter.getIndex(parameter_search_idx); }

// ---------------------------------------------------------------------------------------------------------------------

ConfigSearchTransformation::ConfigSearchTransformation(paramcabac::TransformedParameters::TransformIdSubseq trans_id,
                                                       const std::pair<int64_t, int64_t>& range)
    : parameter_search_idx(0) {
    const uint16_t MATCH_CODING_BUFFER_SIZE = 255;
    const uint8_t RLE_GUARD = 255;
    switch (trans_id) {
        case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            parameter = SearchSpace<int16_t>(0, 0, 1);  // No param
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            parameter = SearchSpace<int16_t>(0, 0, 1);  // No param
            transformedSeqs.emplace_back(std::make_pair(0, 1));
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            parameter = SearchSpace<int16_t>(MATCH_CODING_BUFFER_SIZE, MATCH_CODING_BUFFER_SIZE, 1);
            transformedSeqs.emplace_back(std::make_pair(0, MATCH_CODING_BUFFER_SIZE));
            transformedSeqs.emplace_back(std::make_pair(0, MATCH_CODING_BUFFER_SIZE));
            transformedSeqs.emplace_back(range);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            parameter = SearchSpace<int16_t>(RLE_GUARD, RLE_GUARD, 1);
            transformedSeqs.emplace_back(std::make_pair(0, RLE_GUARD));
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
    // Try all parameters
    parameter_search_idx++;
    if (parameter_search_idx == parameter.size()) {
        // All parameters tried
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
    transformation = SearchSpace<int8_t>(0, 3, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

bool ConfigSearch::increment() {
    // Search current transformation
    if (params[transformation.getIndex(transformation_search_idx)].increment()) {
        return true;
    }

    // Current transformation completely searched, next transformation
    transformation_search_idx++;
    if (transformation_search_idx == transformation.size()) {
        transformation_search_idx = 0;
        return false;
    }

    // All transformations searched
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ConfigSearch::mutate(float rate) {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::normal_distribution<> d{0, rate};

    transformation_search_idx = transformation.mutate(transformation_search_idx, d(gen));
    params[transformation.getIndex(transformation_search_idx)].mutate(gen, d);
}

// ---------------------------------------------------------------------------------------------------------------------

paramcabac::Subsequence ConfigSearch::createConfig(genie::core::GenSubIndex descriptor_subsequence, bool tokentype) {
    // Generate top level transformation
    auto t = paramcabac::TransformedParameters(
        paramcabac::TransformedParameters::TransformIdSubseq(transformation.getIndex(transformation_search_idx)),
        params[transformation.getIndex(transformation_search_idx)].getParameter());

    // Add config for transformed sequences
    std::vector<paramcabac::TransformedSubSeq> tss;
    size_t i = 0;
    for (auto& p : params[transformation.getIndex(transformation_search_idx)].getTransformedSeqs()) {
        tss.emplace_back(p.createConfig(
            descriptor_subsequence,
            i == params[transformation.getIndex(transformation_search_idx)].getTransformedSeqs().size() - 1));
    }
    paramcabac::Subsequence ret(std::move(t), descriptor_subsequence.second, tokentype, std::move(tss));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ResultTransformed optimizeTransformedSequence(ConfigSearchTranformedSeq& seq, const genie::core::GenSubIndex& gensub,
                                              util::DataBlock& data, float timeweight, bool original) {
    ResultTransformed ret;
    float score = INFINITY;
    do {
        auto cfg = seq.createConfig(gensub, original);
        auto input = data;

        util::Watch watch;
        watch.reset();
        gabac::encodeTransformSubseq(cfg, &input);
        auto time = size_t(watch.check() * 1000);  // Milliseconds
        auto size = input.getRawSize();

        float newscore = timeweight * time + (1.0 - timeweight) * size;
        if (newscore < score) {
            // Found new bets score
            score = newscore;
            ret.milliseconds = time;
            ret.size = size;
            ret.config = cfg;
        }
    } while (seq.increment());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
