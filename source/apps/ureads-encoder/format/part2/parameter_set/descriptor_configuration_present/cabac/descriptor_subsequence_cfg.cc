#include "descriptor_subsequence_cfg.h"
#include "ureads-encoder/format/part2/make_unique.h"
#include "ureads-encoder/format/part2/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            DescriptorSubsequenceCfg::DescriptorSubsequenceCfg() {

            }

            // -----------------------------------------------------------------------------------------------------------------

            std::unique_ptr<DescriptorSubsequenceCfg> DescriptorSubsequenceCfg::clone() const {
                auto ret = make_unique<DescriptorSubsequenceCfg>();
                if(descriptor_subsequence_ID) {
                    ret->descriptor_subsequence_ID = make_unique<uint16_t >(*descriptor_subsequence_ID);
                } else {
                    ret->descriptor_subsequence_ID = nullptr;
                }
                ret->transform_subseq_parameters = transform_subseq_parameters->clone();
                for (const auto &c : transformSubseq_cfgs) {
                    ret->transformSubseq_cfgs.push_back(c->clone());
                }
                return ret;
            }

            // -----------------------------------------------------------------------------------------------------------------

            DescriptorSubsequenceCfg::DescriptorSubsequenceCfg(
                    std::unique_ptr<TransformSubseqParameters> _transform_subseq_parameters,
                    uint16_t _descriptor_subsequence_ID,
                    bool tokentype
            ) : descriptor_subsequence_ID(nullptr),
                transform_subseq_parameters(std::move(_transform_subseq_parameters)),
                transformSubseq_cfgs(0) {
                if(!tokentype){
                    descriptor_subsequence_ID = make_unique<uint16_t >(_descriptor_subsequence_ID);
                }
            }

            // -----------------------------------------------------------------------------------------------------------------

            void
            DescriptorSubsequenceCfg::addTransformSubseqCfg(std::unique_ptr<TransformSubseqCfg> _transformSubseq_cfg) {
                transformSubseq_cfgs.push_back(std::move(_transformSubseq_cfg));
            }

            // -----------------------------------------------------------------------------------------------------------------

            void DescriptorSubsequenceCfg::write(BitWriter *writer) const {
                if(descriptor_subsequence_ID) {
                    writer->write(*descriptor_subsequence_ID, 10);
                }
                transform_subseq_parameters->write(writer);
                for (auto &i : transformSubseq_cfgs) {
                    i->write(writer);
                }
            }
        }
    }
}