/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "configuration.h"

#include <iostream>
#include <sstream>

#include <json@53c3eef/json.hpp>

#include "constants.h"
#include "exceptions.h"
#include "stream-handler.h"
#include "streams.h"

namespace genie {
namespace gabac {

EncodingConfiguration::EncodingConfiguration()
    : wordSize(0),
      sequenceTransformationId(gabac::SequenceTransformationId::no_transform),
      sequenceTransformationParameter(0),
      transformedSequenceConfigurations() {}

EncodingConfiguration::EncodingConfiguration(const std::string& jsonstring) {
    using nlohmann::json;
    try {
        auto jtree = json::parse(jsonstring);
        this->wordSize = jtree["word_size"];
        this->sequenceTransformationId = jtree["sequence_transformation_id"];
        this->sequenceTransformationParameter = jtree["sequence_transformation_parameter"];

        for (const auto& child : jtree["transformed_sequences"]) {
            // Declare a transformed sequence configuration
            TransformedSequenceConfiguration transformedSequenceConfiguration;

            // Fill the transformed sequence configuration
            transformedSequenceConfiguration.lutTransformationEnabled = child["lut_transformation_enabled"];
            transformedSequenceConfiguration.lutBits = wordSize * 8;
            transformedSequenceConfiguration.lutOrder = 0;
            if (transformedSequenceConfiguration.lutTransformationEnabled) {
                if (child.count("lut_transformation_bits") > 0) {
                    transformedSequenceConfiguration.lutBits = child["lut_transformation_bits"];
                }
                if (child.count("lut_transformation_order") > 0) {
                    transformedSequenceConfiguration.lutOrder = child["lut_transformation_order"];
                }
            } else {
                transformedSequenceConfiguration.lutBits = 0;
                transformedSequenceConfiguration.lutOrder = 0;
            }
            transformedSequenceConfiguration.diffCodingEnabled = child["diff_coding_enabled"];
            transformedSequenceConfiguration.binarizationId = child["binarization_id"];
            for (const auto& grandchild : child["binarization_parameters"]) {
                transformedSequenceConfiguration.binarizationParameters.push_back(grandchild);
            }
            transformedSequenceConfiguration.contextSelectionId = child["context_selection_id"];

            // Append the filled transformed sequence configuration to our
            // list of transformed sequence configurations
            this->transformedSequenceConfigurations.push_back(transformedSequenceConfiguration);
        }
    } catch (json::exception& e) {
        GABAC_DIE("JSON parsing error: " + std::string(e.what()));
    }
}

EncodingConfiguration::~EncodingConfiguration() = default;

std::string EncodingConfiguration::toJsonString() const {
    using nlohmann::json;
    try {
        json root;
        root["word_size"] = this->wordSize;
        root["sequence_transformation_id"] = this->sequenceTransformationId;
        root["sequence_transformation_parameter"] = this->sequenceTransformationParameter;

        json sequenceList;
        for (const auto& transformedSequenceConfiguration : this->transformedSequenceConfigurations) {
            json curSequence;

            // Fill the property tree for the transformed sequence
            // configuration

            curSequence["lut_transformation_enabled"] = transformedSequenceConfiguration.lutTransformationEnabled;

            if (transformedSequenceConfiguration.lutTransformationEnabled) {
                curSequence["lut_transformation_bits"] = transformedSequenceConfiguration.lutBits;
                curSequence["lut_transformation_order"] = transformedSequenceConfiguration.lutOrder;
            }

            curSequence["diff_coding_enabled"] = transformedSequenceConfiguration.diffCodingEnabled;
            curSequence["binarization_id"] = transformedSequenceConfiguration.binarizationId;

            curSequence["binarization_parameters"] = transformedSequenceConfiguration.binarizationParameters;

            curSequence["context_selection_id"] = transformedSequenceConfiguration.contextSelectionId;

            sequenceList.push_back(curSequence);
        }
        root["transformed_sequences"] = sequenceList;
        return root.dump(4);
    } catch (json::exception& e) {
        GABAC_DIE("JSON parsing error: " + std::string(e.what()));
    }
}

std::string EncodingConfiguration::toPrintableString() const {
    std::stringstream s;

    s << this->wordSize << "  |  ";
    s << static_cast<int>(this->sequenceTransformationId) << "  |  ";
    s << this->sequenceTransformationParameter << "  |  ";
    for (const auto& transformedSequenceConfiguration : this->transformedSequenceConfigurations) {
        s << transformedSequenceConfiguration.toPrintableString();
    }

    return s.str();
}

std::string TransformedSequenceConfiguration::toPrintableString() const {
    std::stringstream s;
    s << "[";
    s << static_cast<int>(lutTransformationEnabled) << "  |  ";
    s << static_cast<int>(lutBits) << "  |  ";
    s << static_cast<int>(lutOrder) << "  |  ";
    s << static_cast<int>(diffCodingEnabled) << "  |  ";
    s << static_cast<int>(binarizationId) << "  |  ";
    s << "[ ";
    for (const auto& binarizationParameter : binarizationParameters) {
        s << binarizationParameter << " ";
    }
    s << "]  |  ";
    s << static_cast<int>(contextSelectionId);
    s << "]";
    return s.str();
}

bool TransformedSequenceConfiguration::operator==(const TransformedSequenceConfiguration& conf) const {
    return conf.diffCodingEnabled == this->diffCodingEnabled &&
           (conf.lutTransformationEnabled ? conf.lutOrder == this->lutOrder : true) &&
           conf.lutTransformationEnabled == this->lutTransformationEnabled &&
           conf.binarizationId == this->binarizationId && conf.binarizationParameters == this->binarizationParameters &&
           conf.contextSelectionId == this->contextSelectionId &&
           (conf.lutTransformationEnabled ? conf.lutBits == this->lutBits : true);
}
bool TransformedSequenceConfiguration::operator!=(const TransformedSequenceConfiguration& conf) const {
    return !(*this == conf);
}

void generalizeLUT(gabac::EncodingConfiguration& ret, uint64_t max, unsigned index) {
    const size_t MAX_LUT_SIZE = 1u << 20u;
    if (!ret.transformedSequenceConfigurations[index].lutTransformationEnabled) {
        return;
    }

    // Calculate how many bits we need to represent the maximum value
    unsigned bits = uint8_t(std::ceil(std::log2(max + 1)));

    // Lut bits must be at least the number of calculated bits
    ret.transformedSequenceConfigurations[index].lutBits =
        std::min(std::max(bits, ret.transformedSequenceConfigurations[index].lutBits), 32u);

    // Maximum value too high for LUT. Indices higher 0 encode lengths, these
    // can never exceed The Lut maximum for order 0, as this would require
    // 1+2+3+..+8000000 symbols, which is at least 32TB
    if (max > MAX_LUT_SIZE && index == 0) {
        ret.transformedSequenceConfigurations[index].lutTransformationEnabled = false;
    } else if (max > size_t(std::sqrt(MAX_LUT_SIZE))) {
        // Too high for order 1
        ret.transformedSequenceConfigurations[index].lutOrder = 0;
    } else if (max > size_t(std::pow(MAX_LUT_SIZE, 1.0 / 3.0))) {
        // Too high for order 2
        ret.transformedSequenceConfigurations[index].lutOrder =
            std::min(1u, ret.transformedSequenceConfigurations[index].lutOrder);
    }
}

void optimizeLUT(gabac::EncodingConfiguration& ret, uint64_t max, unsigned index) {
    if (!ret.transformedSequenceConfigurations[index].lutTransformationEnabled) {
        return;
    }

    // Lut bits has an optimal value
    ret.transformedSequenceConfigurations[index].lutBits =
        std::min(uint8_t(std::ceil(std::log2(max + 1))), uint8_t(ret.transformedSequenceConfigurations[index].lutBits));
    ret.transformedSequenceConfigurations[index].lutBits =
        std::min(ret.transformedSequenceConfigurations[index].lutBits, 32u);
}

void generalizeBin(gabac::EncodingConfiguration& c1, uint64_t max, unsigned index) {
    unsigned bits = 0;
    const unsigned TU_MAX = gabac::getBinarization(gabac::BinarizationId::TU).paramMax;
    switch (c1.transformedSequenceConfigurations[index].binarizationId) {
        case gabac::BinarizationId::BI:
            bits = uint8_t(std::ceil(std::log2(max + 1)));

            // Correct too few bits for BI binarization, or too many
            bits = std::min(std::max(bits, c1.transformedSequenceConfigurations[index].binarizationParameters[0]), 32u);
            c1.transformedSequenceConfigurations[index].binarizationParameters = {bits};
            break;
        case gabac::BinarizationId::TU:
            if (max > TU_MAX) {
                // Correct too high values for TU: switch to TEG
                // Set invalid parameters and call recursively to fix it.
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::TEG;
                c1.transformedSequenceConfigurations[index].binarizationParameters = {32};

                // Check newly created binarization
                generalizeBin(c1, max, index);
            } else {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {
                    std::max(unsigned(max), c1.transformedSequenceConfigurations[index].binarizationParameters[0])};
            }
            break;
        case gabac::BinarizationId::EG:
            // Correct too high values for EG: switch to BI instead
            if (max > gabac::getBinarization(gabac::BinarizationId::EG).max(0)) {
                // Set invalid parameters and call recursively to fix it.
                c1.transformedSequenceConfigurations[index].binarizationParameters = {0};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::BI;
                generalizeBin(c1, max, index);
            }
            break;
        case gabac::BinarizationId::SEG:
            // Guaranteed to work if [-max, max] in range, which is
            // automatically the case if the positive range is working
            if (max > gabac::getBinarization(gabac::BinarizationId::SEG).max(0)) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {0};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::BI;
                generalizeBin(c1, max, index);
            }
            break;
        case gabac::BinarizationId::TEG:
            // Check if parameter is in TU range
            if (TU_MAX < c1.transformedSequenceConfigurations[index].binarizationParameters[0]) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {TU_MAX};
            }
            // Guaranteed to work if [-max, max] in range, which is
            // automatically the case if the positive range is working
            if (max > gabac::getBinarization(gabac::BinarizationId::TEG).max(0)) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {0};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::BI;
                generalizeBin(c1, max, index);
            }

            break;
        case gabac::BinarizationId::STEG:
            // Check if parameter is in TU range
            if (TU_MAX < c1.transformedSequenceConfigurations[index].binarizationParameters[0]) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {TU_MAX};
            }
            // Guaranteed to work if [-max, max] in range, which is
            // automatically the case if the positive range is working
            if (max > gabac::getBinarization(gabac::BinarizationId::STEG).max(0)) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {0};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::BI;
                generalizeBin(c1, max, index);
            }
            break;
        default:
            return;
    }
}

void optimizeBin(gabac::EncodingConfiguration& c1, uint64_t max, unsigned index) {
    unsigned bits = 0;
    switch (c1.transformedSequenceConfigurations[index].binarizationId) {
        case gabac::BinarizationId::BI:
            bits = std::min(uint8_t(std::ceil(std::log2(max + 1))),
                            uint8_t(c1.transformedSequenceConfigurations[index].binarizationParameters[0]));

            // Use optimal value for bits
            c1.transformedSequenceConfigurations[index].binarizationParameters = {std::min(unsigned(bits), 32u)};
            break;
        case gabac::BinarizationId::TU:
            c1.transformedSequenceConfigurations[index].binarizationParameters = {std::min(unsigned(max), 32u)};
            break;
        case gabac::BinarizationId::TEG:
            // TEG 0 is not allowed in the standard and EG is faster.
            if (c1.transformedSequenceConfigurations[index].binarizationParameters[0] == 0) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {0};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::EG;
                optimizeBin(c1, max, index);
            } else if (c1.transformedSequenceConfigurations[index].binarizationParameters[0] > max) {
                // No reason to have a parameter this large
                c1.transformedSequenceConfigurations[index].binarizationParameters = {unsigned(max)};
            }
            break;
        case gabac::BinarizationId::STEG:
            // STEG 0 is not allowed in the standard and SEG is faster.
            if (c1.transformedSequenceConfigurations[index].binarizationParameters[0] == 0) {
                c1.transformedSequenceConfigurations[index].binarizationParameters = {0};
                c1.transformedSequenceConfigurations[index].binarizationId = gabac::BinarizationId::SEG;
                optimizeBin(c1, max, index);
            } else if (c1.transformedSequenceConfigurations[index].binarizationParameters[0] > max) {
                // No reason to have a parameter this large
                c1.transformedSequenceConfigurations[index].binarizationParameters = {unsigned(max)};
            }
            break;
        default:
            return;
    }
}

gabac::EncodingConfiguration EncodingConfiguration::optimize(uint64_t max, unsigned) const {
    // Start with the current configuration
    gabac::EncodingConfiguration ret = *this;

    if (max > std::numeric_limits<uint32_t>::max()) {
        GABAC_DIE(
            "Gabac does not support values that are not representable in 32 "
            "bits.");
    }

    optimizeLUT(ret, max, 0);

    // Update binarization
    optimizeBin(ret, max, 0);

    // Special cases for single transforms
    switch (ret.sequenceTransformationId) {
        case gabac::SequenceTransformationId::equality_coding:

            // Just single bits is the optimal configuration
            ret.transformedSequenceConfigurations[1].lutBits =
                std::min(1u, ret.transformedSequenceConfigurations[1].lutBits);
            optimizeLUT(ret, 1, 1);
            optimizeBin(ret, 1, 1);
            break;
        case gabac::SequenceTransformationId::match_coding:

            // Optimal configuration is the window size
            ret.transformedSequenceConfigurations[1].lutBits =
                std::min(uint8_t(ret.transformedSequenceConfigurations[1].lutBits),
                         uint8_t(std::ceil(std::log2(ret.sequenceTransformationParameter + 1))));
            optimizeLUT(ret, ret.sequenceTransformationParameter, 1);
            optimizeBin(ret, ret.sequenceTransformationParameter, 1);

            // Stream 3 contains lengths, these can be up to 32 bits
            ret.transformedSequenceConfigurations[2].lutBits =
                std::min(32u, ret.transformedSequenceConfigurations[2].lutBits);
            optimizeLUT(ret, std::numeric_limits<uint32_t>::max(), 2);
            optimizeBin(ret, std::numeric_limits<uint32_t>::max(), 2);
            break;
        case gabac::SequenceTransformationId::rle_coding:

            // Here we have 32 bit lengths again
            ret.transformedSequenceConfigurations[1].lutBits =
                std::min(32u, ret.transformedSequenceConfigurations[1].lutBits);
            optimizeLUT(ret, ret.sequenceTransformationParameter, 1);
            optimizeBin(ret, ret.sequenceTransformationParameter, 1);
            break;
        default:
            break;
    }

    return ret;
}

gabac::EncodingConfiguration EncodingConfiguration::generalize(uint64_t max, unsigned wordsize) const {
    // Start with the current configuration
    gabac::EncodingConfiguration ret = *this;

    if (max > std::numeric_limits<uint32_t>::max()) {
        GABAC_DIE(
            "Gabac does not support values that are not representable in 32 "
            "bits.");
    }

    // Word size might be too big
    ret.wordSize = std::min(ret.wordSize, wordsize);

    if (ret.wordSize == 8) {
        ret.transformedSequenceConfigurations[0].diffCodingEnabled = false;
    }

    generalizeLUT(ret, max, 0);

    // Update binarization
    generalizeBin(ret, max, 0);

    // Special cases for single transforms
    switch (ret.sequenceTransformationId) {
        case gabac::SequenceTransformationId::equality_coding:

            // Just single byte/bit flags
            ret.transformedSequenceConfigurations[1].lutBits =
                std::max(1u, ret.transformedSequenceConfigurations[1].lutBits);
            generalizeLUT(ret, 1, 1);
            generalizeBin(ret, 1, 1);
            break;
        case gabac::SequenceTransformationId::match_coding:

            // Stream 2 contains only indices in the range of the window size
            // So we now a worst case for the values and can calculate the
            // lut bits and binarizations
            ret.transformedSequenceConfigurations[1].lutBits =
                std::max(uint8_t(std::ceil(std::log2(ret.sequenceTransformationParameter + 1))),
                         uint8_t(ret.transformedSequenceConfigurations[1].lutBits));

            generalizeLUT(ret, ret.sequenceTransformationParameter, 1);
            generalizeBin(ret, ret.sequenceTransformationParameter, 1);

            // Stream 3 contains lengths, these can be up to 32 bits
            ret.transformedSequenceConfigurations[2].lutBits =
                std::max(32u, ret.transformedSequenceConfigurations[2].lutBits);
            generalizeLUT(ret, std::numeric_limits<uint32_t>::max(), 2);
            generalizeBin(ret, std::numeric_limits<uint32_t>::max(), 2);
            break;
        case gabac::SequenceTransformationId::rle_coding:

            // Here we have 32 bit lengths again
            ret.transformedSequenceConfigurations[1].lutBits =
                std::max(32u, ret.transformedSequenceConfigurations[1].lutBits);
            generalizeLUT(ret, ret.sequenceTransformationParameter, 1);
            generalizeBin(ret, ret.sequenceTransformationParameter, 1);
            break;
        default:
            break;
    }

    return ret;
}

bool EncodingConfiguration::operator==(const EncodingConfiguration& conf) const {
    return conf.wordSize == this->wordSize && conf.sequenceTransformationId == this->sequenceTransformationId &&
           conf.sequenceTransformationParameter == this->sequenceTransformationParameter &&
           conf.transformedSequenceConfigurations == this->transformedSequenceConfigurations;
}
bool EncodingConfiguration::operator!=(const EncodingConfiguration& conf) const { return !(conf == *this); }

bool EncodingConfiguration::isGeneral(uint64_t max, unsigned wordsize) const {
    try {
        return *this == this->generalize(max, wordsize);
    } catch (...) {
        return false;
    }
}

bool EncodingConfiguration::isSubGeneral(uint64_t max, unsigned sub) const {
    try {
        return this->transformedSequenceConfigurations[sub] ==
               this->generalize(max, this->wordSize).transformedSequenceConfigurations[sub];
    } catch (...) {
        return false;
    }
}
bool EncodingConfiguration::isOptimal(uint64_t max, unsigned wordsize) const {
    try {
        return *this == this->optimize(max, wordsize);
    } catch (...) {
        return false;
    }
}

void IOConfiguration::validate() const {
    if (!inputStream) {
        GABAC_DIE("Invalid input stream");
    }
    if (!outputStream) {
        GABAC_DIE("Invalid output stream");
    }
    if (!logStream) {
        GABAC_DIE("Invalid logging output stream");
    }
    if (unsigned(this->level) > unsigned(IOConfiguration::LogLevel::FATAL)) {
        GABAC_DIE("Invalid logging level");
    }
}

std::ostream& IOConfiguration::log(const LogLevel& l) const {
    static NullStream nullstr;
    if (static_cast<int>(l) >= static_cast<int>(level)) {
        return *logStream;
    }
    return nullstr;
}

}  // namespace gabac
}  // namespace genie
