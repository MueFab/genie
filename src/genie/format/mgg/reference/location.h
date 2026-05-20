/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/meta/external_ref/fasta.h"
#include "genie/core/meta/external_ref/mpeg.h"
#include "genie/core/meta/external_ref/raw.h"
#include "genie/core/meta/internal_ref.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {

/**
 * @brief
 */
class Location {
 private:
    uint8_t reserved;        //!< @brief
    bool external_ref_flag;  //!< @brief

 public:
    /**
     * @brief
     */
    virtual ~Location() = default;

    /**
     * @brief
     * @param _reserved
     * @param _external_ref_flag
     */
    explicit Location(uint8_t _reserved, bool _external_ref_flag);

    /**
     * @brief
     * @param reader
     */
    explicit Location(genie::util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    bool isExternal() const;

    /**
     * @brief
     * @param reader
     * @param seq_count
     * @param _version
     * @return
     */
    static std::unique_ptr<Location> factory(genie::util::BitReader& reader, size_t seq_count,
                                             genie::core::MpegMinorVersion _version);

    /**
     * @brief
     * @param base
     * @param _version
     * @return
     */
    static std::unique_ptr<Location> referenceLocationFactory(std::unique_ptr<genie::core::meta::RefBase> base,
                                                              genie::core::MpegMinorVersion _version);

    /**
     * @brief
     * @param writer
     */
    virtual void write(genie::util::BitWriter& writer);

    /**
     * @brief
     * @return
     */
    virtual std::unique_ptr<genie::core::meta::RefBase> decapsulate() = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
