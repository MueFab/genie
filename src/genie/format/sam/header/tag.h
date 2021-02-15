/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_HEADER_TAG_H_
#define SRC_GENIE_FORMAT_SAM_HEADER_TAG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

// ---------------------------------------------------------------------------------------------------------------------

enum class SortingOrder : uint8_t { UNKNOWN = 0, UNSORTED = 1, QUERYNAME = 2, COORDINATE = 3 };
enum class Grouping : uint8_t { NONE = 0, QUERY = 1, REFERENCE = 2 };
enum class Topology : uint8_t { LINEAR = 0, CIRCULAR = 1 };
enum class Platform : uint8_t {
    CAPILLARY = 0,
    LS454 = 1,
    ILLUMINA = 2,
    SOLID = 3,
    HELICOS = 4,
    IONTORRENT = 5,
    ONT = 6,
    PACBIO = 7
};

// ---------------------------------------------------------------------------------------------------------------------

class TagBase {
 private:
    std::string name;

 public:
    explicit TagBase(std::string _name);
    const std::string& getName() const;
    virtual std::string toString() const = 0;
    virtual ~TagBase() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& getSortingOrderString();

const std::vector<std::string>& getGroupingOrderString();

const std::vector<std::string>& getTopologyString();

const std::vector<std::string>& getPlatformString();

std::string generateEnumRegex(const std::vector<std::string>& values);

const std::string& getSAMRegex();

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_HEADER_TAG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
