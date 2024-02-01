/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(util::BitReader &reader) {

}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getSampleID() const {return sample_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getSampleName() const {return sample_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getChr1ID() const {return chr1_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getChr1Name() const {return chr1_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getChr2ID() const {return chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getChr2Name() const {return chr2_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::getNumCounts() const {return counts.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getNormCounts() const {return norm_count_names.size();}


void ContactRecord::write(util::BitWriter &writer) const {}
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
