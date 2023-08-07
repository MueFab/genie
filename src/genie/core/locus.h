/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_LOCUS_H_
#define SRC_GENIE_CORE_LOCUS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief Refers to a specific position in the genome
 */
class Locus {
 private:
    std::string refName;  //! @brief Reference sequence / chromosome name
    bool posPresent;      //! @brief If false, no assumptions can be made about start and end
    uint32_t start;       //! @brief First base included in the locus
    uint32_t end;         //! @brief First base not included in the locus

 public:
    /**
     * @brief Getter for reference name
     * @return refName
     */
    const std::string& getRef() const;

    /**
     * @brief Getter for starting position
     * @return start
     */
    uint32_t getStart() const;

    /**
     * @brief Getter for end position
     * @return end
     */
    uint32_t getEnd() const;

    /**
     * @brief If the locus contains any exact position
     * @return If true, start and end are valid.
     */
    bool positionPresent() const;

    /**
     * @brief Constructs a locus without a position [start - end] (sequence name only)
     * @param _ref Sequence name
     */
    explicit Locus(std::string _ref);

    /**
     * @brief Constructs a locus with exact position
     * @param _ref Sequence name
     * @param _start Start position
     * @param _end  End position
     */
    Locus(std::string _ref, uint32_t _start, uint32_t _end);

    /**
     * @brief Parse a locus from a string in format "sequenceName:start-pos" or "sequenceName"
     * @param string Input string
     * @return Constructed locus
     */
    static Locus fromString(const std::string& string);

    /**
     * @brief Convert into string in format "sequenceName:start-pos" or "sequenceName"
     * @return Converted string
     */
    std::string toString() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_LOCUS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
