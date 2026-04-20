#ifndef GENIE_BACKEND_BACKEND_H
#define GENIE_BACKEND_BACKEND_H

#include <iostream>
#include <cstddef>
#include <vector>
#include <type_traits>
#include "types.h"
#include <genie/util/bit_writer.h>

namespace genie::backend {

// --- Overloaded Interface for all backends ---

// Implementation declarations (available for backward compatibility)
namespace genie_std_impl {
    void sort_sparse_mat_inplace(::genie::backend::genie_std_impl::UInt64VecDtype& row_ids, ::genie::backend::genie_std_impl::UInt64VecDtype& col_ids, ::genie::backend::genie_std_impl::UIntVecDtype& counts);
    void bin_mat_to_bytes(const ::genie::backend::genie_std_impl::BinMatDtype& bin_mat, uint8_t** payload, size_t& payload_len);
    void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, ::genie::backend::genie_std_impl::BinMatDtype& bin_mat);
    void serialize_mat(const ::genie::backend::genie_std_impl::UIntMatDtype& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload);
    void serialize_arr(const ::genie::backend::genie_std_impl::UIntVecDtype& arr, uint32_t nelems, std::ostream& payload);
    void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, ::genie::backend::genie_std_impl::UIntMatDtype& mat);
    void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, ::genie::backend::genie_std_impl::UIntVecDtype& arr);
    void compute_mask(const ::genie::backend::genie_std_impl::UInt64VecDtype& ids, size_t nelems, ::genie::backend::genie_std_impl::BinVecDtype& mask);
    template<typename T>
    void random_matrix(size_t nrows, size_t ncols, T min, T max, std::vector<std::vector<T>>& mat) {
        mat.assign(nrows, std::vector<T>(ncols));
        for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                mat[idx_i][idx_j] = static_cast<T>(min + (rand() % (static_cast<int>(max - min))));
            }
        }
    }
}

#ifdef GENIE_HAS_XTENSOR_BACKEND
namespace genie_xt_impl {
    template<typename T>
    auto random_matrix(size_t nrows, size_t ncols, T min, T max) {
        auto mat = ::xt::empty<T>({nrows, ncols});
        for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                mat(idx_i, idx_j) = static_cast<T>(min + (rand() % (static_cast<int>(max - min))));
            }
        }
        return mat;
    }
}
#endif

#ifdef GENIE_HAS_EIGEN_BACKEND
namespace genie_eigen_impl {
    template<typename T>
    auto random_matrix(size_t nrows, size_t ncols, T min, T max) {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mat(static_cast<Eigen::Index>(nrows), static_cast<Eigen::Index>(ncols));
        for (Eigen::Index idx_i = 0; idx_i < mat.rows(); ++idx_i) {
            for (Eigen::Index idx_j = 0; idx_j < mat.cols(); ++idx_j) {
                mat(idx_i, idx_j) = static_cast<T>(min + (rand() % static_cast<int>(max - min)));
            }
        }
        return mat;
    }
}
#endif

// --- Helper Utilities ---

namespace detail {
    // Basic detection for XTensor-like and Eigen-like shapes
    template<typename T, typename = void>
    struct has_shape : std::false_type {};
    template<typename T>
    struct has_shape<T, std::void_t<decltype(std::declval<T>().shape())>> : std::true_type {};

    template<typename T, typename = void>
    struct has_rows : std::false_type {};
    template<typename T>
    struct has_rows<T, std::void_t<decltype(std::declval<T>().rows())>> : std::true_type {};

    template<typename T>
    struct is_2d_vector : std::false_type {};
    template<typename T>
    struct is_2d_vector<std::vector<std::vector<T>>> : std::true_type {};

    template<typename T>
    auto random_matrix(size_t nrows, size_t ncols, T min, T max) {
#if defined(GENIE_BACKEND_XTENSOR) && defined(GENIE_HAS_XTENSOR_BACKEND)
        return genie_xt_impl::random_matrix<T>(nrows, ncols, min, max);
#elif defined(GENIE_BACKEND_EIGEN) && defined(GENIE_HAS_EIGEN_BACKEND)
        return genie_eigen_impl::random_matrix<T>(nrows, ncols, min, max);
#else
        std::vector<std::vector<T>> mat;
        genie_std_impl::random_matrix<T>(nrows, ncols, min, max, mat);
        return mat;
#endif
    }

    template<typename T>
    auto amin(const T& mat) {
        if constexpr (has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
            return ::xt::amin(mat)();
#endif
        } else if constexpr (has_rows<T>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
            return mat.minCoeff();
#endif
        } else {
            typename T::value_type::value_type min_val = mat[0][0];
            for (const auto& row : mat) {
                for (const auto& val : row) if (val < min_val) min_val = val;
            }
            return min_val;
        }
    }
}

template<typename T>
size_t get_mat_shape(const T& mat, size_t axis) {
    if constexpr (detail::has_shape<T>::value) {
        return mat.shape()[axis];
    } else if constexpr (detail::has_rows<T>::value) {
        return axis == 0 ? mat.rows() : mat.cols();
    } else {
        if (axis == 0) return mat.size();
        if constexpr (detail::is_2d_vector<T>::value) {
            return mat.empty() ? 0 : mat[0].size();
        } else {
            return 0;
        }
    }
}

template<typename T>
auto get_mat_element(const T& mat, size_t row, size_t col) {
    if constexpr (detail::has_shape<T>::value) {
        return mat(row, col);
    } else if constexpr (detail::has_rows<T>::value) {
        return mat(static_cast<std::ptrdiff_t>(row), static_cast<std::ptrdiff_t>(col));
    } else {
        if constexpr (detail::is_2d_vector<T>::value) {
            return mat[row][col];
        } else {
            return 0;
        }
    }
}

template<typename T, typename V>
void set_mat_element(T& mat, size_t row, size_t col, V val) {
    if constexpr (detail::has_shape<T>::value) {
        mat(row, col) = val;
    } else if constexpr (detail::has_rows<T>::value) {
        mat(static_cast<std::ptrdiff_t>(row), static_cast<std::ptrdiff_t>(col)) = val;
    } else {
        if constexpr (detail::is_2d_vector<T>::value) {
            mat[row][col] = val;
        }
    }
}

template<typename T, typename S>
void resize_mat(T& mat, const S& shape) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        mat = ::xt::empty<typename T::value_type>(shape);
#endif
    } else if constexpr (detail::has_rows<T>::value) {
        mat.resize(static_cast<Eigen::Index>(shape[0]), static_cast<Eigen::Index>(shape[1]));
    } else {
        mat.assign(shape[0], std::vector<typename T::value_type::value_type>(shape[1], 0));
    }
}

template<typename T>
void clear_mat(T& mat) {
    ::genie::backend::resize_mat(mat, std::vector<size_t>{0, 0});
}

template<typename T>
T clone_mat(const T& mat) {
    return mat; // Default copy is fine for our backend types
}

template<typename T>
size_t get_mat_dimension(const T& mat) { return 2; } // Likelihood mats are always 2D natively modeled as 2D

template<typename T>
bool mats_equal(const T& m1, const T& m2) {
    size_t r1 = ::genie::backend::get_mat_shape(m1, 0);
    size_t c1 = ::genie::backend::get_mat_shape(m1, 1);
    size_t r2 = ::genie::backend::get_mat_shape(m2, 0);
    size_t c2 = ::genie::backend::get_mat_shape(m2, 1);
    if (r1 != r2 || c1 != c2) return false;
    for (size_t idx_i = 0; idx_i < r1; ++idx_i) {
        for (size_t idx_j = 0; idx_j < c1; ++idx_j) {
            if (::genie::backend::get_mat_element(m1, idx_i, idx_j) != ::genie::backend::get_mat_element(m2, idx_i, idx_j)) return false;
        }
    }
    return true;
}

template<typename T>
void resize_arr(T& arr, size_t nelems) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        arr = ::xt::empty<typename T::value_type>({nelems});
#endif
    } else if constexpr (detail::has_rows<T>::value) {
        arr.resize(static_cast<Eigen::Index>(nelems));
    } else {
        arr.resize(nelems);
    }
}

template<typename T>
auto get_arr_element(const T& arr, size_t idx) {
    if constexpr (detail::has_shape<T>::value) {
        return arr(idx);
    } else if constexpr (detail::has_rows<T>::value) {
        return arr(static_cast<Eigen::Index>(idx));
    } else {
        return arr[idx];
    }
}

template<typename T, typename V>
void set_arr_element(T& arr, size_t idx, V val) {
    if constexpr (detail::has_shape<T>::value) {
        arr(idx) = val;
    } else if constexpr (detail::has_rows<T>::value) {
        arr(static_cast<Eigen::Index>(idx)) = val;
    } else {
        arr[idx] = val;
    }
}

template<typename T>
size_t get_arr_size(const T& arr) {
    if constexpr (detail::has_shape<T>::value) {
        return arr.size();
    } else if constexpr (detail::has_rows<T>::value) {
        return static_cast<size_t>(arr.size());
    } else {
        return arr.size();
    }
}

template<typename T>
void clear_arr(T& arr) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        arr = ::xt::empty<typename T::value_type>({0});
#endif
    } else if constexpr (detail::has_rows<T>::value) {
        arr.resize(0);
    } else {
        arr.clear();
    }
}

template<typename T, typename V>
void append_arr_element(T& arr, const V& val) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        // xtensor is not efficient at appends, but for unification:
        auto current_size = arr.size();
        auto old_arr = arr;
        arr.resize({current_size + 1});
        for(size_t i=0; i<current_size; ++i) arr(i) = old_arr(i);
        arr(current_size) = val;
#endif
    } else if constexpr (detail::has_rows<T>::value) {
        auto current_size = arr.size();
        arr.conservativeResize(current_size + 1);
        arr(current_size) = val;
    } else {
        arr.push_back(val);
    }
}

template<typename T, typename V>
void assign_arr(T& arr, size_t nelems, const V& val) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        arr = ::xt::empty<typename T::value_type>({nelems});
        arr.fill(val);
#endif
    } else if constexpr (detail::has_rows<T>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        arr = T::Constant(static_cast<std::ptrdiff_t>(nelems), val);
#endif
    } else {
        arr.assign(nelems, val);
    }
}

template<typename T, typename V>
void fill_arr(T& arr, size_t start, size_t end, const V& val) {
    if constexpr (detail::has_shape<T>::value) {
        for (size_t idx_i = start; idx_i < end; ++idx_i) arr(idx_i) = val;
    } else if constexpr (detail::has_rows<T>::value) {
        for (size_t idx_i = start; idx_i < end; ++idx_i) arr(static_cast<std::ptrdiff_t>(idx_i)) = val;
    } else {
        std::fill(arr.begin() + start, arr.begin() + end, val);
    }
}

template<typename T, typename V>
bool any_equal(const T& mat, V val) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        return ::xt::any(::xt::equal(mat, val));
#else
        return false;
#endif
    } else if constexpr (detail::has_rows<T>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        return (mat.array() == val).any();
#else
        return false;
#endif
    } else {
        if constexpr (detail::is_2d_vector<T>::value) {
            for (const auto& row : mat) {
                for (const auto& v : row) if (v == val) return true;
            }
        }
        return false;
    }
}

template<typename T>
auto max_coeff(const T& mat) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        using ValType = typename T::value_type;
        return static_cast<ValType>(::xt::amax(mat)(0));
#else
        return int8_t(0);
#endif
    } else if constexpr (detail::has_rows<T>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        return mat.maxCoeff();
#else
        return int8_t(0);
#endif
    } else {
        using ValType = typename T::value_type::value_type;
        ValType current_max = -128; // Fallback for signed char
        if constexpr (detail::is_2d_vector<T>::value) {
            bool first = true;
            for (const auto& row : mat) {
                for (const auto& v : row) {
                    if (first || v > current_max) {
                        current_max = v;
                        first = false;
                    }
                }
            }
        }
        return current_max;
    }
}

template<typename T, typename V>
void replace_value(T& mat, V cond_val, V target_val) {
    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        for (auto it = mat.begin(); it != mat.end(); ++it) {
            if (*it == cond_val) *it = target_val;
        }
#endif
    } else if constexpr (detail::has_rows<T>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        for(int idx_i=0; idx_i<mat.rows(); ++idx_i)
            for(int idx_j=0; idx_j<mat.cols(); ++idx_j)
                if (mat(idx_i,idx_j) == cond_val) mat(idx_i,idx_j) = target_val;
#endif
    } else {
        if constexpr (detail::is_2d_vector<T>::value) {
            for (auto& row : mat) {
                for (auto& v : row) if (v == cond_val) v = target_val;
            }
        }
    }
}

template<typename T, typename B>
void get_bit_plane(const T& mat, uint8_t bit, B& bit_mat) {
    size_t nrows = get_mat_shape(mat, 0);
    size_t ncols = get_mat_shape(mat, 1);
    resize_mat(bit_mat, std::vector<size_t>{nrows, ncols});

    if constexpr (detail::has_shape<T>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        bit_mat = ::xt::eval(::xt::cast<bool>(mat & (1 << bit)));
#endif
    } else {
        for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                set_mat_element(bit_mat, idx_i, idx_j, (get_mat_element(mat, idx_i, idx_j) & (1 << bit)) != 0);
            }
        }
    }
}

// --- Template Dispatcher Implementation ---

template<typename Type1, typename Type2, typename Type3>
void sort_sparse_mat_inplace(Type1& row_ids, Type2& col_ids, Type3& counts) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::sort_sparse_mat_inplace(row_ids, col_ids, counts);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::sort_sparse_mat_inplace(row_ids, col_ids, counts);
#endif
    } else {
        genie_std_impl::sort_sparse_mat_inplace(row_ids, col_ids, counts);
    }
}

template<typename Type1>
void bin_mat_to_bytes(const Type1& bin_mat, uint8_t** payload, size_t& payload_len) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::bin_mat_to_bytes(bin_mat, payload, payload_len);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::bin_mat_to_bytes(bin_mat, payload, payload_len);
#endif
    } else {
        genie_std_impl::bin_mat_to_bytes(bin_mat, payload, payload_len);
    }
}

template<typename Type1>
void bin_mat_from_bytes(const uint8_t* payload, size_t payload_len, size_t nrows, size_t ncols, Type1& bin_mat) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
#endif
    } else {
        genie_std_impl::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
    }
}

template<typename Type1>
void serialize_mat(const Type1& mat, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, std::ostream& payload) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::serialize_mat(mat, dtype_id, nrows, ncols, payload);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::serialize_mat(mat, dtype_id, nrows, ncols, payload);
#endif
    } else {
        genie_std_impl::serialize_mat(mat, dtype_id, nrows, ncols, payload);
    }
}

template<typename Type1>
void serialize_arr(const Type1& arr, uint32_t nelems, std::ostream& payload) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::serialize_arr(arr, nelems, payload);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::serialize_arr(arr, nelems, payload);
#endif
    } else {
        genie_std_impl::serialize_arr(arr, nelems, payload);
    }
}

template<typename Type1>
void deserialize_mat(const std::vector<uint8_t>& payload_bytes, uint32_t dtype_id, uint32_t nrows, uint32_t ncols, Type1& mat) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::deserialize_mat(payload_bytes, dtype_id, nrows, ncols, mat);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::deserialize_mat(payload_bytes, dtype_id, nrows, ncols, mat);
#endif
    } else {
        genie_std_impl::deserialize_mat(payload_bytes, dtype_id, nrows, ncols, mat);
    }
}

template<typename Type1>
void deserialize_arr(const std::vector<uint8_t>& payload_bytes, uint32_t nelems, Type1& arr) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::deserialize_arr(payload_bytes, nelems, arr);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::deserialize_arr(payload_bytes, nelems, arr);
#endif
    } else {
        genie_std_impl::deserialize_arr(payload_bytes, nelems, arr);
    }
}

template<typename Type1, typename Type2>
void compute_mask(const Type1& ids, size_t nelems, Type2& mask) {
    if constexpr (detail::has_rows<Type1>::value) {
#ifdef GENIE_HAS_EIGEN_BACKEND
        genie_eigen_impl::compute_mask(ids, nelems, mask);
#endif
    } else if constexpr (detail::has_shape<Type1>::value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        genie_xt_impl::compute_mask(ids, nelems, mask);
#endif
    } else {
        genie_std_impl::compute_mask(ids, nelems, mask);
    }
}

} // namespace genie::backend

#endif // GENIE_BACKEND_BACKEND_H
