#include "id-tokenization.h"
#include "params.h"
#include "util.h"

#include <genie/stream-saver.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace spring {

void generate_id_tokens(char *prev_ID, uint32_t *prev_tokens_ptr, std::string &current_id,
                        std::vector<gabac::DataBlock> &tokens) {
    uint8_t token_len = 0, match_len = 0;
    uint32_t i = 0, k = 0, tmp = 0, token_ctr = 0, digit_value = 0, prev_digit = 0;
    int delta = 0;

    char *id_ptr = &current_id[0];
    char *id_ptr_tok = NULL;

    while (*id_ptr != 0) {
        match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++;
        id_ptr_tok = id_ptr + 1;

        // Check if the token is a alphabetic word
        if (isalpha(*id_ptr)) {
            while (isalpha(*id_ptr_tok)) {
                // compare with the same token from previous ID
                match_len += (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++,
                    id_ptr_tok++;
            }
            if (match_len == token_len && !isalpha(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[6 * (token_ctr + 1) + 0].push_back(8);  // MATCH

            } else {
                tokens[6 * (token_ctr + 1) + 0].push_back(2);  // STRING
                for (k = 0; k < token_len; k++) {
                    tokens[6 * (token_ctr + 1) + 2].push_back((uint8_t)(*(id_ptr + k)));
                }
                tokens[6 * (token_ctr + 1) + 2].push_back((uint8_t)'\0');
            }
        }
        // check if the token is a run of zeros
        else if (*id_ptr == '0') {
            while (*id_ptr_tok == '0') {
                // compare with the same token from previous ID
                match_len += ('0' == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++, id_ptr_tok++;
            }
            if (match_len == token_len && prev_ID[prev_tokens_ptr[token_ctr] + token_len] != '0') {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[6 * (token_ctr + 1) + 0].push_back(8);  // MATCH
            } else {
                tokens[6 * (token_ctr + 1) + 0].push_back(2);  // STRING
                for (k = 0; k < token_len; k++) {
                    tokens[6 * (token_ctr + 1) + 2].push_back((uint8_t)'0');
                }
                tokens[6 * (token_ctr + 1) + 2].push_back((uint8_t)'\0');
            }
        }
        // Check if the token is a number smaller than (1<<29)
        else if (isdigit(*id_ptr)) {
            digit_value = (*id_ptr - '0');
            bool prev_token_digit_flag = true;  // true if corresponding token in previous read is a digit
            if (*prev_ID != 0) {
                if (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1]) &&
                    prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] != '0')
                    prev_digit = prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] - '0';
                else
                    prev_token_digit_flag = false;
            }

            if (prev_token_digit_flag && *prev_ID != 0) {
                tmp = 1;
                while (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + tmp]) && prev_digit < (1 << 26)) {
                    prev_digit = prev_digit * 10 + (prev_ID[prev_tokens_ptr[token_ctr] + tmp] - '0');
                    tmp++;
                }
            }

            while (isdigit(*id_ptr_tok) && digit_value < (1 << 26)) {
                digit_value = digit_value * 10 + (*id_ptr_tok - '0');
                // if (*prev_ID != 0){
                //    prev_digit = prev_digit * 10 + (prev_ID[prev_tokens_ptr[token_ctr]
                //    + token_len] - '0');
                //}
                // compare with the same token from previous ID
                match_len += (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++,
                    id_ptr_tok++;
            }
            if (prev_token_digit_flag && match_len == token_len &&
                !isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[6 * (token_ctr + 1) + 0].push_back(8);  // MATCH

            } else if (prev_token_digit_flag && *prev_ID != 0 && (delta = (digit_value - prev_digit)) < 256 &&
                       delta > 0) {
                // prev_ID condition to make sure that DDELTA is not used first time
                // token appears
                tokens[6 * (token_ctr + 1) + 0].push_back(5);  // DDELTA
                tokens[6 * (token_ctr + 1) + 5].push_back((uint8_t)delta);

            } else {
                // Encode a token type ID_DIGIT and the value (byte-based)
                tokens[6 * (token_ctr + 1) + 0].push_back(4);  // DIGITS
                big_endian_push_uint32((uint32_t)digit_value, &tokens[6 * (token_ctr + 1) + 4]);
            }
        } else {
            // compare with the same token from previous ID
            // match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr]]);
            if (match_len == token_len) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[6 * (token_ctr + 1) + 0].push_back(8);  // MATCH

            } else {
                // Encode a token type ID_CHAR and the char
                tokens[6 * (token_ctr + 1) + 0].push_back(3);  // CHAR
                tokens[6 * (token_ctr + 1) + 3].push_back((uint8_t)(*id_ptr));
            }
        }

        prev_tokens_ptr[token_ctr] = i;
        i += token_len;
        id_ptr = id_ptr_tok;
        match_len = 0;
        token_len = 0;
        token_ctr++;
        if (token_ctr > MAX_NUM_TOKENS_ID - 2) throw std::runtime_error("Too many tokens in ID");
    }
    strcpy(prev_ID, current_id.c_str());
    tokens[6 * (token_ctr + 1) + 0].push_back(10);  // END

    // fill rest of prev_tokens_ptr to 0, otherwise we've bug due to garbage
    // values
    for (uint32_t id = token_ctr + 1; id < MAX_NUM_TOKENS_ID; id++) prev_tokens_ptr[id] = 0;
}

void generate_id_tokens(char *prev_ID, uint32_t *prev_tokens_ptr, std::string &current_id,
                        std::vector<int64_t> tokens[128][6]) {
    uint8_t token_len = 0, match_len = 0;
    uint32_t i = 0, k = 0, tmp = 0, token_ctr = 0, digit_value = 0, prev_digit = 0;
    int delta = 0;

    char *id_ptr = &current_id[0];
    char *id_ptr_tok = NULL;

    while (*id_ptr != 0) {
        match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++;
        id_ptr_tok = id_ptr + 1;

        // Check if the token is a alphabetic word
        if (isalpha(*id_ptr)) {
            while (isalpha(*id_ptr_tok)) {
                // compare with the same token from previous ID
                match_len += (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++,
                    id_ptr_tok++;
            }
            if (match_len == token_len && !isalpha(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[token_ctr + 1][0].push_back(8);  // MATCH

            } else {
                tokens[token_ctr + 1][0].push_back(2);  // STRING
                for (k = 0; k < token_len; k++) {
                    tokens[token_ctr + 1][2].push_back((int64_t)(*(id_ptr + k)));
                }
                tokens[token_ctr + 1][2].push_back((int64_t)'\0');
            }
        }
        // check if the token is a run of zeros
        else if (*id_ptr == '0') {
            while (*id_ptr_tok == '0') {
                // compare with the same token from previous ID
                match_len += ('0' == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++, id_ptr_tok++;
            }
            if (match_len == token_len && prev_ID[prev_tokens_ptr[token_ctr] + token_len] != '0') {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[token_ctr + 1][0].push_back(8);  // MATCH
            } else {
                tokens[token_ctr + 1][0].push_back(2);  // STRING
                for (k = 0; k < token_len; k++) {
                    tokens[token_ctr + 1][2].push_back((int64_t)'0');
                }
                tokens[token_ctr + 1][2].push_back((int64_t)'\0');
            }
        }
        // Check if the token is a number smaller than (1<<29)
        else if (isdigit(*id_ptr)) {
            digit_value = (*id_ptr - '0');
            bool prev_token_digit_flag = true;  // true if corresponding token in previous read is a digit
            if (*prev_ID != 0) {
                if (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1]) &&
                    prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] != '0')
                    prev_digit = prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] - '0';
                else
                    prev_token_digit_flag = false;
            }

            if (prev_token_digit_flag && *prev_ID != 0) {
                tmp = 1;
                while (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + tmp]) && prev_digit < (1 << 26)) {
                    prev_digit = prev_digit * 10 + (prev_ID[prev_tokens_ptr[token_ctr] + tmp] - '0');
                    tmp++;
                }
            }

            while (isdigit(*id_ptr_tok) && digit_value < (1 << 26)) {
                digit_value = digit_value * 10 + (*id_ptr_tok - '0');
                // if (*prev_ID != 0){
                //    prev_digit = prev_digit * 10 + (prev_ID[prev_tokens_ptr[token_ctr]
                //    + token_len] - '0');
                //}
                // compare with the same token from previous ID
                match_len += (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++,
                    id_ptr_tok++;
            }
            if (prev_token_digit_flag && match_len == token_len &&
                !isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[token_ctr + 1][0].push_back(8);  // MATCH

            } else if (prev_token_digit_flag && *prev_ID != 0 && (delta = (digit_value - prev_digit)) < 256 &&
                       delta > 0) {
                // prev_ID condition to make sure that DDELTA is not used first time
                // token appears
                tokens[token_ctr + 1][0].push_back(5);  // DDELTA
                tokens[token_ctr + 1][5].push_back((int64_t)delta);

            } else {
                // Encode a token type ID_DIGIT and the value (byte-based)
                tokens[token_ctr + 1][0].push_back(4);  // DIGITS
                tokens[token_ctr + 1][4].push_back((int64_t)digit_value);
            }
        } else {
            // compare with the same token from previous ID
            // match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr]]);
            if (match_len == token_len) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                tokens[token_ctr + 1][0].push_back(8);  // MATCH

            } else {
                // Encode a token type ID_CHAR and the char
                tokens[token_ctr + 1][0].push_back(3);  // CHAR
                tokens[token_ctr + 1][3].push_back((int64_t)(*id_ptr));
            }
        }

        prev_tokens_ptr[token_ctr] = i;
        i += token_len;
        id_ptr = id_ptr_tok;
        match_len = 0;
        token_len = 0;
        token_ctr++;
        if (token_ctr > MAX_NUM_TOKENS_ID - 2) throw std::runtime_error("Too many tokens in ID");
    }
    strcpy(prev_ID, current_id.c_str());
    tokens[token_ctr + 1][0].push_back(10);  // END

    // fill rest of prev_tokens_ptr to 0, otherwise we've bug due to garbage
    // values
    for (uint32_t id = token_ctr + 1; id < MAX_NUM_TOKENS_ID; id++) prev_tokens_ptr[id] = 0;
}

std::string decode_id_tokens(std::string &prev_ID, uint32_t *prev_tokens_ptr, uint32_t *prev_tokens_len,
                             const std::vector<int64_t> tokens[128][6], uint32_t pos_in_tokens_array[128][6]) {
    // works only for id expressed in terms of previous ID (does not store tokens
    // for older ids) supports tokentypes DUP, DIFF, STRING, CHAR, DIGITS, DDELTA,
    // MATCH, END
    std::string current_ID;
    int t = 0;
    int64_t tokType = tokens[t][0][pos_in_tokens_array[t][0]++];
    if (tokType != 0 && tokType != 1)  // not DUP or DIFF
        throw std::runtime_error("Invalid first tokentype encountered");
    int64_t distance = tokens[t][tokType][pos_in_tokens_array[t][tokType]++];
    if (distance != 0 && distance != 1) throw std::runtime_error("Invalid distance encountered");
    if (tokType == 0 && distance == 0) throw std::runtime_error("Invalid distance encountered (0 distance for DUP)");
    if (tokType == 0) {
        // DUP - copy prev_ID to current_ID and keep prev_ID and prev_tokens_ptr as
        // they are
        return std::string(prev_ID);
    } else {
        // DIFF
        bool end_token_found = false;
        for (t = 1; t < 128; t++) {
            // 128 is the maximum number of tokens
            tokType = tokens[t][0][pos_in_tokens_array[t][0]++];
            std::string str_token;
            if (tokType == 10) {
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
                str_token =
                    std::to_string(tokens[t][tokType][pos_in_tokens_array[t][tokType]++] +
                                   std::stoul(prev_ID.substr(prev_tokens_ptr[t], prev_tokens_len[t]), nullptr, 10));
            } else if (tokType == 8) {
                // MATCH
                str_token = prev_ID.substr(prev_tokens_ptr[t], prev_tokens_len[t]);
            }
            prev_tokens_ptr[t] = current_ID.size();
            prev_tokens_len[t] = str_token.size();
            current_ID.append(str_token);
        }
        if (end_token_found == false) throw std::runtime_error("Didn't find END token within 128 tokens.");
        prev_ID = current_ID;
        return current_ID;
    }
}

void big_endian_push_uint32(uint32_t val, gabac::DataBlock *vec) {
    vec->push_back((uint8_t)((val >> 24) & 0xff));
    vec->push_back((uint8_t)((val >> 16) & 0xff));
    vec->push_back((uint8_t)((val >> 8) & 0xff));
    vec->push_back((uint8_t)((val)&0xff));
}

}  // namespace spring
