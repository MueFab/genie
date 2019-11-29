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
            uint32_t ecigar_len : 24;
            std::unique_ptr<std::string> ecigar_string;
            uint8_t reverse_comp : 8;

            // for (as=0; as < as_depth; as++)
            std::vector<int32_t> mapping_score;

        public:
            Alignment(
                    std::unique_ptr<std::string> _ecigar_string,
                    uint8_t _reverse_comp
            );

            size_t getAsDepth() const;

            void addMappingScore(int32_t score);
        };
    }
}


#endif //GENIE_ALIGNMENT_H
