/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_PARAMETER_SET_H
#define GENIE_QV_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <genie/core/record/record.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "computed_ref.h"
#include "data_unit.h"
#include "descriptor_box.h"
#include "quality-values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 * ISO 23092-2 Section 3.3.1 table 6 + 3.3.2 table 7 (fused for simplification)
 */
class ParameterSet : public DataUnit {
   public:
    explicit ParameterSet(util::BitReader &bitReader);

    ParameterSet(uint8_t _parameter_set_ID, uint8_t _parent_parameter_set_ID, DatasetType _dataset_type,
                 AlphabetID _alphabet_id, uint32_t _read_length, bool _paired_end, bool _pos_40_bits_flag,
                 uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag, bool _spliced_reads_flag);

    ParameterSet();

    const ComputedRef &getComputedRef() const;

    size_t getNumberTemplateSegments() const;

    void setComputedRef(std::unique_ptr<ComputedRef> parameter_set_crps);

    void addClass(record::ClassType class_id, std::unique_ptr<QualityValues> conf);

    void setDescriptor(GenDesc index, std::unique_ptr<DescriptorBox> descriptor);

    const DescriptorBox *getDescriptor(GenDesc index) const;

    void addGroup(std::unique_ptr<std::string> rgroup_id);

    void setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    void write(util::BitWriter &writer) const override;

    DatasetType getDatasetType() const;

    uint8_t getPosSize() const;

    bool hasMultipleAlignments() const;

    uint32_t getMultipleSignatureBase() const;

    uint8_t getSignatureSize() const;

    uint8_t getID() const;

    ParameterSet &operator=(const ParameterSet &other);

    ParameterSet &operator=(ParameterSet &&other) noexcept;

    ParameterSet(const ParameterSet &other);

    ParameterSet(ParameterSet &&other) noexcept;

   private:
    void preWrite(util::BitWriter &writer) const;

    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     *
     * ------------------------------------------------------------------------------------------------------------ */

    //!< uint16_t reserved : 10; //!< Line 8, currently set to zero
    //!< uint32_t data_unit_size : 22; //!< Line 9, currently computed on the fly

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
     * ISO 23092-2 Section 3.3.2 table 6
     *
     * ------------------------------------------------------------------------------------------------------------- */

    uint8_t parameter_set_ID : 8;         //!< Line 2
    uint8_t parent_parameter_set_ID : 8;  //!< Line 3

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
     * ISO 23092-2 Section 3.3.2 table 7
     *
     * ------------------------------------------------------------------------------------------------------------- */

    DatasetType dataset_type;                        //!< : 4; Line 2
    AlphabetID alphabet_ID : 8;                      //!< Line 3
    uint32_t read_length : 24;                       //!< Line 4
    uint8_t number_of_template_segments_minus1 : 2;  //!< Line 5
    //!< uint8_t reserved_2 : 6; //!< Line 6, currently set to 0
    uint32_t max_au_data_unit_size : 29;  //!< Line 7
    bool pos_40_bits_flag : 1;            //!< Line 8
    uint8_t qv_depth : 3;                 //!< Line 9
    uint8_t as_depth : 3;                 //!< Line 10
    //!< uint8_t num_classes : 4; //!< Line 11, currently infered from vector
    std::vector<record::ClassType> class_IDs;                 //!< : 4; For loop Lines 12 + 13
    std::vector<std::unique_ptr<DescriptorBox>> descriptors;  //!< For loop lines 14 - 22
    //!< uint16_t num_groups : 16; //!< Line 23 currently infered from vector
    std::vector<std::string> rgroup_IDs;                            //!< For Loop lines 24 + 25
    bool multiple_alignments_flag : 1;                              //!< Line 26
    bool spliced_reads_flag : 1;                                    //!< Line 27
    uint32_t multiple_signature_base : 31;                          //!< Line 28
    std::unique_ptr<uint8_t> u_signature_size;                      //!< : 6; Line 29 + 30
    std::vector<std::unique_ptr<QualityValues>> qv_coding_configs;  //!< For loop lines 31 to 41
    bool crps_flag : 1;                                             //!< Line 42
    std::unique_ptr<ComputedRef> parameter_set_crps;                //!< Lines 43 + 44

    /** Padding in write() //!< Line 45 + 46 */

    /** ----------------------------------------------------------------------------------------------------------- */
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------