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
            explicit Segment(std::unique_ptr <std::string> _sequence);

            explicit Segment(uint32_t length, util::BitReader *reader);

            size_t getQvDepth() const;

            size_t getLength() const;

            void addQualityValues(std::unique_ptr <std::string> qv);

            virtual void write(util::BitWriter *write) const;
        };
    }
}


#endif //GENIE_SEGMENT_H
