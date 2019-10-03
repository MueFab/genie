#include "reference-encoder-plainc.h"


/**
 * Remove all insertions and clips from the sequence and account for deletions by inserting '0' into the sequence.
 * @param seq
 * @param cigar
 * @param output_seq
 * @param output_length
 * @return
 */
static int local_assembly_state_preprocess(const char *seq, const char *cigar, char **output_seq, uint32_t *output_length) {
    *output_length = 0;
    uint32_t cigar_len = strlen(cigar);
    uint32_t seq_len = strlen(seq);
    uint32_t count = 0;

    /* Calculate output size */
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

    /* Allocate output */
    *output_seq = (char*)calloc(*output_length, sizeof(char));
    if(!*output_seq) {
        return -1;
    }

    /* Actual preprocessing */
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
                /* Write out regular bases */
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
                /* Skip these types, basically */
                read_pos += count;
                break;
            case 'N':
            case 'D':
            case 'H':
                /* Inject '0' to account for deletions */
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

/**
 * Remove one read to make room for new reads inside the sequence buffer.
 * @param state
 * @return
 */
static int local_assembly_state_remove_oldest_read(LOCAL_ASSEMBLY_STATE *state) {
    if (state->num_sequences == 0) {
        return -1;
    }
    /* Erase oldest read */
    state->crBufSize -= state->sequence_lengths[0];
    free(state->sequences[0]);
    state->num_sequences--;

    /* Shift all remaining reads in the buffer to fill the gap */
    for (int i = 0; i < state->num_sequences; ++i) {
        state->sequences[i] = state->sequences[i + 1];
        state->sequence_lengths[i] = state->sequence_lengths[i + 1];
        state->sequence_positions[i] = state->sequence_positions[i + 1];
    }
    return 0;
}

/**
 * Makes sure that the capacity is at least minimalSize reads. If not the buffers for positions / lengths are reallocated.
 * @param state
 * @param minimalSize
 * @return
 */
static int local_assembly_state_min_size(LOCAL_ASSEMBLY_STATE *state, uint32_t minimalSize) {
    if (state->num_sequences_capacity >= minimalSize) {
        /* Nothing to do */
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

/**
 * Find a consensus base for one single position. Returns \0 if nothing found.
 * @param state
 * @param abs_position
 * @param max
 * @return
 */
static int local_assembly_state_vote(LOCAL_ASSEMBLY_STATE *state, uint32_t abs_position, char* max) {
    const size_t CHAR_RANGE = 256;
    uint32_t votes[CHAR_RANGE];
    memset(&votes, 0, sizeof(uint32_t) * CHAR_RANGE);

    /* Collect all alignments */
    for (size_t i = 0; i < state->num_sequences; ++i) {
        int64_t distance = abs_position - state->sequence_positions[i];
        if(distance >= 0 && distance < state->sequence_lengths[i]){
            char c = state->sequences[i][distance];
            if (c != '0') {
                votes[c]++;
            }
        }
    }

    /* Find max */
    uint16_t max_value = 0;
    for (int i = 0; i < CHAR_RANGE; ++i) {
        if (max_value < votes[i]) {
            max_value = votes[i];
            *max = i;
        } else if (max_value == votes[i]) {
            *max = (*max <= i) ? *max : i;
        }
    }
    return 0;
}

/**
 * Get the position of the leftmost read in the buffer
 * @param state
 * @param border
 * @return
 */
static int local_assembly_state_get_window_border(LOCAL_ASSEMBLY_STATE *state, uint32_t* border) {
    *border = -1; // Wrapping around to maximum value
    for(int i = 0; i < state->num_sequences; ++i) {
        *border = (state->sequence_positions[i] < *border) ? state->sequence_positions[i] : *border;
    }
    return 0;
}


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
    /* Sequences */
    for (uint32_t i = 0; i < state->num_sequences; ++i) {
        free(state->sequences[i]);
    }

    /* Administrative and mapping stuff */
    free(state->sequences);
    free(state->sequence_positions);
    free(state->sequence_lengths);
    return 0;
}

int local_assembly_state_add_read(LOCAL_ASSEMBLY_STATE *state, const char *seq, const char *cigar, uint32_t pos) {
    char *processed_read = 0;
    uint32_t processed_size = 0;

    if(local_assembly_state_preprocess(seq, cigar, &processed_read, &processed_size)) {
        return -1;
    }

    /* Remove reads until enough space available */
    while (state->crBufSize + processed_size > state->cr_buf_max_size) {
        if(local_assembly_state_remove_oldest_read(state)) {
            /* Buffer to small even for this single read */
            return -1;
        }
    }

    /* Make sure there is enough administrative space */
    if(local_assembly_state_min_size(state, state->num_sequences + 1)) {
        return -1;
    }

    /* Copy data */
    state->sequence_positions[state->num_sequences] = pos;
    state->sequence_lengths[state->num_sequences] = processed_size;
    state->sequences[state->num_sequences] = processed_read;
    state->num_sequences++;
    state->crBufSize += processed_size;
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