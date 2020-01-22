#ifndef GENIE_OPTIONAL_H
#define GENIE_OPTIONAL_H

namespace genie {
namespace util {
template<typename T>
class Optional {
   private:
    bool present;
    uint8_t buffer[sizeof(T)];
   public:
    Optional() {

    }
};
}
}

#endif  // GENIE_OPTIONAL_H
