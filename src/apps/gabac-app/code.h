#ifndef GABACIFY_CODE_H_
#define GABACIFY_CODE_H_

#include <cstddef>
#include <string>

namespace gabacify {

void code(const std::string& inputFilePath,
          // RESTRUCT_DISABLE const std::string& configurationFilePath,
          const std::string& outputFilePath,
          size_t blocksize,
          uint8_t descID,
          uint8_t subseqID,
          bool decode,
          const std::string& dependencyFilePath = "");

}  // namespace gabacify

#endif  // GABACIFY_CODE_H_
