/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/constants.h"
#include "genie/core/record/linked_record/linked_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 *  @brief
 */
class info_field {
 private:
    uint8_t info_tag_len;             //!< @brief
    std::string info_tag;             //!< @brief
    uint8_t info_type;                //!< @brief
    uint8_t info_array_len;           //!< @brief
    std::vector<uint8_t> info_value;  //!< @brief type??
};

class FormatField {
 public:
    //    uint8_t len;
    //    std::string format;
    //    uint8_t type;
    //    uint8_t array_len;
    //    std::vector<std::vector<std::vector<uint8_t>>> value_array;
    //
    //    FormatField() : len(0), format(""), type(0), array_len(0), value_array(0) {}
    //
    //    FormatField(uint8_t len, std::string format, uint8_t type, uint8_t array_len,
    //                std::vector<std::vector<std::vector<uint8_t>>> value)
    //        : len(len), format(format), type(type), array_len(array_len), value_array(value) {}
    //
    //    void write(util::BitReader& reader, uint32_t sample_count);
};

/**
 *  @brief
 */
class VariantGenotype {
 private:
    uint64_t variant_index;      //!< @brief
    uint32_t sample_index_from;  //!< @brief
    uint32_t sample_count;       //!< @brief

    std::vector<FormatField> format;  //!< @brief

    std::vector<std::vector<uint8_t>> alleles;  //!< @brief
    std::vector<std::vector<uint8_t>> phasings;  //!< @brief

    std::vector<std::vector<uint32_t>> likelihoods;  //!< @brief

    bool linked_record;        //!< @brief
    boost::optional<LinkRecord> link_record;

 public:
    /**
     * @brief
     */
    //    VariantGenotype()
    //        : variant_index(0),
    //          sample_index_from(0),
    //          sample_count(0),
    ////          format_count(0),
    //          format(0),
    ////          genotype_present(0),
    ////          likelihood_present(0),
    ////          n_alleles_per_sample(1),
    //          alleles(0),
    //          phasings(0),
    //          n_likelihoods(0),
    //          likelihoods(0),
    //          linked_record(0),
    //          link_name_len(0),
    //          link_name(),
    //          reference_box_ID(0) {}

    //    VariantGenotype(uint64_t variant_index, uint32_t sample_index_from, uint32_t sample_count, uint8_t format_count,
    //           std::vector<FormatField> format, uint8_t genotype_present, uint8_t likelihood_present,
    //           uint8_t n_alleles_per_sample, std::vector<std::vector<uint8_t>> alleles,
    //           std::vector<std::vector<uint8_t>> phasings, uint8_t n_likelihoods,
    //           std::vector<std::vector<uint32_t>> likelihoods, uint8_t linked_record, uint8_t link_name_len = 0,
    //           std::string link_name = "", uint8_t reference_box_ID = 0);

    explicit VariantGenotype(util::BitReader& bitreader);

    //    void write(std::ostream& outputfile) const;
    //
    uint64_t getVariantIndex() const;
    uint32_t getStartSampleIndex() const;
    uint32_t getSampleCount() const;

    uint8_t getFormatCount() const;
    //    const std::vector<FormatField>& getFormat() const;
    bool isGenotypePresent() const;
    bool isLikelihoodPresent() const;
    uint8_t getNumberOfAllelesPerSample() const;
    const std::vector<std::vector<uint8_t>>& getAlleles() const;
    const std::vector<std::vector<uint8_t>>& getPhasing() const;
    uint8_t getNumberOfLikelihoods() const;
    const std::vector<std::vector<uint32_t>>& getLikelihoods() const;
    bool isLinkRecordExist() const;
    const boost::optional<LinkRecord>& getLinkRecord() const;
//    uint8_t getLinkNameLength() const;
//    const std::string& getLinkName() const;
//    uint8_t getReferenceBoxID() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
