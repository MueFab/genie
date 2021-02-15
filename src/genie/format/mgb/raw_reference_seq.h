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
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class RawReferenceSequence {
 private:
    uint16_t sequence_ID;      //!<
    uint64_t seq_start;        //!<
    uint64_t seq_end;          //!<
    std::string ref_sequence;  //!<

 public:
    /**
     *
     * @return
     */
    uint16_t getSeqID() const;

    /**
     *
     * @return
     */
    uint64_t getStart() const;

    /**
     *
     * @return
     */
    uint64_t getEnd() const;

    /**
     *
     * @return
     */
    std::string& getSequence();

    /**
     *
     * @return
     */
    const std::string& getSequence() const;

    /**
     *
     * @param _sequence_ID
     * @param _seq_start
     * @param _ref_sequence
     */
    RawReferenceSequence(uint16_t _sequence_ID, uint64_t _seq_start, std::string&& _ref_sequence);

    /**
     *
     * @param reader
     * @param headerOnly
     */
    explicit RawReferenceSequence(util::BitReader& reader, bool headerOnly);

    /**
     *
     */
    virtual ~RawReferenceSequence() = default;

    /**
     *
     * @param s
     * @return
     */
    bool isIdUnique(const RawReferenceSequence& s) const;

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     *
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
