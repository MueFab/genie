/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TAG_H
#define GENIE_TAG_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {
namespace header {

/**
 *
 */
enum class SortingOrder : uint8_t { UNKNOWN = 0, UNSORTED = 1, QUERYNAME = 2, COORDINATE = 3 };

/**
 *
 */
enum class Grouping : uint8_t { NONE = 0, QUERY = 1, REFERENCE = 2 };

/**
 *
 */
enum class Topology : uint8_t { LINEAR = 0, CIRCULAR = 1 };

/**
 *
 */
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

/**
 *
 */
class TagBase {
   private:
    std::string name;  //!<

   public:
    /**
     *
     * @param _name
     */
    explicit TagBase(std::string _name);

    /**
     *
     * @return
     */
    const std::string& getName() const;

    /**
     *
     * @return
     */
    virtual std::string toString() const = 0;

    /**
     *
     */
    virtual ~TagBase() = default;
};

/**
 *
 * @return
 */
const std::vector<std::string>& getSortingOrderString();

/**
 *
 * @return
 */
const std::vector<std::string>& getGroupingOrderString();

/**
 *
 * @return
 */
const std::vector<std::string>& getTopologyString();

/**
 *
 * @return
 */
const std::vector<std::string>& getPlatformString();

/**
 *
 * @param values
 * @return
 */
std::string generateEnumRegex(const std::vector<std::string>& values);

/**
 *
 * @return
 */
const std::string& getSAMRegex();

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace header
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TAG_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------