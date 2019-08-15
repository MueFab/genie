#ifndef SPRING_ID_TOKENIZATION_H_
#define SPRING_ID_TOKENIZATION_H_

#include <string>
#include <vector>
#include <genie/stream-saver.h>

namespace spring {

    void generate_id_tokens(char *prev_ID, uint32_t *prev_tokens_ptr, std::string &current_id,
                            std::vector<int64_t> tokens[128][8]);

    std::string decode_id_tokens(std::string &prev_ID, uint32_t *prev_tokens_ptr, uint32_t *prev_tokens_len,
                                 const std::vector<int64_t> tokens[128][8], uint32_t pos_in_tokens_array[128][8]);


    void read_read_id_tokens_from_file(const std::string &infile_name, std::vector<int64_t> tokens[128][8],
                                       dsg::StreamSaver *ld);

}  // namespace spring

#endif  // SPRING_ID_TOKENIZATION_H_
