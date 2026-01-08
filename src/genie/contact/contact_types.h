#ifndef GENIE_CONTACT_TYPES_H
#define GENIE_CONTACT_TYPES_H

#include <cstdint>
#include <vector>

#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
#include <xtensor/xtensor.hpp>
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
#include <Eigen/Dense>
#endif

namespace genie::contact {

#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using UInt8VecDtype = xt::xtensor<uint8_t, 1, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
using UIntMatDtype = xt::xtensor<uint32_t, 2, xt::layout_type::row_major>;
using UInt64VecDtype = xt::xtensor<uint64_t, 1, xt::layout_type::row_major>;
using Int64VecDtype = xt::xtensor<int64_t, 1, xt::layout_type::row_major>;
using DoubleVecDtype = xt::xtensor<double, 1, xt::layout_type::row_major>;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
using BinVecDtype = Eigen::Matrix<bool, Eigen::Dynamic, 1>;
using BinMatDtype = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using UInt8VecDtype = Eigen::Matrix<uint8_t, Eigen::Dynamic, 1>;
using UIntVecDtype = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;
using UIntMatDtype = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using UInt64VecDtype = Eigen::Matrix<uint64_t, Eigen::Dynamic, 1>;
using Int64VecDtype = Eigen::Matrix<int64_t, Eigen::Dynamic, 1>;
using DoubleVecDtype = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using MatShapeDtype = std::vector<Eigen::Index>;
#else // GENIE_CONTACT_BACKEND_STD // GENIE_USE_STD
using BinVecDtype = std::vector<bool>;
using BinMatDtype = std::vector<std::vector<bool>>;
using UInt8VecDtype = std::vector<uint8_t>;
using UIntVecDtype = std::vector<uint32_t>;
using UIntMatDtype = std::vector<std::vector<uint32_t>>;
using UInt64VecDtype = std::vector<uint64_t>;
using Int64VecDtype = std::vector<int64_t>;
using DoubleVecDtype = std::vector<double>;
using MatShapeDtype = std::vector<size_t>;
#endif

enum class TransformID : uint8_t {
    ID_0 = 0,
    ID_1 = 1,
    ID_2 = 2,
    ID_3 = 3,
};

struct RunLengthEncodingData {
  bool firstVal;
  UIntVecDtype rl_entries;
  uint32_t maxCount;
  TransformID transformID;
};

}
#endif // GENIE_CONTACT_TYPES_H
