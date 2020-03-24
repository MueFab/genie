/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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
namespace entropy {
namespace gabac {

EncodingConfiguration::EncodingConfiguration()
    : subseq() {}

EncodingConfiguration::EncodingConfiguration(paramcabac::Subsequence&& _subseq) {
    subseq = std::move(_subseq);
}

EncodingConfiguration::~EncodingConfiguration() = default;

bool EncodingConfiguration::operator==(const EncodingConfiguration& conf) const { return (conf == *this); }
bool EncodingConfiguration::operator!=(const EncodingConfiguration& conf) const { return !(conf == *this); }

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
}  // namespace entropy
}  // namespace genie