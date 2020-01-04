#ifndef GENIE_SUPPORT_VALUES_H
#define GENIE_SUPPORT_VALUES_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include "util/bitwriter.h"
#include "util/bitreader.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {

/**
 * ISO 23092-2 Section 8.3.2 table
 */
    class SupportValues {
    private:
        uint8_t output_symbol_size : 6;               //!< Line 2
        uint8_t coding_subsym_size : 6;               //!< Line 3
        uint8_t coding_order : 2;                     //!< Line 4
        std::unique_ptr<bool> share_subsym_lut_flag;  //!< : 1; Line 7
        std::unique_ptr<bool> share_subsym_prv_flag;  //!< : 1; Line 8
    public:
        enum class TransformIdSubsym : uint8_t {
            NO_TRANSFORM = 0, LUT_TRANSFORM = 1, DIFF_CODING = 2
        };

        SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                      const TransformIdSubsym &_transform_ID_subsym, bool _share_subsym_prv_flag = true,
                      bool _share_subsym_lut_flag = true);

        SupportValues();

        SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader) {
            output_symbol_size = reader.read(6);
            coding_subsym_size = reader.read(6);
            coding_order = reader.read(2);
            if(coding_subsym_size < output_symbol_size && coding_order > 0) {
                if(transformIdSubsym == TransformIdSubsym::LUT_TRANSFORM) {
                    share_subsym_lut_flag = util::make_unique<bool>(reader.read(1));
                }
                share_subsym_prv_flag = util::make_unique<bool>(reader.read(1));
            }
        }

        virtual ~SupportValues() = default;

        std::unique_ptr<SupportValues> clone() const;

        virtual void write(util::BitWriter &writer) const;

        uint8_t getOutputSymbolSize () const {
            return output_symbol_size;
        }

        uint8_t getCodingSubsymSize () const {
            return coding_subsym_size;
        }

        uint8_t getCodingOrder () const {
            return coding_order;
        }

        bool getShareSubsymLutFlag () const {
            return *share_subsym_lut_flag;
        }

        bool getShareSubsymPrvFlag () const {
            return *share_subsym_prv_flag;
        }
    };
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUPPORT_VALUES_H
