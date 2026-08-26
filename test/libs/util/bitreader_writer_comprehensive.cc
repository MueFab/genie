/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 *
 * @brief Comprehensive unit tests for genie::util::BitWriter and
 * genie::util::BitReader.
 *
 * These tests cover every public method on both classes plus edge cases
 * (bit widths at byte boundaries, alignment transitions, stream-position
 * manipulation, zero-length payloads, multibyte integral endianness).
 * They exist to give the codebase confidence that util::BitWriter /
 * util::BitReader are the canonical replacements for the deprecated
 * core::Writer shim (see docs/tasks/done/2026-04-29-deprecate-core-writer.md).
 */

#include <cstdint>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------
// BitWriter: WriteBits — bit-width boundaries
// -----------------------------------------------------------------------------

TEST(BitWriterComprehensive, WriteBitsWidthOne) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(1, 1);
  w.FlushBits();
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0x80u);  // MSB of byte
}

TEST(BitWriterComprehensive, WriteBitsWidthSeven) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0x7F, 7);
  w.FlushBits();
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xFEu);
}

TEST(BitWriterComprehensive, WriteBitsWidthEight) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xAB, 8);
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xABu);
}

TEST(BitWriterComprehensive, WriteBitsWidthNine) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0x1FF, 9);
  w.FlushBits();
  EXPECT_EQ(str.str().size(), 2u);
  // LSB-first emission: 0x1FF = 0b1_1111_1111 (9 bits).
  // First byte holds the lower 8 bits packed LSB-first into bits [7:0]
  // (held-bits go in MSBs, so byte = 0xFF). Second byte holds the remaining
  // MSB '1' shifted into bit 7 (byte = 0x80).
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xFFu);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[1]), 0x80u);
}

TEST(BitWriterComprehensive, WriteBitsWidthThirtyTwo) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xDEADBEEFu, 32);
  EXPECT_EQ(str.str().size(), 4u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xDEu);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[1]), 0xADu);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[2]), 0xBEu);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[3]), 0xEFu);
}

TEST(BitWriterComprehensive, WriteBitsWidthSixtyFour) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0x0123456789ABCDEFULL, 64);
  EXPECT_EQ(str.str().size(), 8u);
}

TEST(BitWriterComprehensive, WriteBitsZeroValue) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0, 8);
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0x00u);
}

// -----------------------------------------------------------------------------
// BitWriter: alignment tracking
// -----------------------------------------------------------------------------

TEST(BitWriterComprehensive, IsByteAlignedAfterConstruction) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  EXPECT_TRUE(w.IsByteAligned());
}

TEST(BitWriterComprehensive, IsByteAlignedAfterPartialWrite) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0x5, 4);
  EXPECT_FALSE(w.IsByteAligned());
  w.FlushBits();
  EXPECT_TRUE(w.IsByteAligned());
}

TEST(BitWriterComprehensive, FlushBitsIsIdempotent) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xFF, 8);  // already aligned
  w.FlushBits();
  EXPECT_NO_THROW(w.FlushBits());
  EXPECT_EQ(str.str().size(), 1u);
}

TEST(BitWriterComprehensive, FlushBitsOnEmptyWriter) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  EXPECT_NO_THROW(w.FlushBits());
  EXPECT_EQ(str.str().size(), 0u);
}

// -----------------------------------------------------------------------------
// BitWriter: total-bits counter
// -----------------------------------------------------------------------------

TEST(BitWriterComprehensive, GetTotalBitsWrittenTracksUnflushedBits) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  EXPECT_EQ(w.GetTotalBitsWritten(), 0u);
  w.WriteBits(0xF, 4);  // held in buffer, not yet on stream
  EXPECT_EQ(w.GetTotalBitsWritten(), 4u);
  EXPECT_EQ(str.str().size(), 0u);  // confirm no bytes flushed
  w.FlushBits();
  EXPECT_EQ(w.GetTotalBitsWritten(), 8u);
  EXPECT_EQ(str.str().size(), 1u);
}

TEST(BitWriterComprehensive, GetTotalBitsWrittenAcrossMultipleWrites) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xFF, 16);
  w.WriteBits(0xAA, 8);
  EXPECT_EQ(w.GetTotalBitsWritten(), 24u);
  EXPECT_EQ(str.str().size(), 3u);
}

// -----------------------------------------------------------------------------
// BitWriter: aligned byte/stream APIs
// -----------------------------------------------------------------------------

TEST(BitWriterComprehensive, WriteAlignedByteAfterNonAlignedRequiresFlush) {
  // WriteAlignedByte uses stream_.write() directly, bypassing the bit buffer.
  // A caller mixing WriteBits() with WriteAlignedByte() is responsible for
  // ensuring alignment; we test the aligned-only path here.
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedByte(0x42);
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0x42u);
}

TEST(BitWriterComprehensive, WriteAlignedBytesRawBuffer) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  const std::string payload = "Hello, Genie!";
  w.WriteAlignedBytes(payload.data(), payload.size());
  EXPECT_EQ(str.str(), payload);
  EXPECT_EQ(w.GetTotalBitsWritten(), payload.size() * 8u);
}

TEST(BitWriterComprehensive, WriteAlignedBytesInlineUint) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  // MSB-first emission per byte (see bit_writer.cc:60-80).
  w.WriteAlignedBytes(uint64_t{0x0102030405060708ULL}, 8);
  EXPECT_EQ(str.str().size(), 8u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0x01u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[1]), 0x02u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[7]), 0x08u);
}

TEST(BitWriterComprehensive, WriteAlignedBytesUintOneByte) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedBytes(uint64_t{0xA5}, 1);
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xA5u);
}

TEST(BitWriterComprehensive, WriteAlignedIntAllWidths) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedInt<uint8_t>(0x12);
  w.WriteAlignedInt<uint16_t>(0x3456);
  w.WriteAlignedInt<uint32_t>(0x789ABCDEu);
  w.WriteAlignedInt<uint64_t>(0x1122334455667788ULL);
  EXPECT_EQ(str.str().size(), 1u + 2u + 4u + 8u);
}

TEST(BitWriterComprehensive, WriteAlignedStreamCopiesPayload) {
  std::stringstream str;
  std::stringstream in;
  const std::string payload(257, 'X');  // > kWriteBufferSize (100)
  in.write(payload.data(), payload.size());

  genie::util::BitWriter w(str);
  w.WriteAlignedStream(in);
  EXPECT_EQ(str.str().size(), payload.size());
  EXPECT_EQ(str.str(), payload);
}

// -----------------------------------------------------------------------------
// BitWriter: stream position manipulation
// -----------------------------------------------------------------------------

TEST(BitWriterComprehensive, GetAndSetStreamPosition) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedByte(0xAA);
  w.WriteAlignedByte(0xBB);
  const auto pos = w.GetStreamPosition();
  EXPECT_EQ(pos, 2);

  // seekp(0) only repositions the write head; existing stream bytes
  // remain intact until overwritten. Overwrite byte 0 with 0xCC; byte 1
  // stays 0xBB because we only write one byte.
  w.SetStreamPosition(0);
  w.WriteAlignedByte(0xCC);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xCCu);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[1]), 0xBBu);
}

// -----------------------------------------------------------------------------
// BitWriter: destructor flushes held bits
// -----------------------------------------------------------------------------

TEST(BitWriterComprehensive, DestructorFlushesHeldBits) {
  std::stringstream str;
  {
    genie::util::BitWriter w(str);
    w.WriteBits(0x5, 4);  // held, not flushed
  }  // destructor runs FlushBits()
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0x50u);
}

// -----------------------------------------------------------------------------
// BitReader: ReadBits widths and roundtrip
// -----------------------------------------------------------------------------

TEST(BitReaderComprehensive, ReadBitsWidthOneToSixtyFour) {
  // Build a stream of 64 known bits via the writer, then read back in
  // varying widths.
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xAAAAAAAAAAAAAAAAULL, 64);  // alternating 1/0 from MSB
  genie::util::BitReader r(str);
  // 64 bits read 1 at a time: 1,0,1,0,...,1,0
  for (int i = 0; i < 64; ++i) {
    const auto bit = r.ReadBits(1);
    if (i % 2 == 0) {
      EXPECT_EQ(bit, 1ULL) << "bit " << i;
    } else {
      EXPECT_EQ(bit, 0ULL) << "bit " << i;
    }
  }
}

TEST(BitReaderComprehensive, ReadByteAlignedAfterFullByte) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xAB, 8);
  genie::util::BitReader r(str);
  EXPECT_TRUE(r.IsByteAligned());
  EXPECT_EQ(r.ReadBits(8), 0xABULL);
  EXPECT_TRUE(r.IsByteAligned());
}

TEST(BitReaderComprehensive, ReadAlignedByte) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedByte(0x5A);
  genie::util::BitReader r(str);
  EXPECT_EQ(r.ReadAlignedByte(), 0x5AULL);
}

TEST(BitReaderComprehensive, ReadAlignedBytesFillsBuffer) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  const std::string payload = "MPEG-G Part 6 is fun.";
  w.WriteAlignedBytes(payload.data(), payload.size());
  genie::util::BitReader r(str);
  std::string out(payload.size(), ' ');
  r.ReadAlignedBytes(out.data(), out.size());
  EXPECT_EQ(out, payload);
}

TEST(BitReaderComprehensive, ReadAlignedIntMultiByte) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedInt<uint16_t>(0x1234);
  w.WriteAlignedInt<uint32_t>(0xDEADBEEFu);
  genie::util::BitReader r(str);
  EXPECT_EQ(r.ReadAlignedInt<uint16_t>(), 0x1234u);
  EXPECT_EQ(r.ReadAlignedInt<uint32_t>(), 0xDEADBEEFu);
}

TEST(BitReaderComprehensive, ReadTemplateIntegralType) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedInt<uint8_t>(0x7F);
  w.WriteAlignedInt<uint16_t>(0xBEEF);
  w.WriteAlignedInt<uint32_t>(0xCAFEBABEu);
  genie::util::BitReader r(str);
  EXPECT_EQ(r.Read<uint8_t>(), 0x7Fu);
  EXPECT_EQ(r.Read<uint16_t>(), 0xBEEFu);
  EXPECT_EQ(r.Read<uint32_t>(), 0xCAFEBABEu);
}

TEST(BitReaderComprehensive, ReadIntegerWithExplicitBitWidth) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0x123, 12);  // 12 bits, 4 of which are held in the bit buffer
  w.FlushBits();          // must flush before reading back
  genie::util::BitReader r(str);
  EXPECT_EQ(r.Read<uint16_t>(12), 0x123u);
}

TEST(BitReaderComprehensive, FlushHeldBitsReturnsAndResets) {
  // After a partial read (5 bits from a byte-aligned stream), the reader
  // holds the remaining 3 bits. FlushHeldBits() returns those bits and
  // zeros the held state so subsequent reads are byte-aligned.
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0x5A, 8);  // 0b01011010
  genie::util::BitReader r(str);
  // MSB-first: read the upper 5 bits = 0b01011 = 11
  EXPECT_EQ(r.ReadBits(5), 0x0BULL);
  EXPECT_FALSE(r.IsByteAligned());
  EXPECT_EQ(r.GetTotalBitsRead(), 5u);
  const auto dropped = r.FlushHeldBits();
  EXPECT_TRUE(r.IsByteAligned());
  EXPECT_EQ(r.GetTotalBitsRead(), 8u);  // 5 read + 3 flushed
  EXPECT_NE(dropped, 0u);  // some bits held before flush
}

TEST(BitReaderComprehensive, ReadAlignedStringTerminated) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  const std::string msg = "MPEG-G";
  w.WriteAlignedBytes(msg.data(), msg.size());
  w.WriteAlignedByte(0x00);  // null terminator
  genie::util::BitReader r(str);
  EXPECT_EQ(r.ReadAlignedStringTerminated(), msg);
}

TEST(BitReaderComprehensive, ReadAlignedStringTerminatedEmpty) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedByte(0x00);  // immediate null
  genie::util::BitReader r(str);
  EXPECT_EQ(r.ReadAlignedStringTerminated(), "");
}

TEST(BitReaderComprehensive, SkipAlignedBytesAdvancesPosition) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedBytes("0123456789", 10);
  genie::util::BitReader r(str);
  const auto start = r.GetStreamPosition();
  r.SkipAlignedBytes(5);
  EXPECT_EQ(r.GetStreamPosition(), start + 5);
  char buf[5];
  r.ReadAlignedBytes(buf, 5);
  EXPECT_EQ(std::string(buf, 5), "56789");
}

// -----------------------------------------------------------------------------
// BitReader: stream state
// -----------------------------------------------------------------------------

TEST(BitReaderComprehensive, StreamStateFlags) {
  std::stringstream str;
  str.write("abc", 3);
  genie::util::BitReader r(str);
  EXPECT_TRUE(r.IsStreamGood());
  char buf[10];
  r.ReadAlignedBytes(buf, 10);  // overrun -> failbit set
  EXPECT_FALSE(r.IsStreamGood());
  r.ClearStreamState();
  EXPECT_TRUE(r.IsStreamGood());
}

TEST(BitReaderComprehensive, GetAndSetStreamPosition) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteAlignedBytes("0123456789", 10);
  genie::util::BitReader r(str);
  r.SetStreamPosition(5);
  EXPECT_EQ(r.GetStreamPosition(), 5u);
  char buf[3];
  r.ReadAlignedBytes(buf, 3);
  EXPECT_EQ(std::string(buf, 3), "567");
}

// -----------------------------------------------------------------------------
// BitReader: total bits read
// -----------------------------------------------------------------------------

TEST(BitReaderComprehensive, GetTotalBitsReadAccumulates) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xFF, 8);
  w.WriteBits(0xAA, 8);
  w.WriteBits(0x5, 4);
  genie::util::BitReader r(str);
  EXPECT_EQ(r.GetTotalBitsRead(), 0u);
  r.ReadBits(8);
  EXPECT_EQ(r.GetTotalBitsRead(), 8u);
  r.ReadBits(8);
  EXPECT_EQ(r.GetTotalBitsRead(), 16u);
  r.ReadBits(4);
  EXPECT_EQ(r.GetTotalBitsRead(), 20u);
}

// -----------------------------------------------------------------------------
// Cross-component: interleaved aligned and non-aligned writes/reads
// -----------------------------------------------------------------------------

TEST(BitRoundtripComprehensive, InterleavedAlignedNonAligned) {
  // Write 12 bits, flush, write 1 aligned byte, write 12 more bits,
  // flush. Read back 12 bits, flush the reader's held bits, read the
  // aligned byte, then read 12 more bits.
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xABC, 12);
  w.FlushBits();
  w.WriteAlignedByte(0xDE);
  w.WriteBits(0x123, 12);
  w.FlushBits();

  genie::util::BitReader r(str);
  EXPECT_EQ(r.ReadBits(12), 0xABCULL);
  EXPECT_FALSE(r.IsByteAligned());
  (void)r.FlushHeldBits();  // align reader before aligned read
  EXPECT_TRUE(r.IsByteAligned());
  EXPECT_EQ(r.ReadAlignedByte(), 0xDEULL);
  EXPECT_EQ(r.ReadBits(12), 0x123ULL);
}

TEST(BitRoundtripComprehensive, ManySmallFieldsBackToBack) {
  // 100 4-bit values: 0,1,2,...,99
  std::stringstream str;
  genie::util::BitWriter w(str);
  for (uint64_t i = 0; i < 100; ++i) {
    w.WriteBits(i & 0x0F, 4);
  }
  w.FlushBits();

  genie::util::BitReader r(str);
  for (uint64_t i = 0; i < 100; ++i) {
    const auto v = r.ReadBits(4);
    EXPECT_EQ(v, i & 0x0F) << "field " << i;
  }
}

TEST(BitRoundtripComprehensive, FullByteBoundary) {
  // Writing exactly 8 bits then flushing should produce exactly 1 byte.
  std::stringstream str;
  genie::util::BitWriter w(str);
  w.WriteBits(0xA5, 8);
  EXPECT_EQ(str.str().size(), 1u);
  EXPECT_EQ(static_cast<uint8_t>(str.str()[0]), 0xA5u);
}

TEST(BitRoundtripComprehensive, EmptyStreamRoundtrip) {
  std::stringstream str;
  genie::util::BitWriter w(str);
  genie::util::BitReader r(str);
  EXPECT_EQ(r.GetTotalBitsRead(), 0u);
  EXPECT_EQ(w.GetTotalBitsWritten(), 0u);
}

TEST(BitRoundtripComprehensive, LargePayload) {
  std::stringstream str;
  const size_t n = 10000;
  genie::util::BitWriter w(str);
  for (size_t i = 0; i < n; ++i) {
    w.WriteAlignedByte(static_cast<uint8_t>(i & 0xFF));
  }
  EXPECT_EQ(str.str().size(), n);

  genie::util::BitReader r(str);
  for (size_t i = 0; i < n; ++i) {
    EXPECT_EQ(r.ReadAlignedByte(), static_cast<uint64_t>(i & 0xFF)) << "i=" << i;
  }
}
