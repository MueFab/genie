
#ifndef ZLIBISTREAM_H
#define ZLIBISTREAM_H
#include <istream>
#include <memory>

#include "zlibstreambuffer.h"

#endif //ZLIBISTREAM_H

namespace genie::entropy::zlib {

    class ZlibInputStream : public std::istream {

        std::unique_ptr<ZlibStreamBuffer> buffer;

        public:
            explicit ZlibInputStream(std::unique_ptr<ZlibStreamBuffer> buffer);

    };

}