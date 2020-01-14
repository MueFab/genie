#ifndef GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
#define GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "transform_subseq_cfg.h"
#include "transform_subseq_parameters.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
/**
 * ISO 23092-2 Section 8.3.1 table lines 5 to 12 + 8.3.5 table 102
 */
class DescriptorSubsequenceCfg {
   private:
    std::unique_ptr<uint16_t> descriptor_subsequence_ID;  //!< : 10; line 5, absent in 8.3.5, present in 8.3.1
    std::unique_ptr<TransformSubseqParameters> transform_subseq_parameters;  //!< lines 6 + 7
    std::vector<std::unique_ptr<TransformSubseqCfg>> transformSubseq_cfgs;   //!< lines 8 to 12
    DescriptorSubsequenceCfg() = default;

   public:
    DescriptorSubsequenceCfg(std::unique_ptr<TransformSubseqParameters> _transform_subseq_parameters,
                             uint16_t descriptor_subsequence_ID, bool tokentype);

    DescriptorSubsequenceCfg(bool tokentype, util::BitReader& reader) {
        if (!tokentype) {
            descriptor_subsequence_ID = util::make_unique<uint16_t>(reader.read(10));
        }
        transform_subseq_parameters = util::make_unique<TransformSubseqParameters>(reader);
        uint8_t numSubseq = 0;
        switch (transform_subseq_parameters->getTransformIdSubseq()) {
            case TransformSubseqParameters::TransformIdSubseq::NO_TRANSFORM:
                numSubseq = 1;
                break;
            case TransformSubseqParameters::TransformIdSubseq::EQUALITY_CODING:
                numSubseq = 2;
                break;
            case TransformSubseqParameters::TransformIdSubseq::RLE_CODING:
                numSubseq = 2;
                break;
            case TransformSubseqParameters::TransformIdSubseq::MATCH_CODING:
                numSubseq = 3;
                break;
            case TransformSubseqParameters::TransformIdSubseq::MERGE_CODING:
                UTILS_DIE("MERGE coding unsupported");
                break;
        }
        for (size_t i = 0; i < numSubseq; ++i) {
            transformSubseq_cfgs.emplace_back(util::make_unique<TransformSubseqCfg>(reader));
        }
    }

    virtual ~DescriptorSubsequenceCfg() = default;

    void setTransformSubseqCfg(size_t index, std::unique_ptr<TransformSubseqCfg> _transformSubseq_cfg);

    std::unique_ptr<DescriptorSubsequenceCfg> clone() const;

    virtual void write(util::BitWriter& writer) const;

    uint16_t getDescriptorSubsequenceID() const {
        if (!descriptor_subsequence_ID) {
            UTILS_DIE("descriptor_subsequence_ID not present");
        }
        return *descriptor_subsequence_ID.get();
    };

    const TransformSubseqParameters* getTransformParameters() const { return transform_subseq_parameters.get(); }

    const TransformSubseqCfg* getTransformSubseqCfg(uint8_t index) const { return transformSubseq_cfgs[index].get(); }

    size_t getNumTransformSubseqCfgs() const { return transformSubseq_cfgs.size(); }

    const std::vector<std::unique_ptr<TransformSubseqCfg>>& getTransformSubseqCfgs() const {
        return transformSubseq_cfgs;
    }
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
