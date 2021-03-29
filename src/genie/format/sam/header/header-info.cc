/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/header/header-info.h"
#include <vector>
#include "genie/core/locus.h"
#include "genie/format/sam/header/tag-enum.h"
#include "genie/format/sam/header/tag-number.h"
#include "genie/format/sam/header/tag-object.h"
#include "genie/format/sam/header/tag-string-list.h"
#include "genie/format/sam/header/tag-string.h"
#include "genie/format/sam/header/tag-version.h"
#include "genie/util/date.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<HeaderLineInfo>& getHeaderInfo() {
    const auto& _1 = std::placeholders::_1;
    const auto& _2 = std::placeholders::_2;
    static const std::vector<HeaderLineInfo> headerInfo = {
        {"HD",
         {{"VN", true, "^[0-9]+\\.[0-9]+$", &TagVersion::fromString},
          {"SO", false, generateEnumRegex(getSortingOrderString()),
           std::bind(&TagEnum<SortingOrder>::fromString, _1, _2, getSortingOrderString())},
          {"GO", false, generateEnumRegex(getGroupingOrderString()),
           std::bind(&TagEnum<Grouping>::fromString, _1, _2, getGroupingOrderString())},
          {"SS", false, "(coordinate|queryname|unsorted)(:[A-Za-z0-9_-]+)+",
           std::bind(&TagStringList::fromString, _1, _2, ':')}}},

        {"SQ",
         {{"SN", true, getSAMRegex(), &TagString::fromString},
          {"LN", true, "[0-9]+", &TagNumber::fromString},
          {"AH", false, "", &TagObject<core::Locus>::fromString},
          {"AN", false, getSAMRegex() + "(," + getSAMRegex() + ")*", &TagString::fromString},
          {"AS", false, getSAMRegex(), &TagString::fromString},
          {"DS", false, getSAMRegex(), &TagString::fromString},
          {"M5", false, "^[a-f0-9]{32}$", &TagString::fromString},
          {"SP", false, getSAMRegex(), &TagString::fromString},
          {"TP", false, generateEnumRegex(getTopologyString()),
           std::bind(&TagEnum<Topology>::fromString, _1, _2, getTopologyString())},
          {"UR", false, getSAMRegex(), &TagString::fromString}}},

        {"RG",
         {{"ID", true, getSAMRegex(), &TagString::fromString},
          {"BC", false, "[ACMGRSVTWYHKDBN]+", &TagString::fromString},
          {"CN", false, getSAMRegex(), &TagString::fromString},
          {"DS", false, getSAMRegex(), &TagString::fromString},
          {"DT", false, util::Date::getRegex(), &TagObject<util::Date>::fromString},
          {"FO", false, "\\*|[ACMGRSVTWYHKDBN]+", &TagString::fromString},
          {"KS", false, "[ACMGRSVTWYHKDBN]+", &TagString::fromString},
          {"LB", false, getSAMRegex(), &TagString::fromString},
          {"PG", false, getSAMRegex(), &TagString::fromString},
          {"PI", false, "[0-9]+", &TagNumber::fromString},
          {"PL", false, generateEnumRegex(getPlatformString()),
           std::bind(&TagEnum<Platform>::fromString, _1, _2, getPlatformString())},
          {"PM", false, getSAMRegex(), &TagString::fromString},
          {"PU", false, getSAMRegex(), &TagString::fromString},
          {"SM", false, getSAMRegex(), &TagString::fromString}}},

        {"PG",
         {{"ID", true, getSAMRegex(), &TagString::fromString},
          {"PN", false, getSAMRegex(), &TagString::fromString},
          {"CL", false, "", &TagString::fromString},
          {"PP", false, getSAMRegex(), &TagString::fromString},
          {"DS", false, getSAMRegex(), &TagString::fromString},
          {"VN", false, getSAMRegex(), &TagString::fromString}}}};
    return headerInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
