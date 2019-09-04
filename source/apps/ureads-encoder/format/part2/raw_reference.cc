#include "raw_reference.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    RawReferenceSequence::RawReferenceSequence() : ref_sequence(seq_end - seq_start + 1) {
    }

    // -----------------------------------------------------------------------------------------------------------------

    void RawReferenceSequence::write(BitWriter &writer) {
        writer.write(sequence_ID, 16);
        writer.write(seq_start, 40);
        writer.write(seq_end, 40);
        for (auto &i : ref_sequence) {
            writer.write(i, 8);
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    RawReference::RawReference() : DataUnit(DataUnitType::RAW_REFERENCE), seqs(seq_count) {

    }

    // -----------------------------------------------------------------------------------------------------------------

    void RawReference::write(BitWriter *writer) {
        DataUnit::write(writer);
        writer->write(data_unit_size, 64);
        writer->write(seq_count, 16);
        for (auto &i : seqs) {
            i.write(*writer);
        }
    }
}