/** @file HaplotyperTest.cc
 *  @brief This file contains Test cases for the haplotyper
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#include "QualCodec/HaplotyperTest.h"

#include <iostream>

#include "FilterBuffer.h"
#include "SoftclipSpreader.h"
#include "Haplotyper.h"
#include "QualCodec/Quantizers/LloydMaxQuantizer.h"

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {

void equals(double a, double b, double EPSILON = 0.0001) {
    if ((a-EPSILON) < b && (a+EPSILON) > b) {
        std::cout << "Test passed, " << a << " equals " << b << " !" << std::endl;
    } else {
        std::cout << "Test failed, " << a << " not equal to " << b << " !" << std::endl;
    }
}

// ----------------------------------------------------------------------------------------------------------------------


void gaussKernelTest() {
    // Test variance 1
    GaussKernel k(1.0);

    double buffer[63];
    for (size_t i = 0; i < 63; ++i) {
        buffer[i] = k.calcValue(i, 63);
    }

    equals(buffer[31], 0.3989);
    equals(buffer[32], 0.2419);
    equals(buffer[30], 0.2419);
    equals(buffer[33], 0.0539);
    equals(buffer[29], 0.0539);

    // Test variance 17
    GaussKernel k2(17.0);
    double buffer2[127];
    for (size_t i = 0; i < 127; ++i) {
        buffer2[i] = k2.calcValue(i, 127);
    }

    equals(buffer2[63], 0.0234);
    equals(buffer2[62], 0.0234);
    equals(buffer2[64], 0.0234);
    equals(buffer2[61], 0.0233);
    equals(buffer2[65], 0.0233);
    equals(buffer2[41], 0.0101);
    equals(buffer2[85], 0.0101);

    // Test length limits
    equals(k2.calcMinSize(0.01), 47);
    equals(k.calcMinSize(0.01), 7);
}

// ----------------------------------------------------------------------------------------------------------------------

void filterBufferTest() {
    FilterBuffer buffer([](size_t pos, size_t size)->double{return pos/static_cast<double>(size-1);}, 3);

    // Test "Convolution"
    equals(buffer.filter(), 0);
    buffer.push(1);
    equals(buffer.filter(), 1);
    buffer.push(2);
    equals(buffer.filter(), 2.5);
    buffer.push(3);
    equals(buffer.filter(), 4);
    buffer.push(4);

    // Test offset
    equals(buffer.getOffset(), 2);
}

// ----------------------------------------------------------------------------------------------------------------------

void circBufferTest() {
    CircularBuffer<int> buffer(3, 0);

    // Test size
    equals(buffer.size(), 3);
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);

    // Test access
    equals(buffer.front(), 3);
    equals(buffer[2], 3);
    equals(buffer.back(), 1);
    equals(buffer[0], 1);
    equals(buffer[1], 2);

    // Test push
    equals(buffer.push(0), 1);
    equals(buffer.push(0), 2);
    equals(buffer.push(0), 3);
}

// ----------------------------------------------------------------------------------------------------------------------

void lloydQuantizerTest() {
    // Uniform distribution
    calq::LloydMaxQuantizer q(0, 99, 10);
    for (int i = 0; i <= 99; ++i) {
        q.addToPdf(i);
    }
    q.build();

    equals(q.valueToIndex(99), 9);
    equals(q.valueToIndex(10), 1);
    equals(q.valueToIndex(9), 0);
    equals(q.valueToIndex(38), 3);
    equals(q.valueToIndex(0), 0);

    // Non-Uniform
    calq::LloydMaxQuantizer q2(1, 100, 10);
    for (int i = 1; i <= 100; ++i) {
        q2.addToPdf(i, pow(abs(i - 50), 4));
    }

    q2.build();

    equals(q2.valueToIndex(100), 9);
    equals(q2.valueToIndex(94), 8);
    equals(q2.valueToIndex(11), 1);
    equals(q2.valueToIndex(50), 5);
    equals(q2.valueToIndex(73), 5);
    equals(q2.valueToIndex(1), 0);
}

// ----------------------------------------------------------------------------------------------------------------------

void baseSpreaderTest() {
    SoftclipSpreader b(5, 3, false);

    equals(b.getOffset(), 5);

    // Push 0 out

    equals(b.push(1, 1), 0);
    equals(b.push(1, 2), 0);
    equals(b.push(1, 2), 0);
    equals(b.push(1, 2), 0);
    equals(b.push(1, 3), 0);

    // One base not affected by spread
    equals(b.push(1, 3), 1);

    // One base affected by 1 spread
    equals(b.push(1, 2), 2);

    // Six bases affected by 2 spreads
    equals(b.push(1, 2), 3);
    equals(b.push(1, 2), 3);

    equals(b.push(1, 2), 3);
    equals(b.push(1, 2), 3);
    equals(b.push(1, 2), 3);
    equals(b.push(1, 2), 3);

    // One base affected by 1 spread
    equals(b.push(1, 1), 2);


    // No bases affected anymore
    equals(b.push(1, 0), 1);
    equals(b.push(1, 0), 1);
}

// ----------------------------------------------------------------------------------------------------------------------

void fullHaploTest() {
    Haplotyper h(5, 2, 33, 8, 5, 3, 5);

    equals(h.getOffset(), 10);

    // First 5 pushes inside basespreader
    for (int i = 0; i < 5; ++i) {
        equals(h.push("C", "}", 0, 'A'), 0);
    }

    // next 11 pushes inside filterbuffer approaching 0.73
    for (int i = 0; i < 10; ++i) {
        h.push("C", "}", 0, 'A');
    }
    equals(h.push("C", "}", 0, 'A'), 7);
    equals(h.push("C", "}", 0, 'A'), 7);

    // reset
    Haplotyper h2(5, 2, 33, 8, 5, 3, 5);

    h2.push("CCC", "}}}", 15, 'A');

    // push spreaded bases into filterbuffer
    for (int i = 0; i < 8; ++i) {
         h2.push("CCCCCCCCCCCCCC", "}}}}}}}}}}}}", 0, 'C');  // Activity close to 0
    }
    // Reach maximum
    equals(h2.push("CCCCCCCCCCCCCC", "}}}}}}}}}}}}", 0, 'C'), 7);
    equals(h2.push("CCCCCCCCCCCCCC", "}}}}}}}}}}}}", 0, 'C'), 7);
    equals(h2.push("CCCCCCCCCCCCCC", "}}}}}}}}}}}}", 0, 'C'), 7);

    // Reach minimum
    for (int i = 0; i < 9; ++i) {
         h2.push("CCCCCCCCCCCCCC", "}}}}}}}}}}}}", 0, 'C');
    }

    equals(h2.push("CCCCCCCCCCCCCC", "}}}}}}}}}}}}", 0, 'C'), 0);
}

// ----------------------------------------------------------------------------------------------------------------------

void haplotyperTest() {
    std::cout << "****Starting haplotyper test suite****" << std::endl;
    std::cout << "-> BaseSpreader tests" << std::endl;
    baseSpreaderTest();
    std::cout << "-> Circular buffer tests" << std::endl;
    circBufferTest();
    std::cout << "-> Gauss kernel tests" << std::endl;
    gaussKernelTest();
    std::cout << "-> Filter buffer tests" << std::endl;
    filterBufferTest();
    std::cout << "-> Lloyd-max quantizer tests" << std::endl;
    lloydQuantizerTest();
    std::cout << "-> Full Haplotyper tests" << std::endl;
    fullHaploTest();
    std::cout << "****Haplotyper test suite finished****" << std::endl;
}
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------
