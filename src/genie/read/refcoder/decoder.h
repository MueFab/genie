#ifndef GENIE_REF_DECODER_H
#define GENIE_REF_DECODER_H

namespace genie {
namespace read {
namespace refcoder {

// ---------------------------------------------------------------------------------------------------------------------

class Decoder : public core::ReadDecoder {
   public:
    Decoder() : core::ReadDecoder() {}

    void flowIn(core::AccessUnitRaw&&, const util::Section&) override {}

    void dryIn() override { dryOut(); }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace refcoder
}  // namespace read
}  // namespace genie

#endif  // GENIE_DECODER_H
