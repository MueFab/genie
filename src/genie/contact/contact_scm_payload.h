/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
*/

#ifndef GENIE_CONTACT_SCM_PAYLOAD_H
#define GENIE_CONTACT_SCM_PAYLOAD_H

#include <cstdint>
#include <list>
#include <tuple>
#include <optional>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/contact/record.h"
#include "contact_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {


class SCMPayload {
 private:
    uint8_t sample_ID;

 public:
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SCM_PAYLOAD_H
