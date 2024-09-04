/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitRoundtrip, numbers) {
    std::stringstream str;
    genie::util::BitWriter writer(str);
    writer.writeBits(0x2345234523452345, 64);
    writer.writeBits(0x23452345, 32);
    writer.writeBits(0x2345, 16);
    writer.writeBits(0x45, 8);
    writer.writeBits(0x5, 4);
    writer.writeBits(0x1, 2);
    writer.writeBits(0x1, 1);
    writer.writeBits(0x23452345, 37);
    writer.flushBits();

    genie::util::BitReader reader(str);

    EXPECT_EQ(reader.read<uint64_t>(64), 0x2345234523452345ULL);
    EXPECT_EQ(reader.read<uint64_t>(32), 0x23452345ULL);
    EXPECT_EQ(reader.read<uint64_t>(16), 0x2345ULL);
    EXPECT_EQ(reader.read<uint64_t>(8), 0x45ULL);
    EXPECT_EQ(reader.read<uint64_t>(4), 0x5ULL);
    EXPECT_EQ(reader.read<uint64_t>(2), 0x1ULL);
    EXPECT_EQ(reader.read<uint64_t>(1), 0x1ULL);
    EXPECT_EQ(reader.read<uint64_t>(37), 0x23452345ULL);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitRoundtrip, string) {
    std::stringstream str;
    genie::util::BitWriter writer(str);
    std::string input = "Hello World!!!";
    writer.writeAlignedBytes(input.data(), input.length());

    genie::util::BitReader reader(str);
    std::string output(input.size(), ' ');
    reader.readAlignedBytes(output.data(), output.size());
    EXPECT_EQ(input, output);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitRoundtrip, buffer) {
    std::stringstream str;
    genie::util::BitWriter writer(str);
    std::string input = "Hello World!!!";
    writer.writeAlignedBytes(input.c_str(), input.size());

    genie::util::BitReader reader(str);
    std::string output(input.size(), ' ');
    reader.readAlignedBytes(&output[0], output.size());
    EXPECT_EQ(input, output);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitRoundtrip, stream) {
    std::stringstream str;
    std::stringstream input;

    std::string inputstring = "Hello World!";
    uint32_t inputnumber = 42;

    genie::util::BitWriter writer(str);
    {
        genie::util::BitWriter writer2(input);
        writer2.writeBits(inputnumber, sizeof(uint32_t) * 8);
        writer2.writeAlignedBytes(inputstring.data(), inputstring.length());
        input.flush();
    }
    writer.writeAlignedStream(input);
    input.clear();
   {
        genie::util::BitWriter writer2(input);
        writer2.writeBits(inputnumber, sizeof(uint32_t)*8);
        writer2.writeAlignedBytes(inputstring.data(), inputstring.length());
        input.flush();
    }
    writer.writeAlignedStream(input);

    genie::util::BitReader reader(str);
    std::string outputstring(inputstring.size(), ' ');
    EXPECT_EQ(reader.read<uint32_t>(), inputnumber);
    reader.readAlignedBytes(outputstring.data(), outputstring.length());
    EXPECT_EQ(outputstring, inputstring);

    outputstring = std::string(inputstring.size(), ' ');
    EXPECT_EQ(reader.read<uint32_t>(), inputnumber);
    reader.readAlignedBytes(outputstring.data(), outputstring.length());
    EXPECT_EQ(outputstring, inputstring);

}

// ---------------------------------------------------------------------------------------------------------------------