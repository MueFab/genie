/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/qvcalq/decoder.h"
#include <string>
#include <tuple>
#include <vector>
#include "genie/quality/qvcalq/permutation.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

// ---------------------------------------------------------------------------------------------------------------------

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

    // position stuff
    auto permutation = sort_permutation(positions);
    auto sortedPos = apply_permutation(positions, permutation);
    auto sortedCigars = apply_permutation(ecigar_vec, permutation);

    // calq variables
    calq::DecodingOptions options;  // default options
    calq::SideInformation sideInformation;
    calq::EncodingBlock output;
    calq::DecodingBlock input;

    fillInput(input, desc, param_casted);

    // fill sideInformation
    sideInformation.positions.insert(sideInformation.positions.end(), sortedPos.begin(), sortedPos.end());
    sideInformation.cigars = std::move(sortedCigars);

    calq::decode(options, sideInformation, input, &output);
    std::tuple<std::vector<std::string>, core::stats::PerfStats> qv;

    // resort because of needed sort for calq
    std::vector<std::string> qualityStrings(output.qvalues.size());

    for (size_t i = 0; i < qualityStrings.size(); ++i) {
        qualityStrings[i] = std::move(output.qvalues[permutation[i]]);
    }

    std::get<0>(qv) = std::move(qualityStrings);
    std::get<1>(qv).addDouble("time-qvcalq", watch.check());
    return qv;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
