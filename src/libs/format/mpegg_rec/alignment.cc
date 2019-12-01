#include "alignment.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

namespace format {
    namespace mpegg_rec {
        Alignment::Alignment(
                std::unique_ptr<std::string> _ecigar_string,
                uint8_t _reverse_comp
        ) : ecigar_string(std::move(_ecigar_string)),
            reverse_comp(_reverse_comp),
            mapping_score() {

        }

        Alignment::Alignment(
                uint8_t as_depth,
                util::BitReader *reader
        ) {
            ecigar_string = util::make_unique<std::string>(reader->read(24), '\0');
            for (auto &c : *ecigar_string) {
                c = reader->read(8);
            }

            reverse_comp = reader->read(8);
            for (size_t a = 0; a < as_depth; ++a) {
                mapping_score.push_back(reader->read(32));
            }
        }

        Alignment::Alignment(
        ) : ecigar_string(),
            reverse_comp(0),
            mapping_score(0) {

        }

        size_t Alignment::getAsDepth() const {
            return mapping_score.size();
        }

        void Alignment::addMappingScore(int32_t score) {
            mapping_score.push_back(score);
        }

        int32_t Alignment::getMappingScore(size_t index) const {
            return mapping_score[index];
        }

        const std::string* Alignment::getECigar() const {
            return ecigar_string.get();
        }

        uint8_t Alignment::getRComp() const {
            return this->reverse_comp;
        }

        void Alignment::write(util::BitWriter *writer) const {
            writer->write(ecigar_string->length(), 24);
            for (const auto &c : *ecigar_string) {
                writer->write(c, 8);
            }

            writer->write(reverse_comp, 8);
            for (const auto &q : mapping_score) {
                writer->write(q, 32);
            }
        }

        std::unique_ptr<Alignment> Alignment::clone() const {
            //TODO
            return std::unique_ptr<Alignment>();
        }
    }
}