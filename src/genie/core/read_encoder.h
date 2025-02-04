/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_READ_ENCODER_H_
#define SRC_GENIE_CORE_READ_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <tuple>
#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/core/module.h"
#include "genie/core/name_encoder.h"
#include "genie/core/qv_encoder.h"
#include "genie/util/drain.h"
#include "genie/util/selector.h"
#include "genie/util/side_selector.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief The basic interface for modules encoding the plain read data
 */
class ReadEncoder : public Module<record::Chunk, AccessUnit> {
 public:
    using QvSelector = util::SideSelector<QVEncoder, QVEncoder::QVCoded, const record::Chunk&>;  //!< @brief
    using NameSelector = util::SideSelector<NameEncoder, std::tuple<AccessUnit::Descriptor, core::stats::PerfStats>,
                                            const record::Chunk&>;  //!< @brief
    using EntropySelector =
        util::SideSelector<EntropyEncoder, EntropyEncoder::EntropyCoded, AccessUnit::Descriptor&>;  //!< @brief

 protected:
    QvSelector* qvcoder{};            //!< @brief
    NameSelector* namecoder{};        //!< @brief
    EntropySelector* entropycoder{};  //!< @brief
    bool writeOutStreams;

 public:
    /**
     * @brief
     * @param coder
     */
    virtual void setQVCoder(QvSelector* coder);

    /**
     * @brief
     * @param coder
     */
    virtual void setNameCoder(NameSelector* coder);

    /**
     * @brief
     * @param coder
     */
    virtual void setEntropyCoder(EntropySelector* coder);

    /**
     * @brief
     * @param _entropycoder
     * @param a
     * @param write_raw
     * @return
     */
    static AccessUnit entropyCodeAU(EntropySelector* _entropycoder, AccessUnit&& a, bool write_raw);

    /**
     * @brief
     * @param a
     * @return
     */
    AccessUnit entropyCodeAU(AccessUnit&& a);

    /**
     * @brief For polymorphic destruction
     */
    ~ReadEncoder() override = default;

    /**
     * @brief
     * @param _writeOutStreams
     */
    explicit ReadEncoder(bool _writeOutStreams) : writeOutStreams(_writeOutStreams) {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_READ_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
