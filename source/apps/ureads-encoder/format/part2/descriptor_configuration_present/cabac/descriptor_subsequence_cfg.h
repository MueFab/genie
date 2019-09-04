#ifndef GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
#define GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "ureads-encoder/format/part2/descriptor_configuration_present/cabac/transform_subseq_parameters.h"
#include "ureads-encoder/format/part2/descriptor_configuration_present/cabac/transform_subseq_cfg.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    class BitWriter;
    namespace desc_conf_pres {
        namespace cabac {
            /**
            * ISO 23092-2 Section 8.3.1 table lines 5 to 12
            */
            class DescriptorSubsequenceCfg {
            private:
                uint16_t descriptor_subsequence_ID : 10; //!< line 5
                std::unique_ptr<TransformSubseqParameters> transform_subseq_parameters; //!< lines 6 + 7
                std::vector<std::unique_ptr<TransformSubseqCfg>> transformSubseq_cfgs; //!< lines 8 to 12

            public:
                DescriptorSubsequenceCfg();

                DescriptorSubsequenceCfg(std::unique_ptr<TransformSubseqParameters> _transform_subseq_parameters,
                                         uint16_t descriptor_subsequence_ID);

                void addTransformSubseqCfg(std::unique_ptr<TransformSubseqCfg> _transformSubseq_cfg);

                std::unique_ptr<DescriptorSubsequenceCfg> clone() const;

                virtual void write(BitWriter *writer) const;
            };
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
