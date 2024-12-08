/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/stream_handler.h"

#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

#define UINT_MAX_LENGTH (sizeof(uint64_t))
#define U7_MAX_LENGTH (((sizeof(uint64_t) + 6) / 7) * 8)

// -----------------------------------------------------------------------------

size_t StreamHandler::ReadUInt(std::istream& input, uint64_t& ret_val,
                               size_t num_bytes) {
  uint8_t bytes[UINT_MAX_LENGTH] = {};  // NOLINT

  input.read(reinterpret_cast<char*>(bytes),
             static_cast<std::streamsize>(num_bytes));

  ret_val = 0;
  size_t c = 0;  // counter
  while (num_bytes-- > 0) {
    ret_val = ret_val << 8 | bytes[c++];
  }

  return c;
}

// -----------------------------------------------------------------------------

size_t StreamHandler::ReadU7(std::istream& input, uint64_t& ret_val) {
  size_t c = 0;  // counter
  uint8_t byte;
  ret_val = 0;

  do {
    if (c == U7_MAX_LENGTH) UTILS_DIE("ReadU7: out of range");
    c++;

    input.read(reinterpret_cast<char*>(&byte), 1);
    ret_val = (ret_val << 7) | (byte & 0x7F);  // NOLINT
  } while ((byte & 0x80) != 0);

  return c;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] size_t StreamHandler::ReadStream(std::istream& input,
                                                  util::DataBlock* buffer,
                                                  uint64_t& num_symbols) {
  size_t stream_size = 0;
  uint64_t payload_size = 0;
  stream_size += ReadUInt(input, payload_size, 4);

  num_symbols = 0;
  if (payload_size >= 4) {
    stream_size += ReadUInt(input, num_symbols, 4);
    stream_size += ReadBytes(input, payload_size - 4, buffer);
  }

  return stream_size;
}

// -----------------------------------------------------------------------------

size_t StreamHandler::ReadBytes(std::istream& input, const size_t bytes,
                                util::DataBlock* buffer) {
  if (bytes > 0) {
    if (bytes % buffer->GetWordSize()) {
      UTILS_DIE("Input stream length not a multiple of word Size");
    }
    buffer->Resize(bytes / buffer->GetWordSize());
    input.read(static_cast<char*>(buffer->GetData()),
               static_cast<std::streamsize>(bytes));
  }
  return bytes;
}

// -----------------------------------------------------------------------------

size_t StreamHandler::ReadFull(std::istream& input, util::DataBlock* buffer) {
  const auto safe = input.exceptions();
  input.exceptions(std::ios::badbit);

  const size_t buffer_size =
      static_cast<size_t>(1000000) / buffer->GetWordSize();
  buffer->Resize(0);
  while (input.good()) {
    const size_t pos = buffer->Size();
    buffer->Resize(pos + buffer_size);
    input.read(
        static_cast<char*>(buffer->GetData()) + pos * buffer->GetWordSize(),
        static_cast<std::streamsize>(buffer_size * buffer->GetWordSize()));
  }
  if (!input.eof()) {
    UTILS_DIE("Error while reading input stream");
  }
  if (input.gcount() % buffer->GetWordSize()) {
    UTILS_DIE("Input stream length not a multiple of word Size");
  }
  buffer->Resize(buffer->Size() -
                 (buffer_size - input.gcount() / buffer->GetWordSize()));
  input.exceptions(safe);
  return buffer->Size();
}

// -----------------------------------------------------------------------------

[[maybe_unused]] size_t StreamHandler::ReadBlock(std::istream& input,
                                                 const size_t bytes,
                                                 util::DataBlock* buffer) {
  const auto safe = input.exceptions();
  input.exceptions(std::ios::badbit);

  if (bytes % buffer->GetWordSize()) {
    UTILS_DIE("Input stream length not a multiple of word Size");
  }
  const size_t buffer_size = bytes / buffer->GetWordSize();
  buffer->Resize(buffer_size);
  input.read(static_cast<char*>(buffer->GetData()),
             static_cast<std::streamsize>(buffer_size * buffer->GetWordSize()));
  if (!input.good()) {
    if (!input.eof()) {
      UTILS_DIE("Error while reading input stream");
    }
    buffer->Resize(buffer->Size() -
                   (buffer_size - input.gcount() / buffer->GetWordSize()));
  }
  input.exceptions(safe);
  return buffer->Size();
}

// -----------------------------------------------------------------------------

size_t StreamHandler::ReadStreamSize(std::istream& input) {
  input.seekg(0, std::istream::end);
  const size_t length = input.tellg();
  input.seekg(0, std::istream::beg);

  return length;
}

// -----------------------------------------------------------------------------

size_t StreamHandler::WriteUInt(std::ostream& output, const uint64_t value,
                                size_t num_bytes) {
  uint8_t bytes[UINT_MAX_LENGTH] = {};
  size_t c = 0;  // counter
  while (num_bytes-- > 0) {
    bytes[c++] = value >> num_bytes * 8 & 0xFF;
  }
  output.write(reinterpret_cast<char*>(bytes), static_cast<std::streamsize>(c));

  return c;
}

// -----------------------------------------------------------------------------

size_t StreamHandler::WriteU7(std::ostream& output, const uint64_t value) {
  uint8_t bytes[U7_MAX_LENGTH] = {};
  size_t c = 0;  // counter
  int shift;
  constexpr int input_max_size = sizeof(value) * 8;
  for (shift = 0; shift < input_max_size && value >> shift != 0; shift += 7) {
  }
  if (shift > 0) shift -= 7;

  for (; shift >= 0; shift -= 7) {
    const auto code = static_cast<uint8_t>(
        ((value >> shift) & 0x7F) | (shift > 0 ? 0x80 : 0x00));  // NOLINT
    bytes[c++] = code;
  }

  output.write(reinterpret_cast<char*>(bytes), static_cast<std::streamsize>(c));

  return c;
}

// -----------------------------------------------------------------------------

[[maybe_unused]] size_t StreamHandler::WriteStream(std::ostream& output,
                                                   util::DataBlock* buffer,
                                                   const uint64_t num_symbols) {
  size_t stream_size = 0;
  const uint64_t payload_size = buffer->GetRawSize() + (num_symbols ? 4 : 0);
  stream_size += WriteUInt(output, payload_size, 4);
  if (payload_size >= 4) {
    stream_size += WriteUInt(output, num_symbols, 4);
    stream_size += WriteBytes(output, buffer);
  }
  return stream_size;
}

// -----------------------------------------------------------------------------

size_t StreamHandler::WriteBytes(std::ostream& output,
                                 util::DataBlock* buffer) {
  const size_t ret = buffer->GetRawSize();
  if (ret > 0) {
    output.write(static_cast<char*>(buffer->GetData()),
                 static_cast<std::streamsize>(ret));
    buffer->Clear();
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
