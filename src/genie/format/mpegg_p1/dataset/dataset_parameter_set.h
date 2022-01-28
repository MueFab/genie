/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mpegg_p1/dataset/dataset_header.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetParameterSet : GenInfo {
 public:
    class USignature {
     private:
        boost::optional<uint8_t> u_signature_length;

     public:
        USignature() : u_signature_length(boost::none) {}

        explicit USignature(genie::util::BitReader& reader) {
            if (reader.read<bool>(1)) {
                u_signature_length = reader.read<uint8_t>();
            }
        }

        bool isConstantLength() const { return u_signature_length != boost::none; }

        uint8_t getConstLength() const { return *u_signature_length; }

        void setConstLength(uint8_t length) { u_signature_length = length; }

        void write(genie::util::BitWriter& writer) const {
            writer.write(u_signature_length != boost::none, 1);
            if (u_signature_length != boost::none) {
                writer.write(*u_signature_length, 8);
            }
        }
    };

    class ParameterUpdateInfo {
     private:
        bool multiple_alignment_flag;
        bool pos_40_bits_flag;
        core::AlphabetID alphabetId;
        boost::optional<USignature> u_signature;

     public:
        ParameterUpdateInfo(bool _multiple_alignment_flag, bool _pos_40_bits_flag, core::AlphabetID _alphabetId)
            : multiple_alignment_flag(_multiple_alignment_flag),
              pos_40_bits_flag(_pos_40_bits_flag),
              alphabetId(_alphabetId) {}

        explicit ParameterUpdateInfo(genie::util::BitReader& reader) {
            multiple_alignment_flag = reader.read<bool>(1);
            pos_40_bits_flag = reader.read<bool>(1);
            alphabetId = reader.read<core::AlphabetID>(8);
            if (reader.read<bool>(1)) {
                u_signature = USignature(reader);
            }
            reader.flush();
        }

        void write(genie::util::BitWriter& writer) const {
            writer.write(multiple_alignment_flag, 1);
            writer.write(pos_40_bits_flag, 1);
            writer.write(static_cast<uint8_t>(alphabetId), 8);
            writer.write(u_signature != boost::none, 1);
            if (u_signature != boost::none) {
                u_signature->write(writer);
            }
            writer.flush();
        }

        void addUSignature(USignature signature) { u_signature = signature; }

        bool getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

        bool getPos40BitsFlag() const { return pos_40_bits_flag; }

        core::AlphabetID getAlphabetID() const { return alphabetId; }

        bool hasUSignature() const { return u_signature != boost::none; }

        const USignature& getUSignature() const { return *u_signature; }
    };

 private:
    uint8_t dataset_group_id;
    uint16_t dataset_id;
    uint8_t parameter_set_ID;
    uint8_t parent_parameter_set_ID;
    boost::optional<ParameterUpdateInfo> param_update;
    genie::core::parameter::ParameterSet params;
    core::MPEGMinorVersion version;

 public:
    DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id, uint8_t _parameter_set_ID,
                        uint8_t _parent_parameter_set_ID, genie::core::parameter::ParameterSet ps,
                        core::MPEGMinorVersion _version)
        : dataset_group_id(_dataset_group_id),
          dataset_id(_dataset_id),
          parameter_set_ID(_parameter_set_ID),
          parent_parameter_set_ID(_parent_parameter_set_ID),
          params(std::move(ps)),
          version(_version) {}

    DatasetParameterSet(genie::util::BitReader& reader, core::MPEGMinorVersion _version, bool parameters_update_flag)
        : version(_version) {
        dataset_group_id = reader.readBypassBE<uint8_t>();
        dataset_id = reader.readBypassBE<uint16_t>();
        parameter_set_ID = reader.readBypassBE<uint8_t>();
        parent_parameter_set_ID = reader.readBypassBE<uint8_t>();
        if (version != genie::core::MPEGMinorVersion::V1900 && parameters_update_flag) {
            param_update = ParameterUpdateInfo(reader);
        }
        params = genie::core::parameter::ParameterSet(reader);
    }

    void write(genie::util::BitWriter& writer) const override {
        writer.writeBypassBE(dataset_group_id);
        writer.writeBypassBE(dataset_id);
        writer.writeBypassBE(parameter_set_ID);
        writer.writeBypassBE(parent_parameter_set_ID);
        if (param_update != boost::none) {
            param_update->write(writer);
        }
        params.write(writer);
    }

    void addParameterUpdate(ParameterUpdateInfo update) {
        if (version != core::MPEGMinorVersion::V1900) {
            param_update = std::move(update);
        }
    }

    uint8_t getDatasetGroupID() const { return dataset_group_id; }

    uint16_t getDatasetID() const { return dataset_id; }

    uint8_t getParameterSetID() const { return parameter_set_ID; }

    uint8_t getParentParameterSetID() const { return parent_parameter_set_ID; }

    bool hasParameterUpdate() const { return param_update != boost::none; }

    const ParameterUpdateInfo& getParameterUpdate() const { return *param_update; }

    const genie::core::parameter::ParameterSet& getParameterSet() const { return params; }

    genie::core::parameter::ParameterSet&& moveParameterSet() { return std::move(params); }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------