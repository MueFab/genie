/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_SEQ_H_
#define SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_SEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class RawReferenceSequence {
 private:
    uint16_t sequence_ID;      //!< @brief
    uint64_t seq_start;        //!< @brief
    uint64_t seq_end;          //!< @brief
    std::string ref_sequence;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    uint16_t getSeqID() const;

    /**
     * @brief
     * @return
     */
    uint64_t getStart() const;

    /**
     * @brief
     * @return
     */
    uint64_t getEnd() const;

    /**
     * @brief
     * @return
     */
    std::string& getSequence();

    /**
     * @brief
     * @return
     */
    const std::string& getSequence() const;

    /**
     * @brief
     * @param _sequence_ID
     * @param _seq_start
     * @param _ref_sequence
     */
    RawReferenceSequence(uint16_t _sequence_ID, uint64_t _seq_start, std::string&& _ref_sequence);

    /**
     * @brief
     * @param reader
     * @param headerOnly
     */
    explicit RawReferenceSequence(util::BitReader& reader, bool headerOnly);

    /**
     * @brief
     */
    virtual ~RawReferenceSequence() = default;

    /**
     * @brief
     * @param s
     * @return
     */
    bool isIdUnique(const RawReferenceSequence& s) const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    uint64_t getTotalSize() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_SEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
