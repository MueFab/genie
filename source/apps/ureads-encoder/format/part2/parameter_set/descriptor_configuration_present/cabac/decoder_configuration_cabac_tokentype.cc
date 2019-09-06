#include "decoder_configuration_cabac_tokentype.h"
#include "ureads-encoder/format/part2/make_unique.h"
#include "ureads-encoder/format/part2/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace desc_conf_pres {
        namespace cabac {
            DecoderConfigurationCabacTokentype::DecoderConfigurationCabacTokentype()
            : rle_guard_tokentype(34),
              decoder_configuration_tokentype_cabac_0(make_unique<DescriptorSubsequenceCfg>(
                      make_unique<TransformSubseqParameters>(TransformSubseqParameters::TransformIdSubseq::NO_TRANSFORM, 0), 0, true)),
              decoder_configuration_tokentype_cabac_1(make_unique<DescriptorSubsequenceCfg>(
                      make_unique<TransformSubseqParameters>(TransformSubseqParameters::TransformIdSubseq::NO_TRANSFORM, 0), 0, true)){
                decoder_configuration_tokentype_cabac_0->addTransformSubseqCfg(make_unique<TransformSubseqCfg>(SupportValues::TransformIdSubsym::NO_TRANSFORM, make_unique<SupportValues>(), make_unique<CabacBinarization>()));
            }

            // -----------------------------------------------------------------------------------------------------------------

            void DecoderConfigurationCabacTokentype::addSubsequenceCfg(std::unique_ptr<DescriptorSubsequenceCfg> cfg) {
                // Todo
            }

            // -----------------------------------------------------------------------------------------------------------------

            std::unique_ptr<DecoderConfiguration> DecoderConfigurationCabacTokentype::clone() const {
                auto ret = make_unique<DecoderConfigurationCabacTokentype>();
                ret->rle_guard_tokentype = rle_guard_tokentype;
                ret->decoder_configuration_tokentype_cabac_0 = decoder_configuration_tokentype_cabac_0->clone();
                ret->decoder_configuration_tokentype_cabac_1 = decoder_configuration_tokentype_cabac_1->clone();
            }

            // -----------------------------------------------------------------------------------------------------------------

            void DecoderConfigurationCabacTokentype::write(BitWriter *writer) const {
                DecoderConfigurationCabac::write(writer);
                writer->write(rle_guard_tokentype, 8);
                decoder_configuration_tokentype_cabac_0->write(writer);
                decoder_configuration_tokentype_cabac_0->write(writer);
            }
        }
    }
}