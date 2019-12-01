#ifndef GENIE_SOURCE_H
#define GENIE_SOURCE_H

#include "drain.h"

template<typename TYPE>
class Source {
private:
    Drain<TYPE>* drain;
protected:
    void flowOut (TYPE t, size_t id) {
        drain->flowIn(std::move(t), id);
    }
    void dryOut() {
        drain->dryIn();
    }
public:
    virtual void setDrain(Drain<TYPE>* d) {
        drain = d;
    }

    virtual bool pump(){
        return false;
    }

    virtual ~Source () = default;
};

#endif //GENIE_SOURCE_H
