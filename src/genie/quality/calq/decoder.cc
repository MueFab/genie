/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/calq/decoder.h"
#include <string>
#include <tuple>
#include <vector>
#include "genie/quality/calq/permutation.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

// ---------------------------------------------------------------------------------------------------------------------

bool Decoder::isAligned(const core::AccessUnit::Descriptor& desc) {
    // TODO: better check
    return desc.getSize() != 3;
}

std::vector<std::string> Decoder::decodeAligned(const quality::paramqv1::QualityValues1& param,
                                                const std::vector<std::string>& ecigar_vec,
                                                const std::vector<uint64_t>& positions,
                                                core::AccessUnit::Descriptor& desc) {
    // sort
    auto permutation = sort_permutation(positions);
    auto sortedPos = apply_permutation(positions, permutation);
    auto sortedCigars = apply_permutation(ecigar_vec, permutation);

    // calq variables
    calq::DecodingOptions options;  // default options
    calq::SideInformation sideInformation;
    calq::EncodingBlock output;
    calq::DecodingBlock input;

    sideInformation.qualOffset = 33;  // TODO ASK
    fillInput(input, desc, param);

    // fill sideInformation
    sideInformation.positions.insert(sideInformation.positions.end(), sortedPos.begin(), sortedPos.end());
    sideInformation.cigars = std::move(sortedCigars);

    calq::decode(options, sideInformation, input, &output);

    // resort because of needed sort for calq
    std::vector<std::string> qualityStrings(output.qvalues.size());

    for (size_t i = 0; i < qualityStrings.size(); ++i) {
        qualityStrings[i] = std::move(output.qvalues[permutation[i]]);
    }

    return qualityStrings;
}

std::vector<std::string> Decoder::decodeUnaligned(const quality::paramqv1::QualityValues1& param_casted,
                                                  const std::vector<std::string>& ecigar_vec,
                                                  core::AccessUnit::Descriptor& desc) {
    // from qvwriteout

    std::vector<std::string> qv;

    for (const auto& ecigar : ecigar_vec) {
        qv.emplace_back();
        if (!desc.get(0).end() && !desc.get(0).pull()) {
            continue;
        }
        core::CigarTokenizer::tokenize(
            ecigar, core::getECigarInfo(),
            [&qv, &desc, &param_casted](uint8_t cigar, const util::StringView& bs, const util::StringView&) -> bool {
                auto codebook = (uint8_t)param_casted.getNumberCodeBooks() - 1;
                if (core::getECigarInfo().lut_step_ref[cigar] ||
                    core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)) {
                    codebook = desc.get(1).end() ? 0 : (uint8_t)desc.get(1).pull();
                }

                for (size_t i = 0; i < bs.length(); ++i) {
                    auto index = (uint8_t)desc.get((uint16_t)(codebook + 2)).pull();
                    qv.back().push_back(param_casted.getCodebook(codebook).getEntries()[index]);
                }
                return true;
            });
    }
    return qv;
}

void Decoder::fillInput(calq::DecodingBlock& input, core::AccessUnit::Descriptor& desc,
                        const quality::paramqv1::QualityValues1& param) {
    // quantizerIndices + istepIndices
    for (size_t i = 1; i < desc.getSize(); ++i) {
        auto data = static_cast<uint8_t*>(desc.get(i).getData().getData());
        auto size = desc.get(i).getData().size();

        if (i == 1) {
            input.quantizerIndices = std::vector<uint8_t>(data, data + size);
        } else {
            input.stepindices.emplace_back(data, data + size);
        }
    }

    // codebooks
    for (size_t i = 0; i < param.getNumberCodeBooks(); ++i) {
        input.codeBooks.push_back(param.getCodebook(i).getEntries());
    }
}

std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::process(
    const core::parameter::QualityValues& param, const std::vector<std::string>& ecigar_vec,
    const std::vector<uint64_t>& positions, core::AccessUnit::Descriptor& desc) {
    util::Watch watch;
    const auto& param_casted = dynamic_cast<const quality::paramqv1::QualityValues1&>(param);

    std::vector<std::string> resultQV;

    if (isAligned(desc)) {
        resultQV = decodeAligned(param_casted, ecigar_vec, positions, desc);
    } else {
        resultQV = decodeUnaligned(param_casted, ecigar_vec, desc);
    }

    core::stats::PerfStats stats;
    stats.addDouble("time-qvcalq", watch.check());
    return std::make_tuple(resultQV, stats);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
