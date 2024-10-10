/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/meta/external-ref/fasta.h"
#include "genie/core/meta/external-ref/mpeg.h"
#include "genie/core/meta/external-ref/raw.h"
#include "genie/core/meta/internal-ref.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

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
    [[nodiscard]] bool isExternal() const;

    /**
     * @brief
     * @param reader
     * @param seq_count
     * @param _version
     * @return
     */
    static std::unique_ptr<Location> factory(genie::util::BitReader& reader, size_t seq_count,
                                             genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param base
     * @param _version
     * @return
     */
    static std::unique_ptr<Location> referenceLocationFactory(std::unique_ptr<genie::core::meta::RefBase> base,
                                                              genie::core::MPEGMinorVersion _version);

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

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
