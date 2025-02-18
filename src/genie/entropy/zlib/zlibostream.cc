#include "genie/entropy/zlib/zlibistream.h"

namespace genie::entropy::zlib {

     ZlibInputStream::ZlibInputStream(std::unique_ptr<ZlibStreamBuffer> buf): buffer(std::move(buf)) {
         std::istream::init(buffer.get());
     }



}  // namespace genie::entropy::zlib