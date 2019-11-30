#ifndef GENIE_DRAIN_H
#define GENIE_DRAIN_H


#include <cstddef>

template<typename TYPE>
class Drain {
public:
    virtual void flowIn(TYPE t, size_t id) = 0;
    virtual void dryIn() = 0;
    virtual ~Drain () = default;
};

#endif //GENIE_DRAIN_H
