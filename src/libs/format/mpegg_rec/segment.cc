#include "segment.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>

namespace format {
    namespace mpegg_rec {

        Segment::Segment(
        ) : sequence(),
            quality_values() {

        }

        Segment::Segment(
                std::unique_ptr<std::string> _sequence
        ) : sequence(std::move(_sequence)),
            quality_values() {

        }

        Segment::Segment(
                uint32_t length,
                uint8_t qv_depth,
                util::BitReader *reader
        ) {
            this->sequence = util::make_unique<std::string>(length, 0);
            for (size_t i = 0; i < length; ++i) {
                (*this->sequence)[i] = reader->read(8);
            }
            for (size_t a = 0; a < qv_depth; ++a) {
                this->quality_values.push_back(util::make_unique<std::string>(length, 0));
                for (size_t i = 0; i < length; ++i) {
                    (*this->quality_values[a])[i] = reader->read(8);
                }
            }
        }

        size_t Segment::getQvDepth() const {
            return quality_values.size();
        }

        size_t Segment::getLength() const {
            return sequence->length();
        }
        const std::string* Segment::getSequence() const {
            return sequence.get();
        }



        void Segment::addQualityValues(std::unique_ptr<std::string> qv) {
            if (qv->length() != sequence->length()) {
                UTILS_DIE("QV and sequence lengths do not match");
            }
            quality_values.push_back(std::move(qv));
        }

        void Segment::write(util::BitWriter *writer) const {
            for (const auto &a : *this->sequence) {
                writer->write(a, 8);
            }
            for (const auto &a : this->quality_values) {
                for (const auto &b : *a) {
                    writer->write(b, 8);
                }
            }
        }

        std::unique_ptr<Segment> Segment::clone() const {
            auto ret = util::make_unique<Segment>();

            ret->sequence = util::make_unique<std::string>(*this->sequence);
            for (const auto &a : this->quality_values) {
                ret->quality_values.push_back(util::make_unique<std::string>(*a));
            }
            return ret;
        }
    }
}