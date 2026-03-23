/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_TRACK_PROPERTY_RECORD_RECORD_H_
#define SRC_GENIE_CORE_TRACK_PROPERTY_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/bit_writer.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::track_property {

/**
 * @brief Structure representing a track property (stored as attribute)
 */
struct TrackProperty {
    uint8_t track_property_len;
    std::string track_property;
    uint8_t track_property_type;
    uint8_t track_property_array_len;
    std::vector<uint8_t> track_property_values;  // Raw byte values for the attribute
};

/**
 * @brief Class representing a track property record
 */
class Record {
 private:
    uint16_t track_index_;           //!< @brief Track index
    uint8_t track_type_;             //!< @brief Track type (GTF, GFF, GENBANK, BED, BEDGRAPH, WIG, BIGWIG)
    uint8_t track_property_count_;   //!< @brief Count of track properties

    std::vector<TrackProperty> properties_;  //!< @brief Vector of track properties (stored as attributes)

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
        : track_index_(0),
          track_type_(0),
          track_property_count_(0),
          properties_(0),
          reserved_(0),
          linked_record_(0),
          link_name_len_(0),
          link_name_(""),
          reference_box_id_(0) {}

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
    void Write(util::BitWriter& writer);

    // Getter methods

    /**
     * @brief Gets the track index
     * @return The track index
     */
    [[nodiscard]] uint16_t GetTrackIndex() const {
        return track_index_;
    }

    /**
     * @brief Gets the track type
     * @return The track type
     */
    [[nodiscard]] uint8_t GetTrackType() const {
        return track_type_;
    }

    /**
     * @brief Gets the track property count
     * @return The property count
     */
    [[nodiscard]] uint8_t GetTrackPropertyCount() const {
        return track_property_count_;
    }

    /**
     * @brief Gets the properties
     * @return Vector of track properties (attributes)
     */
    [[nodiscard]] const std::vector<TrackProperty>& GetProperties() const {
        return properties_;
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

}  // namespace genie::core::record::track_property

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_TRACK_PROPERTY_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
