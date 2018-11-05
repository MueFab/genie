#include "descriptors/spring/id_tokenization.h"
#include "descriptors/spring/util.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>

namespace spring {

void generate_id_tokens (char *prev_ID, uint32_t *prev_tokens_ptr, std::string &current_id, std::vector<int64_t> tokens[128][8], bool dont_write_to_vector) {
  uint8_t token_len = 0, match_len = 0;
  uint32_t i = 0, k = 0, tmp = 0, token_ctr = 0, digit_value = 0,
           prev_digit = 0;
  int delta = 0;

  char *id_ptr = &current_id[0];
  char *id_ptr_tok = NULL;

  while (*id_ptr != 0) {
    match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr] + token_len]),
        token_len++;
    id_ptr_tok = id_ptr + 1;

    // Check if the token is a alphabetic word
    if (isalpha(*id_ptr)) {
      while (isalpha(*id_ptr_tok)) {
        // compare with the same token from previous ID
        match_len +=
            (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]),
            token_len++, id_ptr_tok++;
      }
      if (!dont_write_to_vector) {
        if (match_len == token_len &&
            !isalpha(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
          // The token is the same as last ID
          // Encode a token_type ID_MATCH
          tokens[token_ctr+1][0].push_back(8); // MATCH

        } else {
          tokens[token_ctr+1][0].push_back(2); // STRING
          for (k = 0; k < token_len; k++) {
            tokens[token_ctr+1][2].push_back((int64_t)(*(id_ptr + k)));
          }
          tokens[token_ctr+1][2].push_back((int64_t)'\0');
        }
      }
    }
    // check if the token is a run of zeros
    else if (*id_ptr == '0') {
      while (*id_ptr_tok == '0') {
        // compare with the same token from previous ID
        match_len += ('0' == prev_ID[prev_tokens_ptr[token_ctr] + token_len]),
            token_len++, id_ptr_tok++;
      }
      if (!dont_write_to_vector) {
        if (match_len == token_len &&
            prev_ID[prev_tokens_ptr[token_ctr] + token_len] != '0') {
          // The token is the same as last ID
          // Encode a token_type ID_MATCH
          tokens[token_ctr+1][0].push_back(8); // MATCH
        } else {
            tokens[token_ctr+1][0].push_back(2); // STRING
            for (k = 0; k < token_len; k++) {
              tokens[token_ctr+1][2].push_back((int64_t)'0');
            }
            tokens[token_ctr+1][2].push_back((int64_t)'\0');
        }
      }
    }
    // Check if the token is a number smaller than (1<<29)
    else if (isdigit(*id_ptr)) {
      digit_value = (*id_ptr - '0');
      if (*prev_ID != 0) {
        prev_digit = prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] - '0';
      }

      if (*prev_ID != 0) {
        tmp = 1;
        while (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + tmp])) {
          prev_digit = prev_digit * 10 +
                       (prev_ID[prev_tokens_ptr[token_ctr] + tmp] - '0');
          tmp++;
        }
      }

      while (isdigit(*id_ptr_tok) && digit_value < (1 << 29)) {
        digit_value = digit_value * 10 + (*id_ptr_tok - '0');
        // if (*prev_ID != 0){
        //    prev_digit = prev_digit * 10 + (prev_ID[prev_tokens_ptr[token_ctr]
        //    + token_len] - '0');
        //}
        // compare with the same token from previous ID
        match_len +=
            (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]),
            token_len++, id_ptr_tok++;
      }
      if (!dont_write_to_vector) {
        if (match_len == token_len &&
            !isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
          // The token is the same as last ID
          // Encode a token_type ID_MATCH
          tokens[token_ctr+1][0].push_back(8); // MATCH

        } else if (*prev_ID != 0 && (delta = (digit_value - prev_digit)) < 256 && delta > 0) {
          // prev_ID condition to make sure that DDELTA is not used first time token appears
          tokens[token_ctr+1][0].push_back(5); // DDELTA
          tokens[token_ctr+1][5].push_back((int64_t)delta);

        } else {
          // Encode a token type ID_DIGIT and the value (byte-based)
          tokens[token_ctr+1][0].push_back(4); // DIGITS
          tokens[token_ctr+1][4].push_back((int64_t)digit_value);
        }
      }
    } else {
      // compare with the same token from previous ID
      // match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr]]);
      if(!dont_write_to_vector) {
        if (match_len == token_len) {
          // The token is the same as last ID
          // Encode a token_type ID_MATCH
          tokens[token_ctr+1][0].push_back(8); // MATCH

        } else {
          // Encode a token type ID_CHAR and the char
          tokens[token_ctr+1][0].push_back(3); // CHAR
          tokens[token_ctr+1][3].push_back((int64_t)(*id_ptr));
        }
      }
    }

    prev_tokens_ptr[token_ctr] = i;
    i += token_len;
    id_ptr = id_ptr_tok;
    match_len = 0;
    token_len = 0;
    token_ctr++;
    if(token_ctr > 126)
      throw std::runtime_error("Too many tokens in ID");
  }
  strcpy(prev_ID, current_id.c_str());
  if (!dont_write_to_vector)
    tokens[token_ctr+1][0].push_back(9); // END
}

std::string decode_id_tokens (std::string &prev_ID, uint32_t *prev_tokens_ptr, uint32_t *prev_tokens_len, const std::vector<int64_t> tokens[128][8], uint32_t pos_in_tokens_array[128][8]) {
  // works only for id expressed in terms of previous ID (does not store tokens for older ids)
  // supports tokentypes DUP, DIFF, STRING, CHAR, DIGITS, DDELTA, MATCH, END
  std::string current_ID;
  int t = 0;
  int64_t tokType = tokens[t][0][pos_in_tokens_array[t][0]++];
  if (tokType != 0 && tokType != 1) // not DUP or DIFF
    throw std::runtime_error("Invalid first tokentype encountered");
  int64_t distance = tokens[t][tokType][pos_in_tokens_array[t][tokType]++];
  if (distance != 0 && distance != 1)
    throw std::runtime_error("Invalid distance encountered"); 
  if (tokType == 0 && distance == 0)
    throw std::runtime_error("Invalid distance encountered (0 distance for DUP)"); 
  if (tokType == 0) {
    // DUP - copy prev_ID to current_ID and keep prev_ID and prev_tokens_ptr as they are
    return std::string(prev_ID);
  }
  else {
    // DIFF
    bool end_token_found = false;
    for (t = 1; t < 128; t++) {
      // 128 is the maximum number of tokens
      tokType = tokens[t][0][pos_in_tokens_array[t][0]++];
      std::string str_token;
      if (tokType == 9) {
        end_token_found = true;
        break; 
      } else if (tokType == 2) {
        // STRING
        while ((char)tokens[t][tokType][pos_in_tokens_array[t][tokType]] != '\0')
          str_token.push_back((char)tokens[t][tokType][pos_in_tokens_array[t][tokType]++]);
        pos_in_tokens_array[t][tokType]++;
      } else if (tokType == 3) {
        // CHAR
        str_token.push_back((char)tokens[t][tokType][pos_in_tokens_array[t][tokType]++]);
      } else if (tokType == 4) {
        // DIGITS
        str_token = std::to_string(tokens[t][tokType][pos_in_tokens_array[t][tokType]++]);
      } else if (tokType == 5) {
        // DDELTA
        str_token = std::to_string(tokens[t][tokType][pos_in_tokens_array[t][tokType]++] + std::stoul(prev_ID.substr(prev_tokens_ptr[t], prev_tokens_len[t]), nullptr, 10));
      } else if (tokType == 8) {
        // MATCH
        str_token = prev_ID.substr(prev_tokens_ptr[t], prev_tokens_len[t]);
      }
      prev_tokens_ptr[t] = current_ID.size();
      prev_tokens_len[t] = str_token.size();
      current_ID.append(str_token);
    }
    if (end_token_found == false)
      throw std::runtime_error("Didn't find END token within 128 tokens.");
    prev_ID = current_ID;
    return current_ID;
  }
}

void write_read_id_tokens_to_file(const std::string &outfile_name, const std::vector<int64_t> tokens[128][8]) {
  for (int i = 0; i < 128; i++) {
    for (int j = 0; j < 8; j++) {
      if (!tokens[i][j].empty()) {
        std::string outfile_name_i_j = outfile_name + "." + std::to_string(i) + "." + std::to_string(j);
        write_vector_to_file(tokens[i][j], outfile_name_i_j);
      }
    }
  }
}

void read_read_id_tokens_from_file(const std::string &infile_name, std::vector<int64_t> tokens[128][8]) {
  for (int i = 0; i < 128; i++) {
    for (int j = 0; j < 8; j++) {
        std::string infile_name_i_j = infile_name + "." + std::to_string(i) + "." + std::to_string(j);
        tokens[i][j] = read_vector_from_file(infile_name_i_j);
    }
  }
}

}  // namespace spring
