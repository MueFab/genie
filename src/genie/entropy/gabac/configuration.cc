/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/configuration.h"
#include <utility>
#include "genie/entropy/gabac/config-manual.h"
#include "genie/entropy/gabac/encode-desc-subseq.h"
#include "genie/entropy/gabac/stream-handler.h"
#include "genie/entropy/gabac/streams.h"
#include "genie/entropy/paramcabac/transformed-subseq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration() : subseqCfg() {}

// ---------------------------------------------------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration(const core::GenSubIndex sub) { subseqCfg = getEncoderConfigManual(sub); }

// ---------------------------------------------------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration(paramcabac::Subsequence&& _subseq) { subseqCfg = _subseq; }

// ---------------------------------------------------------------------------------------------------------------------

EncodingConfiguration::~EncodingConfiguration() = default;

// ---------------------------------------------------------------------------------------------------------------------

bool EncodingConfiguration::operator==(const EncodingConfiguration& conf) const {
    return (conf.subseqCfg == this->subseqCfg);
}

// ---------------------------------------------------------------------------------------------------------------------

bool EncodingConfiguration::operator!=(const EncodingConfiguration& conf) const { return !(conf == *this); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t EncodingConfiguration::getSubseqWordSize() const {
    const std::vector<paramcabac::TransformedSubSeq>& trnsfCfgs = subseqCfg.getTransformSubseqCfgs();

    switch (subseqCfg.getTransformParameters().getTransformIdSubseq()) {
        case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            return paramcabac::StateVars::getMinimalSizeInBytes(trnsfCfgs[0].getSupportValues().getOutputSymbolSize());
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            return paramcabac::StateVars::getMinimalSizeInBytes(trnsfCfgs[1].getSupportValues().getOutputSymbolSize());
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            return paramcabac::StateVars::getMinimalSizeInBytes(trnsfCfgs[2].getSupportValues().getOutputSymbolSize());
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            return paramcabac::StateVars::getMinimalSizeInBytes(trnsfCfgs[1].getSupportValues().getOutputSymbolSize());
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING: {
            const std::vector<uint8_t> subseqShiftSizes = subseqCfg.getTransformParameters().getMergeCodingShiftSizes();
            return paramcabac::StateVars::getMinimalSizeInBytes(subseqShiftSizes[0] +
                                                                trnsfCfgs[0].getSupportValues().getOutputSymbolSize());
        } break;
        default:
            UTILS_DIE("Invalid subseq transforamtion");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const paramcabac::Subsequence& EncodingConfiguration::getSubseqConfig() const { return subseqCfg; }

// ---------------------------------------------------------------------------------------------------------------------

void EncodingConfiguration::setSubseqConfig(paramcabac::Subsequence&& _subseqCfg) { subseqCfg = _subseqCfg; }

// ---------------------------------------------------------------------------------------------------------------------

void IOConfiguration::validate() const {
    if (!inputStream) {
        UTILS_DIE("Invalid input stream");
    }
    if (!outputStream) {
        UTILS_DIE("Invalid output stream");
    }
    if (!logStream) {
        UTILS_DIE("Invalid logging output stream");
    }
    if (unsigned(this->level) > unsigned(IOConfiguration::LogLevel::LOG_FATAL)) {
        UTILS_DIE("Invalid logging level");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& IOConfiguration::log(const LogLevel& l) const {
    static NullStream nullstr;
    if (static_cast<int>(l) >= static_cast<int>(level)) {
        return *logStream;
    }
    return nullstr;
}

// ---------------------------------------------------------------------------------------------------------------------

EncodingConfiguration::EncodingConfiguration(nlohmann::json j) : subseqCfg(std::move(j)) {}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json EncodingConfiguration::toJson() const { return subseqCfg.toJoson(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
