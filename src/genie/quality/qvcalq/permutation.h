#ifndef SRC_GENIE_QUALITY_QVCALQ_PERMUTATION_H_
#define SRC_GENIE_QUALITY_QVCALQ_PERMUTATION_H_

#include <algorithm>
#include <numeric>
#include <vector>

namespace genie {
namespace quality {
namespace qvcalq {

template <typename T>
std::vector<std::size_t> sort_permutation(const std::vector<T>& vec) {
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](std::size_t i, std::size_t j) { return vec[i] < vec[j]; });
    return p;
}

template <typename T>
std::vector<T> apply_permutation(const std::vector<T>& vec, const std::vector<std::size_t>& p) {
    std::vector<T> sorted_vec(vec.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(), [&](std::size_t i) { return vec[i]; });
    return sorted_vec;
}

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

#endif  // SRC_GENIE_QUALITY_QVCALQ_PERMUTATION_H_
