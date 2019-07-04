#ifndef GABACIFY_ANALYZE_H_
#define GABACIFY_ANALYZE_H_

#include <string>

namespace gabacify {

void analyze(const std::string& inputFilePath,
             const std::string& configurationFilePath,
             size_t blocksize,
             uint64_t max_val,
             uint8_t word_size
);

}  // namespace gabacify

#endif  // GABACIFY_ANALYZE_H_
