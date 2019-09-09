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
        parameter_set_ID(_parameter_set_ID),
        parent_parameter_set_ID(_parent_parameter_set_ID),
        dataset_type(_dataset_type),
        alphabet_ID(_alphabet_id),
        read_length(_read_length),
        number_of_template_segments_minus1(_paired_end),
        max_au_data_unit_size(0),
        pos_40_bits_flag(_pos_40_bits_flag),
        qv_depth(_qv_depth),
        as_depth(_as_depth),
        class_IDs(0),
        descriptors(18),
        rgroup_IDs(0),
        multiple_alignments_flag(_multiple_alignments_flag),
        spliced_reads_flag(_spliced_reads_flag),
        multiple_signature_base(0),
        u_signature_size(nullptr),
        qv_coding_configs(0),
        crps_flag(false),
        parameter_set_crps(nullptr) {
        for (auto &i : descriptors) {
            i = make_unique<DescriptorConfigurationContainer>();
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::write(BitWriter *writer) const {
        DataUnit::write(writer);
        writer->write(0, 10); // reserved

        // Calculate size and write structure to tmp buffer
        std::stringstream ss;
        BitWriter tmp_writer(&ss);
        preWrite(&tmp_writer);
        tmp_writer.flush();
        uint64_t bits = tmp_writer.getBitsWritten();
        const uint64_t TYPE_SIZE_SIZE = 8 + 10 + 22; // data_unit_type, reserved, data_unit_size
        bits += TYPE_SIZE_SIZE;
        uint64_t bytes = bits / 8 + ((bits % 8) ? 1 : 0);

        // Now size is known, write to final destination
        writer->write(bytes, 22);
        writer->write(&ss);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::preWrite(BitWriter *writer) const {
        writer->write(parameter_set_ID, 8);
        writer->write(parent_parameter_set_ID, 8);
        writer->write(uint8_t(dataset_type), 4);
        writer->write(uint8_t(alphabet_ID), 8);
        writer->write(read_length, 24);
        writer->write(number_of_template_segments_minus1, 2);
        writer->write(0, 6); // reserved_2
        writer->write(max_au_data_unit_size, 29);
        writer->write(pos_40_bits_flag, 1);
        writer->write(qv_depth, 3);
        writer->write(as_depth, 3);
        writer->write(class_IDs.size(), 4);  // num_classes
        for (auto &i : class_IDs) {
            writer->write(uint8_t(i), 4);
        }
        for (auto &i : descriptors) {
            i->write(writer);
        }
        writer->write(rgroup_IDs.size(), 16); // num_groups
        for (auto &i : rgroup_IDs) {
            for (auto &j : *i) {
                writer->write(j, 8);
            }
            writer->write('\0', 8); // NULL termination
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
            parameter_set_crps->write(writer);
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::setCrps(std::unique_ptr<ParameterSetCrps> _parameter_set_crps) {
        crps_flag = true;
        parameter_set_crps = std::move(_parameter_set_crps);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::addClass(AuType class_id, std::unique_ptr<QvCodingConfig> conf) {
        for(auto& a : class_IDs) {
            if(class_id == a){
                GENIE_THROW_RUNTIME_EXCEPTION("Class already added");
            }
        }
        class_IDs.push_back(class_id);
        qv_coding_configs.push_back(std::move(conf));
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::setDescriptor(uint8_t index, std::unique_ptr<DescriptorConfigurationContainer> descriptor) {
        descriptors[index] = std::move(descriptor);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::addGroup(std::unique_ptr<std::string> rgroup_id) {
        rgroup_IDs.push_back(std::move(rgroup_id));
    }

    // -----------------------------------------------------------------------------------------------------------------

    void ParameterSet::setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size) {
        multiple_signature_base = _multiple_signature_base;
        u_signature_size = make_unique<uint8_t>(_U_signature_size);
    }
}