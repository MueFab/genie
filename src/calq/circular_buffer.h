#ifndef CALQ_CIRCULAR_BUFFER_H_
#define CALQ_CIRCULAR_BUFFER_H_

// -----------------------------------------------------------------------------

#include <cstddef>

// -----------------------------------------------------------------------------

#include <vector>

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

// Fixed-size-buffer in circular shape
template<typename T>
class CircularBuffer
{
 private:
    std::vector<T> data;
    size_t pos;

    // -------------------------------------------------------------------------

 public:
    CircularBuffer(size_t size,
                   const T& val
    )
            : pos(0){
        data.resize(size, val);
    }

    // -------------------------------------------------------------------------

    T& operator[](size_t index){
        return data[(pos + index) % data.size()];
    }

    // -------------------------------------------------------------------------

    const T& operator[](size_t index) const{
        return data[(pos + index) % data.size()];
    }

    // -------------------------------------------------------------------------

    // Leftmost value
    T& back(){
        return (*this)[pos];
    }

    // -------------------------------------------------------------------------

    const T& back() const{
        return (*this)[pos];
    }

    // -------------------------------------------------------------------------

    // Rightmost value
    T& front(){
        return (*this)[(pos + data.size() - 1) % data.size()];
    }

    // -------------------------------------------------------------------------

    const T& front() const{
        return (*this)[(pos + data.size() - 1) % data.size()];
    }

    // -------------------------------------------------------------------------

    size_t size() const{
        return data.size();
    }

    // -------------------------------------------------------------------------

    // Returns oldest value, deletes it and puts new value
    T push(const T& val){
        T oldVal = data[pos];
        data[pos] = val;
        pos = (pos + 1) % data.size();
        return oldVal;
    }
};

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_CIRCULAR_BUFFER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
