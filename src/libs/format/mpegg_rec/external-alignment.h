#ifndef GENIE_EXTERNAL_ALIGNMENT_H
#define GENIE_EXTERNAL_ALIGNMENT_H

#include <cstdint>
#include <memory>

namespace util {
    class BitWriter;

    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class ExternalAlignment {
        public:
            enum class MoreAlignmentInfoType : uint8_t {
                NONE,
                OTHER_REC
            };

            explicit ExternalAlignment(MoreAlignmentInfoType _moreAlignmentInfoType);

            virtual ~ExternalAlignment() = default;

            virtual void write(util::BitWriter *writer) const;

            static std::unique_ptr<ExternalAlignment> factory(util::BitReader *reader);

            virtual std::unique_ptr<ExternalAlignment> clone () const = 0;

        protected:
            MoreAlignmentInfoType moreAlignmentInfoType;
        };
    }
}


#endif //GENIE_EXTERNAL_ALIGNMENT_H
