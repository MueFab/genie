#include <gtest/gtest.h>
#include "genie/core/ndarray.h"

TEST(NDArrayTest, DefaultConstructor) {
    genie::core::NDArray<int> arr;
    EXPECT_EQ(arr.shape().size(), 0);
    EXPECT_EQ(arr.size(), 0);
}

TEST(NDArrayTest, ConstructorWithMoveBuffer) {
    std::vector<int> buffer = {1, 2, 3, 4, 5, 6};
    genie::core::NDArray<int> arr({2, 3}, std::move(buffer));
    EXPECT_EQ(arr.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr.size(), 6);
    EXPECT_EQ(arr({0, 0}), 1);
    EXPECT_EQ(arr({1, 2}), 6);
}

TEST(NDArrayTest, ConstructorWithBufferReference) {
    std::vector<int> buffer = {1, 2, 3, 4, 5, 6};
    genie::core::NDArray<int> arr({2, 3}, buffer);
    EXPECT_EQ(arr.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr.size(), 6);
    EXPECT_EQ(arr({0, 0}), 1);
    EXPECT_EQ(arr({1, 2}), 6);
}

TEST(NDArrayTest, MoveConstructor) {
    std::vector<int> buffer = {1, 2, 3, 4, 5, 6};
    genie::core::NDArray<int> arr1({2, 3}, std::move(buffer));
    genie::core::NDArray<int> arr2 = std::move(arr1);
    EXPECT_EQ(arr2.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr2.size(), 6);
    EXPECT_EQ(arr2({0, 0}), 1);
    EXPECT_EQ(arr2({1, 2}), 6);
}

TEST(NDArrayTest, CopyConstructor) {
    std::vector<int> buffer = {1, 2, 3, 4, 5, 6};
    genie::core::NDArray<int> arr1({2, 3}, buffer);
    genie::core::NDArray<int> arr2 = arr1;
    EXPECT_EQ(arr2.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr2.size(), 6);
    EXPECT_EQ(arr2({0, 0}), 1);
    EXPECT_EQ(arr2({1, 2}), 6);
}

TEST(NDArrayTest, MoveAssignment) {
    std::vector<int> buffer = {1, 2, 3, 4, 5, 6};
    genie::core::NDArray<int> arr1({2, 3}, std::move(buffer));
    genie::core::NDArray<int> arr2;
    arr2 = std::move(arr1);
    EXPECT_EQ(arr2.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr2.size(), 6);
    EXPECT_EQ(arr2({0, 0}), 1);
    EXPECT_EQ(arr2({1, 2}), 6);
}

TEST(NDArrayTest, CopyAssignment) {
    std::vector<int> buffer = {1, 2, 3, 4, 5, 6};
    genie::core::NDArray<int> arr1({2, 3}, buffer);
    genie::core::NDArray<int> arr2;
    arr2 = arr1;
    EXPECT_EQ(arr2.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr2.size(), 6);
    EXPECT_EQ(arr2({0, 0}), 1);
    EXPECT_EQ(arr2({1, 2}), 6);
}

TEST(NDArrayTest, Resize) {
    genie::core::NDArray<int> arr({2, 3});
    arr.resize({3, 3}, 0);
    EXPECT_EQ(arr.shape(), std::vector<size_t>({3, 3}));
    EXPECT_EQ(arr.size(), 9);
    EXPECT_EQ(arr({2, 2}), 0);
}

TEST(NDArrayTest, AccessOperatorVariadic) {
    genie::core::NDArray<int> arr({2, 3}, {1, 2, 3, 4, 5, 6});
    EXPECT_EQ(arr(0, 0), 1);
    EXPECT_EQ(arr(1, 2), 6);
}

TEST(NDArrayTest, AccessOperatorVector) {
    genie::core::NDArray<int> arr({2, 3}, {1, 2, 3, 4, 5, 6});
    EXPECT_EQ(arr({0, 0}), 1);
    EXPECT_EQ(arr({1, 2}), 6);
}

TEST(NDArrayTest, ConstAccessOperatorVariadic) {
    const genie::core::NDArray<int> arr({2, 3}, {1, 2, 3, 4, 5, 6});
    EXPECT_EQ(arr(0, 0), 1);
    EXPECT_EQ(arr(1, 2), 6);
}

TEST(NDArrayTest, ConstAccessOperatorVector) {
    const genie::core::NDArray<int> arr({2, 3}, {1, 2, 3, 4, 5, 6});
    EXPECT_EQ(arr({0, 0}), 1);
    EXPECT_EQ(arr({1, 2}), 6);
}

TEST(NDArrayTest, Shape) {
    genie::core::NDArray<int> arr({2, 3});
    EXPECT_EQ(arr.shape(), std::vector<size_t>({2, 3}));
}

TEST(NDArrayTest, Size) {
    genie::core::NDArray<int> arr({2, 3});
    EXPECT_EQ(arr.size(), 6);
}

TEST(NDArrayTest, Fill) {
    genie::core::NDArray<int> arr({2, 3});
    arr.fill(7);
    EXPECT_EQ(arr({0, 0}), 7);
    EXPECT_EQ(arr({1, 2}), 7);
}

TEST(NDArrayTest, Flatten) {
    genie::core::NDArray<int> arr({2, 3}, {1, 2, 3, 4, 5, 6});
    genie::core::NDArray<int> flatArr = arr.flatten();
    EXPECT_EQ(flatArr.shape(), std::vector<size_t>({6}));
    EXPECT_EQ(flatArr.size(), 6);
    EXPECT_EQ(flatArr(0), 1);
    EXPECT_EQ(flatArr(5), 6);
}

TEST(NDArrayTest, NDArrayFactoryCreateDefault) {
    genie::core::NDArray<int> arr = genie::core::NDArrayFactory<int>::create({2, 3});
    EXPECT_EQ(arr.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr.size(), 6);
}

TEST(NDArrayTest, NDArrayFactoryCreateWithValue) {
    genie::core::NDArray<int> arr = genie::core::NDArrayFactory<int>::create({2, 3}, 5);
    EXPECT_EQ(arr.shape(), std::vector<size_t>({2, 3}));
    EXPECT_EQ(arr.size(), 6);
    EXPECT_EQ(arr({0, 0}), 5);
    EXPECT_EQ(arr({1, 2}), 5);
}

//TEST(NDArrayTest, ElementWiseAddition) {
//    genie::core::NDArray<int> arr1({2, 3}, {1, 2, 3, 4, 5, 6});
//    genie::core::NDArray<int> arr2({2, 3}, {6, 5, 4, 3, 2, 1});
//    genie::core::NDArray<int> result = arr1 + arr2;
//    EXPECT_EQ(result.shape(), std::vector<size_t>({2, 3}));
//    EXPECT_EQ(result({0, 0}), 7);
//    EXPECT_EQ(result({1, 2}), 7);
//}
//
//TEST(NDArrayTest, ElementWiseSubtraction) {
//    genie::core::NDArray<int> arr1({2, 3}, {1, 2, 3, 4, 5, 6});
//    genie::core::NDArray<int> arr2({2, 3}, {6, 5, 4, 3, 2, 1});
//    genie::core::NDArray<int> result = arr1 - arr2;
//    EXPECT_EQ(result.shape(), std::vector<size_t>({2, 3}));
//    EXPECT_EQ(result({0, 0}), -5);
//    EXPECT_EQ(result({1, 2}), 5);
//}
//
//TEST(NDArrayTest, ElementWiseMultiplication) {
//    genie::core::NDArray<int> arr1({2, 3}, {1, 2, 3, 4, 5, 6});
//    genie::core::NDArray<int> arr2({2, 3}, {6, 5, 4, 3, 2, 1});
//    genie::core::NDArray<int> result = arr1 * arr2;
//    EXPECT_EQ(result.shape(), std::vector<size_t>({2, 3}));
//    EXPECT_EQ(result({0, 0}), 6);
//    EXPECT_EQ(result({1, 2}), 6);
//}
//
//TEST(NDArrayTest, ElementWiseDivision) {
//    genie::core::NDArray<int> arr1({2, 3}, {10, 20, 30, 40, 50, 60});
//    genie::core::NDArray<int> arr2({2, 3}, {2, 4, 5, 5, 10, 12});
//    genie::core::NDArray<int> result = arr1 / arr2;
//    EXPECT_EQ(result.shape(), std::vector<size_t>({2, 3}));
//    EXPECT_EQ(result({0, 0}), 5);
//    EXPECT_EQ(result({1, 2}), 5);
//}

//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}