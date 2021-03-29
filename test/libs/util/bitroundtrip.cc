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
    genie::util::BitWriter writer(&str);
    writer.write(0x2345234523452345, 64);
    writer.write(0x23452345, 32);
    writer.write(0x2345, 16);
    writer.write(0x45, 8);
    writer.write(0x5, 4);
    writer.write(0x1, 2);
    writer.write(0x1, 1);
    writer.write(0x23452345, 37);
    writer.flush();

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
    genie::util::BitWriter writer(&str);
    std::string input = "Hello World!!!";
    writer.write(input);

    genie::util::BitReader reader(str);
    std::string output(input.size(), ' ');
    reader.readBypass(output);
    EXPECT_EQ(input, output);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitRoundtrip, buffer) {
    std::stringstream str;
    genie::util::BitWriter writer(&str);
    std::string input = "Hello World!!!";
    writer.writeBypass(input.c_str(), input.size());

    genie::util::BitReader reader(str);
    std::string output(input.size(), ' ');
    reader.readBypass(&output[0], output.size());
    EXPECT_EQ(input, output);
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitRoundtrip, stream) {
    std::stringstream str;
    std::stringstream input;

    std::string inputstring = "Hello World!";
    uint32_t inputnumber = 42;

    genie::util::BitWriter writer(&str);
    {
        genie::util::BitWriter writer2(&input);
        writer2.write(inputnumber, sizeof(uint32_t) * 8);
        writer2.write(inputstring);
        input.flush();
    }
    writer.write(&input);
    input.clear();
   {
        genie::util::BitWriter writer2(&input);
        writer2.write(inputnumber, sizeof(uint32_t)*8);
        writer2.write(inputstring);
        input.flush();
    }
    writer.write(&input);

    genie::util::BitReader reader(str);
    std::string outputstring(inputstring.size(), ' ');
    EXPECT_EQ(reader.read<uint32_t>(), inputnumber);
    reader.readBypass(outputstring);
    EXPECT_EQ(outputstring, inputstring);

    outputstring = std::string(inputstring.size(), ' ');
    EXPECT_EQ(reader.read<uint32_t>(), inputnumber);
    reader.readBypass(outputstring);
    EXPECT_EQ(outputstring, inputstring);

}

// ---------------------------------------------------------------------------------------------------------------------