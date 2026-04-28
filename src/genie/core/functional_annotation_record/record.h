/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FUNCTIONAL_ANNOTATION_RECORD_RECORD_H_
#define SRC_GENIE_CORE_FUNCTIONAL_ANNOTATION_RECORD_RECORD_H_
// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/array_type.h"
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_writer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::functional_annotation {

/**
 * @brief Structure representing an ontology entry
 */
struct Ontology {
    uint8_t ontology_name_len;
    std::string ontology_name;
    uint32_t ontology_ID;  // 24-bit value
};

/**
 * @brief Structure representing an attribute entry
 */
struct Attribute {
    uint8_t attr_tag_len;
    std::string attr_tag;
    uint8_t attr_type;
    uint8_t attr_array_len;
    std::vector<std::vector<uint8_t>> attr_values;
};

/**
 * @brief Class representing a functional annotation record
 */
class Record {
 private:
    uint64_t annotation_index_;  //!< @brief Index of the annotation
    uint16_t seq_id_;            //!< @brief Sequence identifier
    uint64_t start_pos_;         //!< @brief Start position (40-bit value)
    uint64_t end_pos_;           //!< @brief End position (40-bit value)
    uint8_t feature_name_len_;   //!< @brief Length of the feature name
    std::string feature_name_;   //!< @brief Feature name string
    uint32_t feature_ID_;        //!< @brief Feature identifier (24-bit value)
    uint8_t ontology_count_;     //!< @brief Count of ontology entries

    std::vector<Ontology> ontologies_;  //!< @brief Vector of ontology entries

    uint8_t strand_;      //!< @brief Strand information
    uint8_t attr_count_;  //!< @brief Count of attribute entries

    std::vector<Attribute> attributes_;  //!< @brief Vector of attribute entries

    uint8_t reserved_;          //!< @brief Reserved bits (7 bits)
    uint8_t linked_record_;     //!< @brief Flag indicating if record is linked (1 bit)
    uint8_t link_name_len_;     //!< @brief Length of the link name
    std::string link_name_;     //!< @brief Link name string
    uint8_t reference_box_id_;  //!< @brief Reference box identifier

    /**
     * @brief Clears all data in the record
     */
    void ClearData();

 public:
    /**
     * @brief Default constructor for Record
     */
    Record()
        : annotation_index_(0),
          seq_id_(0),
          start_pos_(0),
          end_pos_(0),
          feature_name_len_(0),
          feature_name_(""),
          feature_ID_(0),
          ontology_count_(0),
          ontologies_(0),
          strand_(0),
          attr_count_(0),
          attributes_(0),
          reserved_(0),
          linked_record_(0),
          link_name_len_(0),
          link_name_(""),
          reference_box_id_(0) {}

    /**
     * @brief Parameterized constructor for Record
     * @param annotation_index Index of the annotation
     * @param seq_id Sequence identifier
     * @param start_pos Start position
     * @param end_pos End position
     * @param feature_name_len Length of the feature name
     * @param feature_name Feature name string
     * @param feature_ID Feature identifier
     * @param ontology_count Count of ontology entries
     * @param ontologies Vector of ontology entries
     * @param strand Strand information
     * @param attr_count Count of attribute entries
     * @param attributes Vector of attribute entries
     * @param reserved Reserved bits
     * @param linked_record Flag indicating if record is linked
     * @param link_name_len Length of the link name
     * @param link_name Link name string
     * @param reference_box_ID Reference box identifier
     */
    Record(uint64_t annotation_index, uint16_t seq_id, uint64_t start_pos, uint64_t end_pos,
           uint8_t feature_name_len, std::string feature_name, uint32_t feature_ID,
           uint8_t ontology_count, std::vector<Ontology> ontologies, uint8_t strand,
           uint8_t attr_count, std::vector<Attribute> attributes, uint8_t reserved,
           uint8_t linked_record, uint8_t link_name_len, std::string link_name,
           uint8_t reference_box_ID)
        : annotation_index_(annotation_index),
          seq_id_(seq_id),
          start_pos_(start_pos),
          end_pos_(end_pos),
          feature_name_len_(feature_name_len),
          feature_name_(feature_name),
          feature_ID_(feature_ID),
          ontology_count_(ontology_count),
          ontologies_(ontologies),
          strand_(strand),
          attr_count_(attr_count),
          attributes_(attributes),
          reserved_(reserved),
          linked_record_(linked_record),
          link_name_len_(link_name_len),
          link_name_(link_name),
          reference_box_id_(reference_box_ID) {}

    /**
     * @brief Constructor from a bit reader
     * @param reader The bit reader to read from
     */
    explicit Record(util::BitReader& reader);

    /**
     * @brief Reads record data from a bit reader
     * @param reader The bit reader to read from
     * @return True if read was successful, false otherwise
     */
    bool Read(util::BitReader& reader);

    /**
     * @brief Writes record data to a writer
     * @param writer The writer to write to
     */
    void Write(core::Writer& writer);

    // Getter methods

    /**
     * @brief Gets the annotation index
     * @return The annotation index
     */
    [[nodiscard]] uint64_t GetAnnotationIndex() const {
        return annotation_index_;
    }

    /**
     * @brief Gets the sequence ID
     * @return The sequence ID
     */
    [[nodiscard]] uint16_t GetSeqId() const {
        return seq_id_;
    }

    /**
     * @brief Gets the start position
     * @return The start position
     */
    [[nodiscard]] uint64_t GetStartPos() const {
        return start_pos_;
    }

    /**
     * @brief Gets the end position
     * @return The end position
     */
    [[nodiscard]] uint64_t GetEndPos() const {
        return end_pos_;
    }

    /**
     * @brief Gets the feature name
     * @return The feature name string
     */
    [[nodiscard]] std::string GetFeatureName() const {
        return feature_name_;
    }

    /**
     * @brief Gets the feature ID
     * @return The feature ID
     */
    [[nodiscard]] uint32_t GetFeatureID() const {
        return feature_ID_;
    }

    /**
     * @brief Gets the ontology count
     * @return The ontology count
     */
    [[nodiscard]] uint8_t GetOntologyCount() const {
        return ontology_count_;
    }

    /**
     * @brief Gets the ontologies
     * @return Vector of ontology entries
     */
    [[nodiscard]] const std::vector<Ontology>& GetOntologies() const {
        return ontologies_;
    }

    /**
     * @brief Gets the strand information
     * @return The strand value
     */
    [[nodiscard]] uint8_t GetStrand() const {
        return strand_;
    }

    /**
     * @brief Gets the attribute count
     * @return The attribute count
     */
    [[nodiscard]] uint8_t GetAttrCount() const {
        return attr_count_;
    }

    /**
     * @brief Gets the attributes
     * @return Vector of attribute entries
     */
    [[nodiscard]] const std::vector<Attribute>& GetAttributes() const {
        return attributes_;
    }

    /**
     * @brief Checks if this is a linked record
     * @return True if this is a linked record, false otherwise
     */
    [[nodiscard]] bool IsLinkedRecord() const {
        return (linked_record_ == 0 ? false : true);
    }

    /**
     * @brief Gets the link name
     * @return The link name string
     */
    [[nodiscard]] std::string GetLinkName() const {
        return link_name_;
    }

    /**
     * @brief Gets the reference box ID
     * @return The reference box ID
     */
    [[nodiscard]] uint8_t GetReferenceBoxID() const {
        return reference_box_id_;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::functional_annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FUNCTIONAL_ANNOTATION_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
