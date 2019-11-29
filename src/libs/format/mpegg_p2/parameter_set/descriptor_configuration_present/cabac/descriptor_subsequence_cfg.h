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

        virtual ~DescriptorSubsequenceCfg() = default;

        void setTransformSubseqCfg(size_t index, std::unique_ptr<TransformSubseqCfg> _transformSubseq_cfg);

        TransformSubseqCfg *getTransformSubseqCfg(size_t index) const;

        std::unique_ptr<DescriptorSubsequenceCfg> clone() const;

        virtual void write(util::BitWriter *writer) const;
    };
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
