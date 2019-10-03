#ifndef GENIE_REFERENCE_ENCODER_PLAINC_H
#define GENIE_REFERENCE_ENCODER_PLAINC_H

#include <stdint.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    uint32_t cr_buf_max_size;
    uint32_t crBufSize;
    char **sequences;
    uint32_t num_sequences;
    uint32_t num_sequences_capacity;
    uint32_t *sequence_positions;
    uint32_t *sequence_lengths;
} LOCAL_ASSEMBLY_STATE;

int local_assembly_state_create(LOCAL_ASSEMBLY_STATE **state) {
    *state = (LOCAL_ASSEMBLY_STATE *) calloc(1, sizeof(LOCAL_ASSEMBLY_STATE));
    if(!*state){
        return -1;
    }
    return 0;
}

int local_assembly_state_init(LOCAL_ASSEMBLY_STATE *state, uint32_t _cr_buf_max_size, uint32_t initial_capacity) {
    state->cr_buf_max_size = _cr_buf_max_size;
    state->crBufSize = 0;
    state->sequences = (char **) calloc(initial_capacity, sizeof(char *));
    if(!state->sequences){
        return -1;
    }
    state->num_sequences = 0;
    state->num_sequences_capacity = initial_capacity;
    state->sequence_positions = (uint32_t *) calloc(initial_capacity, sizeof(uint32_t));
    if(!state->sequence_positions){
        return -1;
    }
    state->sequence_lengths = (uint32_t *) calloc(initial_capacity, sizeof(uint32_t));;
    if(!state->sequence_lengths){
        return -1;
    }
    return 0;
}

int local_assembly_state_destroy(LOCAL_ASSEMBLY_STATE *state) {
    for (uint32_t i = 0; i < state->num_sequences; ++i) {
        free(state->sequences[i]);
    }
    free(state->sequences);
    free(state->sequence_positions);
    free(state->sequence_lengths);
    return 0;
}

int local_assembly_state_preprocess(const char *seq, const char *cigar, char **output_seq, uint32_t *output_length) {
    *output_length = 0;
    uint32_t cigar_len = strlen(cigar);
    uint32_t seq_len = strlen(seq);
    uint32_t count = 0;

    for (uint32_t cigar_pos = 0; cigar_pos < cigar_len; ++cigar_pos) {
        if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
            count *= 10;
            count += cigar[cigar_pos] - '0';
            continue;
        }
        switch (cigar[cigar_pos]) {
            case 'M':
            case '=':
            case 'X':
            case 'D':
                *output_length += count;
                break;

            case 'I':
            case 'S':
            case 'P':
            case 'N':
            case 'H':
                break;
            default:
                return -1;
        }
        count = 0;
    }

    *output_seq = (char*)calloc(*output_length, sizeof(char));
    if(!*output_seq) {
        return -1;
    }

    uint32_t read_pos = 0;
    uint32_t out_pos = 0;

    for (size_t cigar_pos = 0; cigar_pos < cigar_len; ++cigar_pos) {
        if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
            count *= 10;
            count += cigar[cigar_pos] - '0';
            continue;
        }
        switch (cigar[cigar_pos]) {
            case 'M':
            case '=':
            case 'X':
                for (size_t i = 0; i < count; ++i) {
                    if (read_pos >= seq_len || out_pos >= *output_length) {
                        return -1;
                    }
                    (*output_seq)[out_pos++] = seq[read_pos++];
                }
                break;

            case 'I':
            case 'S':
            case 'P':
                read_pos += count;
                break;
            case 'N':
            case 'D':
            case 'H':
                for (size_t i = 0; i < count; ++i) {
                    if (out_pos >= *output_length) {
                        return -1;
                    }
                    (*output_seq)[out_pos++] = '0';
                }
                break;
            default:
                return -1;
        }
        count = 0;
    }

    if (read_pos != seq_len) {
        return -1;
    }
    return 0;
}

int local_assembly_state_remove_oldest_read(LOCAL_ASSEMBLY_STATE *state) {
    if (state->num_sequences == 0) {
        return -1;
    }
    // Erase oldest read
    state->crBufSize -= state->sequence_lengths[0];
    free(state->sequences[0]);
    state->num_sequences--;
    for (int i = 0; i < state->num_sequences; ++i) {
        state->sequences[i] = state->sequences[i + 1];
        state->sequence_lengths[i] = state->sequence_lengths[i + 1];
        state->sequence_positions[i] = state->sequence_positions[i + 1];
    }
    return 0;
}

int local_assembly_state_min_size(LOCAL_ASSEMBLY_STATE *state, uint32_t minimalSize) {
    if (state->num_sequences_capacity >= minimalSize) {
        return 0;
    }
    state->num_sequences_capacity *= 2;
    state->sequences = (char** )realloc(state->sequences, state->num_sequences_capacity * sizeof(char*));
    if(!state->sequences){
        return -1;
    }
    state->sequence_positions = (uint32_t*) realloc(state->sequence_positions, state->num_sequences_capacity * sizeof(uint32_t));
    if(!state->sequence_positions){
        return -1;
    }
    state->sequence_lengths = (uint32_t*) realloc(state->sequence_lengths, state->num_sequences_capacity * sizeof(uint32_t));
    if(!state->sequence_lengths){
        return -1;
    }
    return 0;
}

int local_assembly_state_add_read(LOCAL_ASSEMBLY_STATE *state, const char *seq, const char *cigar, uint32_t pos) {
    char *processed_read = 0;
    uint32_t processed_size = 0;

    if(local_assembly_state_preprocess(seq, cigar, &processed_read, &processed_size)) {
        return -1;
    }

    while (state->crBufSize + processed_size > state->cr_buf_max_size) {
        if(local_assembly_state_remove_oldest_read(state)) {
            return -1;
        }
    }

    if(local_assembly_state_min_size(state, state->num_sequences + 1)) {
        return -1;
    }

    state->sequence_positions[state->num_sequences] = pos;
    state->sequence_lengths[state->num_sequences] = processed_size;
    state->sequences[state->num_sequences] = processed_read;
    state->num_sequences++;
    state->crBufSize += processed_size;
    return 0;
}

int local_assembly_state_vote(LOCAL_ASSEMBLY_STATE *state, uint32_t abs_position, char* max) {

    uint32_t votes[256];
    memset(&votes, 0, sizeof(uint32_t) * 256);

    // Collect all alignments
    for (size_t i = 0; i < state->num_sequences; ++i) {
        int64_t distance = abs_position - state->sequence_positions[i];
        if(distance >= 0 && distance < state->sequence_lengths[i]){
            char c = state->sequences[i][distance];
            if (c != '0') {
                votes[c]++;
            }
        }
    }

    // Find max
    uint16_t max_value = 0;
    for (int i = 0; i < 256; ++i) {
        if (max_value < votes[i]) {
            max_value = votes[i];
            *max = i;
        } else if (max_value == votes[i]) {
            *max = (*max <= i) ? *max : i;
        }
    }
    return 0;
}

int local_assembly_state_get_ref(LOCAL_ASSEMBLY_STATE *state, uint32_t startpos, uint32_t len, char** out) {
    *out = (char*) calloc(len, sizeof(char));
    if(!*out) {
        return -1;
    }
    uint32_t outpos = 0;
    for(size_t i = startpos; i < startpos + len; ++i) {
        if(local_assembly_state_vote(state, i, &(*out)[outpos++])) {
            return -1;
        }
    }
    return 0;
}

int local_assembly_state_get_window_border(LOCAL_ASSEMBLY_STATE *state, uint32_t* border) {
    *border = -1; // Wrapping around to maximum value
    for(int i = 0; i < state->num_sequences; ++i) {
        *border = (state->sequence_positions[i] < *border) ? state->sequence_positions[i] : *border;
    }
    return 0;
}

int local_assembly_state_print_window(LOCAL_ASSEMBLY_STATE *state) {
    uint32_t minPos;
    if(local_assembly_state_get_window_border(state, &minPos)) {
        return -1;
    }

    for (uint32_t i = 0; i < state->num_sequences; ++i) {
        uint64_t totalOffset = state->sequence_positions[i] - minPos;
        for (uint32_t s = 0; s < totalOffset; ++s) {
            printf(".");
        }
        printf("%.*s\n", state->sequence_lengths[i], state->sequences[i]);
    }
    return 0;
}

#endif //GENIE_REFERENCE_ENCODER_PLAINC_H
