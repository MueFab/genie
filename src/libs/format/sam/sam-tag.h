#ifndef GENIE_SAM_TAG_H
#define GENIE_SAM_TAG_H

#include <util/exceptions.h>
#include <util/make_unique.h>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace format {
namespace sam {

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

class TagBase {
   private:
    std::string name;

   public:
    explicit TagBase(std::string _name);
    const std::string& getName() const;
    virtual std::string toString() const = 0;
    virtual ~TagBase() = default;
};

const std::vector<std::string>& getSortingOrderString();

const std::vector<std::string>& getGroupingOrderString();

const std::vector<std::string>& getTopologyString();

const std::vector<std::string>& getPlatformString();

std::string generateEnumRegex(const std::vector<std::string>& values);

const std::string& getSAMRegex();

}  // namespace sam
}  // namespace format

#endif  // GENIE_SAM_TAG_H
