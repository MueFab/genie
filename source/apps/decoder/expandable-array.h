#include <vector>

#ifndef EXPANDABLEARRAY_H
#define EXPANDABLEARRAY_H

template<typename T>
struct expandable_array : private std::vector<T> {
private:
    std::vector<T> vec;

public:
    
    using typename std::vector<T>::iterator;
    using typename std::vector<T>::const_iterator;
    using typename std::vector<T>::reverse_iterator;
    using typename std::vector<T>::const_reverse_iterator;

    expandable_array() : vec() {}
    expandable_array(size_t n) : vec(std::vector<T>(n, T())) {}
    expandable_array(size_t n, const T& v) : vec(std::vector<T>(n, v)) {}
    expandable_array(const_iterator b, const_iterator e) : vec(b, e) {}

    T&          operator[](size_t);
    const T&    operator[](size_t) const;

    
    size_t      size() const;
    void        clear();
    bool        empty() const;

    iterator        begin();
    iterator        end();
    const_iterator  begin() const;
    const_iterator  end() const;

    reverse_iterator        rbegin();
    reverse_iterator        rend();
    const_reverse_iterator  rbegin() const;
    const_reverse_iterator  rend() const;

};

template<typename T>
const T& expandable_array<T>::operator[](size_t idx) const {
    if(vec.size() <= idx)
        return T();
    return vec[idx];
}

template<typename T>
T& expandable_array<T>::operator[](size_t idx) {
    while(vec.size() <= idx)
        vec.push_back(T());
    return vec[idx];
}

template<typename T>
size_t  expandable_array<T>::size() const       { return vec.size(); }
template<typename T>
void    expandable_array<T>::clear()            { vec.clear(); }
template<typename T>
bool    expandable_array<T>::empty() const      { return vec.empty(); }

template<typename T>
typename expandable_array<T>::iterator expandable_array<T>::begin()                { return vec.begin(); }
template<typename T>
typename expandable_array<T>::iterator expandable_array<T>::end()                  { return vec.end(); }
template<typename T>
typename expandable_array<T>::const_iterator expandable_array<T>::begin() const    { return vec.begin(); }
template<typename T>
typename expandable_array<T>::const_iterator expandable_array<T>::end() const      { return vec.end(); }

template<typename T>
typename expandable_array<T>::reverse_iterator expandable_array<T>::rbegin()               { return vec.rbegin(); }
template<typename T>
typename expandable_array<T>::reverse_iterator expandable_array<T>::rend()                 { return vec.rend(); }
template<typename T>
typename expandable_array<T>::const_reverse_iterator expandable_array<T>::rbegin() const   { return vec.rbegin(); }
template<typename T>
typename expandable_array<T>::const_reverse_iterator expandable_array<T>::rend() const     { return vec.rend(); }


void initialize_supported_symbols(expandable_array<std::string> &supported_symbols) {
    supported_symbols = expandable_array<std::string>(2, "");
    supported_symbols[0] = "ACGTN";
    supported_symbols[1] = "ACGTRYSWKMBDHVN-";
}

#endif

