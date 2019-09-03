#include "raw_reference.h"


Raw_reference_Sequence::Raw_reference_Sequence() : ref_sequence(seq_end - seq_start + 1) {
}

void Raw_reference_Sequence::write(BitWriter &writer) {
    writer.write(sequence_ID, 16);
    writer.write(seq_start, 40);
    writer.write(seq_end, 40);
    for (auto &i : ref_sequence) {
        writer.write(i, 8);
    }
}


Raw_reference::Raw_reference() : Data_unit(Data_unit_type::raw_reference), seqs(seq_count) {

}

void Raw_reference::write(BitWriter *writer) {
    Data_unit::write(writer);
    writer->write(data_unit_size, 64);
    writer->write(seq_count, 16);
    for (auto &i : seqs) {
        i.write(*writer);
    }
}