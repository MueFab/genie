/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_
#define SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/record.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/block.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mgb/signature_cfg.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

class AUHeader {
 private:
    uint32_t access_unit_ID;                         //!< @brief
    uint8_t num_blocks;                              //!< @brief
    uint8_t parameter_set_ID;                        //!< @brief
    core::record::ClassType au_type;                 //!< @brief
    uint32_t reads_count;                            //!< @brief
    boost::optional<MmCfg> mm_cfg;                   //!< @brief
    boost::optional<RefCfg> ref_cfg;                 //!< @brief
    boost::optional<AuTypeCfg> au_Type_U_Cfg;        //!< @brief
    boost::optional<SignatureCfg> signature_config;  //!< @brief

 public:
    bool operator==(const AUHeader &other) const {
        return access_unit_ID == other.access_unit_ID && num_blocks == other.num_blocks &&
               parameter_set_ID == other.parameter_set_ID && au_type == other.au_type &&
               reads_count == other.reads_count && mm_cfg == other.mm_cfg && ref_cfg == other.ref_cfg &&
               au_Type_U_Cfg == other.au_Type_U_Cfg && signature_config == other.signature_config;
    }

    void write(genie::util::BitWriter &writer, bool write_signatures) const {
        writer.write(access_unit_ID, 32);
        writer.write(num_blocks, 8);
        writer.write(parameter_set_ID, 8);
        writer.write(uint8_t(au_type), 4);
        writer.write(reads_count, 32);
        if (mm_cfg) {
            mm_cfg->write(writer);
        }
        if (ref_cfg) {
            ref_cfg->write(writer);
        }
        if (write_signatures) {
            if (au_Type_U_Cfg) {
                au_Type_U_Cfg->write(writer);
            }
            if (signature_config) {
                signature_config->write(writer);
            }
        }
        writer.flush();
    }

    uint32_t getNumBlocks() const { return num_blocks; }

    void blockAdded() { num_blocks++; }

    AUHeader(util::BitReader &bitReader, const std::map<size_t, core::parameter::EncodingSet> &parameterSets,
             bool read_signatures = true) {
        UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");
        access_unit_ID = bitReader.read<uint32_t>();
        num_blocks = bitReader.read<uint8_t>();
        parameter_set_ID = bitReader.read<uint8_t>();
        au_type = bitReader.read<core::record::ClassType>(4);
        reads_count = bitReader.read<uint32_t>();
        if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
            this->mm_cfg = MmCfg(bitReader);
        }

        if (parameterSets.at(parameter_set_ID).getDatasetType() ==
            core::parameter::ParameterSet::DatasetType::REFERENCE) {
            this->ref_cfg = RefCfg(parameterSets.at(parameter_set_ID).getPosSize(), bitReader);
        }
        if (read_signatures) {
            if (au_type != core::record::ClassType::CLASS_U) {
                this->au_Type_U_Cfg = AuTypeCfg(parameterSets.at(parameter_set_ID).getPosSize(),
                                                parameterSets.at(parameter_set_ID).hasMultipleAlignments(), bitReader);
            } else {
                if (parameterSets.at(parameter_set_ID).isSignatureActivated()) {
                    uint8_t length = 0;
                    if (parameterSets.at(parameter_set_ID).isSignatureConstLength()) {
                        length = parameterSets.at(parameter_set_ID).getSignatureConstLength();
                    }
                    this->signature_config = SignatureCfg(bitReader, length);
                }
            }
        }
        bitReader.flush();
        UTILS_DIE_IF(!bitReader.isAligned(), "Bitreader not aligned");
    }

    AUHeader()
        : AUHeader(0, 0, core::record::ClassType::NONE, 0, core::parameter::ParameterSet::DatasetType::NON_ALIGNED, 0,
                   false) {}

    AUHeader(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
             uint32_t _reads_count, genie::core::parameter::ParameterSet::DatasetType dataset_type, uint8_t posSize,
             bool signatureFlag)
        : access_unit_ID(_access_unit_ID),
          num_blocks(0),
          parameter_set_ID(_parameter_set_ID),
          au_type(_au_type),
          reads_count(_reads_count),
          mm_cfg(),
          ref_cfg(),
          au_Type_U_Cfg(),
          signature_config() {
        if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
            mm_cfg = MmCfg();
        }
        if (dataset_type == genie::core::parameter::ParameterSet::DatasetType::REFERENCE) {
            ref_cfg = RefCfg(posSize);
        }
        if (au_type != core::record::ClassType::CLASS_U) {
            au_Type_U_Cfg = AuTypeCfg(posSize);
        } else {
            if (signatureFlag) {
                signature_config = SignatureCfg();
            }
        }
    }

    /**
     * @brief
     * @return
     */
    uint32_t getID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getParameterID() const;

    /**
     * @brief
     * @return
     */
    const AuTypeCfg &getAlignmentInfo() const;

    /**
     * @brief
     * @return
     */
    uint32_t getReadCount() const;

    /**
     * @brief
     * @return
     */
    core::record::ClassType getClass() const;

    /**
     * @brief
     * @param cfg
     */
    void setMmCfg(MmCfg &&cfg);

    /**
     * @brief
     * @param cfg
     */
    void setRefCfg(RefCfg &&cfg);

    /**
     * @brief
     * @return
     */
    const RefCfg &getRefCfg();

    /**
     * @brief
     * @param cfg
     */
    void setAuTypeCfg(AuTypeCfg &&cfg);

    /**
     * @brief
     * @param cfg
     */
    void setSignatureCfg(SignatureCfg &&cfg);
};

/**
 * @brief
 */
class AccessUnit : public core::parameter::DataUnit {
 public:
    /**
     * @brief
     */
    void debugPrint(const core::parameter::EncodingSet &ps) const;

    /**
     * @brief
     * @param parameterSets
     * @param bitReader
     * @param lazyPayload
     */
    explicit AccessUnit(const std::map<size_t, core::parameter::EncodingSet> &parameterSets, util::BitReader &bitReader,
                        bool lazyPayload = false);

    /**
     * @brief
     * @param bitReader
     */
    void loadPayload(util::BitReader &bitReader);

    /**
     * @brief
     * @return
     */
    size_t getPayloadSize() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @param _access_unit_ID
     * @param _parameter_set_ID
     * @param _au_type
     * @param _reads_count
     * @param dataset_type
     * @param posSize
     * @param signatureSize
     * @param multiple_signature_base
     */
    AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
               uint32_t _reads_count, DatasetType dataset_type, uint8_t posSize, bool signatureFlag);

    /**
     * @brief
     * @param block
     */
    void addBlock(Block block);

    /**
     * @brief
     * @return
     */
    std::vector<Block> &getBlocks();

    AUHeader &getHeader() { return header; }

 private:
    AUHeader header;

    std::vector<Block> blocks;  //!< @brief

    size_t payloadbytes;  //!< @brief
    size_t qv_payloads;   //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
