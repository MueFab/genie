/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#include <gtest/gtest.h>
#include <fstream>
#include "genie/core/record/variant_site/record.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"
#include "helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

TEST(VariantSiteRecord, test) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::stringstream inputfile;
    genie::core::record::variant_site::Record variantSite;

}


// ---------------------------------------------------------------------------------------------------------------------
