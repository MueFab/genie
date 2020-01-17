/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LOCUS_H
#define GENIE_LOCUS_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

class Locus {
   private:
    std::string refName;
    bool posPresent;
    uint32_t start;
    uint32_t end;

   public:
    const std::string& getRef() const;
    uint32_t getStart() const;
    uint32_t getEnd() const;
    bool positionPresent() const;
    explicit Locus(std::string _ref);

    Locus(std::string _ref, uint32_t _start, uint32_t _end);
    static Locus fromString(const std::string& string);

    std::string toString() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_LOCUS_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------