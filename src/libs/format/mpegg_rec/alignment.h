#ifndef GENIE_ALIGNMENT_H
#define GENIE_ALIGNMENT_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace util {
    class BitWriter;

    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class Alignment {
            std::unique_ptr<std::string> ecigar_string;
            uint8_t reverse_comp : 8;

            // for (as=0; as < as_depth; as++)
            std::vector<int32_t> mapping_score;

        public:
            Alignment(
                    std::unique_ptr<std::string> _ecigar_string,
                    uint8_t _reverse_comp
            );

            Alignment(
                    uint8_t as_depth,
                    util::BitReader* reader
            );

            Alignment(
            );

            size_t getAsDepth() const;

            void addMappingScore(int32_t score);

            int32_t getMappingScore(size_t index) const;

            const std::string* getECigar() const;

            uint8_t getRComp() const;

            virtual void write(util::BitWriter *writer) const;

            virtual std::unique_ptr<Alignment> clone() const;
        };
    }
}


#endif //GENIE_ALIGNMENT_H
