#include "decoder_configuration_cabac_regular.h"
#include "ureads-encoder/format/part2/make_unique.h"
#include "ureads-encoder/format/part2/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            DecoderConfigurationCabacRegular::DecoderConfigurationCabacRegular()
                    : num_descriptor_subsequence_cfgs_minus1(-1),
                      descriptor_subsequence_cfgs(0) {

            }

            // -----------------------------------------------------------------------------------------------------------------

            std::unique_ptr<DecoderConfiguration> DecoderConfigurationCabacRegular::clone() const {
                auto ret = make_unique<DecoderConfigurationCabacRegular>();
                ret->encoding_mode_ID = encoding_mode_ID;
                for (const auto &i : descriptor_subsequence_cfgs) {
                    ret->addSubsequenceCfg(i->clone());
                }
                return ret;
            }

            // -----------------------------------------------------------------------------------------------------------------

            void DecoderConfigurationCabacRegular::addSubsequenceCfg(std::unique_ptr<DescriptorSubsequenceCfg> cfg) {
                num_descriptor_subsequence_cfgs_minus1 += 1;
                descriptor_subsequence_cfgs.push_back(std::move(cfg));
            }

            // -----------------------------------------------------------------------------------------------------------------

            void DecoderConfigurationCabacRegular::write(BitWriter *writer) const {
                DecoderConfigurationCabac::write(writer);
                writer->write(num_descriptor_subsequence_cfgs_minus1, 8);
                for (auto &i : descriptor_subsequence_cfgs) {
                    i->write(writer);
                }
            }
        }
    }
}