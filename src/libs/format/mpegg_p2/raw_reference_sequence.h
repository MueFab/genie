#ifndef GENIE_RAW_REFERENCE_SEQUENCE_H
#define GENIE_RAW_REFERENCE_SEQUENCE_H

#include <memory>
#include <string>
#include "util/bitwriter.h"

namespace format {
    namespace mpegg_p2 {
/**
 * ISO 23092-2 Section 3.2 table 5, lines 4 to 7
 */
        class RawReferenceSequence {
        private:
            uint16_t sequence_ID : 16;  //!< Line 4
            uint64_t seq_start : 40;    //!< Line 5
            //!< uint64_t seq_end : 40; //!< Line 6 currently computed by length of string
            std::unique_ptr<std::string> ref_sequence;  //!< Line 7

        public:
            RawReferenceSequence(uint16_t _sequence_ID, uint64_t _seq_start,
                                 std::unique_ptr<std::string> _ref_sequence);

            virtual ~RawReferenceSequence() = default;

            bool isIdUnique(const RawReferenceSequence *s) const;

            virtual std::unique_ptr<RawReferenceSequence> clone() const;

            void write(util::BitWriter &writer) const;

            uint64_t getTotalSize() const;
        };
    }
}  // namespace format

#endif  // GENIE_RAW_REFERENCE_SEQUENCE_H
