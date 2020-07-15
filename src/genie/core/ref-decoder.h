
#ifndef GENIE_REFDECODER_H
#define GENIE_REFDECODER_H

namespace genie {
namespace core {

class RefDecoder {
   public:
    virtual ~RefDecoder() = default;

    virtual std::string decode(core::AccessUnit&& t) = 0;
};
}  // namespace core
}  // namespace genie

#endif  // GENIE_REFDECODER_H
