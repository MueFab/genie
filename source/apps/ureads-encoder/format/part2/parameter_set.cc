#include "parameter_set.h"
#include "ureads-encoder/exceptions.h"

#include <sstream>

#include "ureads-encoder/format/part2/make_unique.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration.h"
#include "ureads-encoder/format/part2/parameter_set/descriptor_configuration_container.h"
#include "ureads-encoder/format/part2/parameter_set/qv_coding_config.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    ParameterSet::ParameterSet(
            uint8_t _parameter_set_ID,
            uint8_t _parent_parameter_set_ID,
            DatasetType _dataset_type,
            AlphabetID _alphabet_id,
            uint32_t _read_length,
            bool _paired_end,
            bool _pos_40_bits_flag,
            uint8_t _qv_depth,
            uint8_t _as_depth,
            bool _multiple_alignments_flag,
            bool _spliced_reads_flag
    ) : DataUnit(DataUnitType::PARAMETER_SET),
        reserved(0),
        data_unit_size(0),
        parameter_set_ID(_parameter_set_ID),
        parent_parameter_set_ID(_parent_parameter_set_ID),
        dataset_type(_dataset_type),
        alphabet_ID(_alphabet_id),
        read_length(_read_length),
        number_of_template_segments_minus1(_paired_end),
        reserved_2(0),
        max_au_data_unit_size(0),
        pos_40_bits_flag(_pos_40_bits_flag),
        qv_depth(_qv_depth),
        as_depth(_as_depth),
        num_classes(0),
        class_IDs(0),
        descriptors(18),
        num_groups(0),
        rgroup_IDs(0),
        multiple_alignments_flag(_multiple_alignments_flag),
        spliced_reads_flag(_spliced_reads_flag),
        multiple_signature_base(0),
        u_signature_size(nullptr),
        qv_coding_configs(0),
        crps_flag(false),
        parameter_set_crps(nullptr),
        internalBitCounter(0) {
        for (auto &i : descriptors) {
            i = make_unique<DescriptorConfigurationContainer>();
        }
        std::stringstream s;
        BitWriter bw(&s);
        write(&bw);
        addSize(bw.getBitsWritten());
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::write(BitWriter *writer) {
        DataUnit::write(writer);
        writer->write(reserved, 10);
        writer->write(data_unit_size, 22);
        writer->write(parameter_set_ID, 8);
        writer->write(parent_parameter_set_ID, 8);
        writer->write(uint8_t(dataset_type), 4);
        writer->write(uint8_t(alphabet_ID), 8);
        writer->write(read_length, 24);
        writer->write(number_of_template_segments_minus1, 2);
        writer->write(reserved_2, 6);
        writer->write(max_au_data_unit_size, 29);
        writer->write(pos_40_bits_flag, 1);
        writer->write(qv_depth, 3);
        writer->write(as_depth, 3);
        writer->write(num_classes, 4);
        for (auto &i : class_IDs) {
            writer->write(uint8_t(i), 4);
        }
        for (auto &i : descriptors) {
            i->write(writer);
        }
        writer->write(num_groups, 16);
        for (auto &i : rgroup_IDs) {
            for (auto &j : *i) {
                writer->write(j, 8);
            }
            writer->write(0, 8); // NULL termination
        }
        writer->write(multiple_alignments_flag, 1);
        writer->write(spliced_reads_flag, 1);
        writer->write(multiple_signature_base, 31);
        if (u_signature_size) {
            writer->write(*u_signature_size, 6);
        }
        for (auto &i : qv_coding_configs) {
            i->write(writer);
        }
        writer->write(crps_flag, 1);
        if (parameter_set_crps) {
            parameter_set_crps->write(*writer);
        }
        writer->flush();
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::setCrps(std::unique_ptr<ParameterSetCrps> _parameter_set_crps) {
        (void) _parameter_set_crps;
        GENIE_THROW_RUNTIME_EXCEPTION("crps not supported");
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::addClass(AuType class_id, std::unique_ptr<QvCodingConfig> conf) {
        std::stringstream s;
        BitWriter bw(&s);
        conf->write(&bw);
        addSize(bw.getBitsWritten());

        num_classes += 1;
        class_IDs.push_back(class_id);
        addSize(4);
        qv_coding_configs.push_back(std::move(conf));
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::setDescriptor(uint8_t index, std::unique_ptr<DescriptorConfigurationContainer> descriptor) {
        std::stringstream s;
        BitWriter bw(&s);
        descriptors[index]->write(&bw);
        internalBitCounter -= bw.getBitsWritten();

        descriptors[index] = std::move(descriptor);
        BitWriter bw2(&s);
        descriptors[index]->write(&bw2);
        addSize(bw2.getBitsWritten());
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::addGroup(std::unique_ptr<std::string> rgroup_id) {
        GENIE_THROW_RUNTIME_EXCEPTION("Groups not supported");
        num_groups += 1;
        rgroup_IDs.push_back(std::move(rgroup_id));
        addSize((rgroup_id->length() + 1) * 8);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size) {
        GENIE_THROW_RUNTIME_EXCEPTION("Signature base not supported");
        multiple_signature_base = _multiple_signature_base;
        u_signature_size = make_unique<uint8_t>(_U_signature_size);
        addSize(6);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::addSize(uint64_t bits) {
        internalBitCounter += bits;
        data_unit_size = internalBitCounter / 8;
        if (internalBitCounter % 8)
            data_unit_size += 1;
    }
}