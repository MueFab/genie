#include "raw_reference.h"
#include "make_unique.h"

#include <sstream>
// -----------------------------------------------------------------------------------------------------------------

namespace format {
    // -----------------------------------------------------------------------------------------------------------------

    RawReference::RawReference()
            : DataUnit(DataUnitType::RAW_REFERENCE),
              data_unit_size(0),
              seq_count(0),
              seqs(0) {

        std::stringstream ss;
        BitWriter bw(&ss);
        write(&bw);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void RawReference::addSequence(std::unique_ptr<RawReferenceSequence> ref) {
        for(const auto &a : seqs){
            if(!a->isIdUnique(ref.get())){
                GENIE_THROW_RUNTIME_EXCEPTION("Reference ID is not unique");
            }
        }
        ++seq_count;
        seqs.push_back(std::move(ref));
    }

    std::unique_ptr<RawReference> RawReference::clone() const {
        auto ret = make_unique<RawReference>();
        ret->data_unit_size = data_unit_size;
        ret->seq_count = seq_count;
        for(const auto &a : seqs){
            ret->seqs.push_back(a->clone());
        }
        return ret;
    }

    void RawReference::write(BitWriter *writer) const{
        DataUnit::write(writer);

        uint64_t size = 0;
        for (auto &i : seqs) {
            size += i->getTotalSize();
        }
        size += (8 + 64 + 16) / 8; // data_unit_type, data_unit_size, seq_count
        writer->write(size, 64);
        writer->write(seq_count, 16);

        for (auto &i : seqs) {
            i->write(writer);
        }
    }
}