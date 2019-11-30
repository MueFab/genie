#ifndef GENIE_SEGMENT_H
#define GENIE_SEGMENT_H

#include <memory>
#include <string>
#include <vector>

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class Segment {
            std::unique_ptr <std::string> sequence;
            // for (qs=0; qs < qv_depth; qs++)
            std::vector <std::unique_ptr<std::string>> quality_values; // or c(1), as specified in subclause 9.2.15

        public:
            Segment();

            explicit Segment(std::unique_ptr <std::string> _sequence);

            Segment(uint32_t length, uint8_t qv_depth, util::BitReader *reader);

            size_t getQvDepth() const;

            size_t getLength() const;

            const std::string* getSequence() const;

            void addQualityValues(std::unique_ptr <std::string> qv);

            virtual void write(util::BitWriter *write) const;

            virtual std::unique_ptr<Segment> clone () const;
        };
    }
}


#endif //GENIE_SEGMENT_H
