#ifndef GENIE_REFERENCE_ENCODER_PLAINC_H
#define GENIE_REFERENCE_ENCODER_PLAINC_H

#include <stdint.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    uint32_t cr_buf_max_size; /* Maximum sequence buffer size in bytes */
    uint32_t crBufSize; /* Current sequence buffer size */
    char **crBuf; /* Sequence buffer */
    uint32_t crBufNumReads; /* Number of sequences in buffer */

    /* The following variables are auxiliary and not explicitly specified */
    uint32_t crBufNumReadsCapacity; /* Maximum number of sequences before buffers have to be reallocated */
    uint32_t *crBufReadMappingPos; /* Sequence starting positions */
    uint32_t *crBufReadLen; /* Lengths of all sequences regarding the reference mapping */
} LOCAL_ASSEMBLY_STATE;

/**
 * Allocate new local assembly state
 * @param state
 * @return Error code (0 in case of success)
 */
int local_assembly_state_create(LOCAL_ASSEMBLY_STATE **state);

/**
 * Initialize state for use
 * @param state
 * @param _cr_buf_max_size
 * @param initial_capacity Capacity of # reads. Will automatically extend if not enough
 * @return Error code (0 in case of success)
 */
int local_assembly_state_init(LOCAL_ASSEMBLY_STATE *state, uint32_t _cr_buf_max_size, uint32_t initial_capacity);

/**
 * Free all memory allocated by state
 * @param state
 * @return Error code (0 in case of success)
 */
int local_assembly_state_destroy(LOCAL_ASSEMBLY_STATE *state);

/**
 * Add new segment to the assembling window
 * @param state
 * @param seq Sequence of segment. Must be null terminated string
 * @param cigar CIGAR mapping string Must be nul terminated
 * @param pos Mapping position
 * @return Error code (0 in case of success)
 */
int local_assembly_state_add_read(LOCAL_ASSEMBLY_STATE *state, const char *seq, const char *cigar, uint32_t pos);

/**
 * Generate reference with available data in specified area
 * @param state
 * @param startpos First base position of area
 * @param len Length of area
 * @param out Where to put generated reference. This string is _not_ null terminated and of size len. Must be manually freed.
 * @return Error code (0 in case of success)
 */
int local_assembly_state_get_ref(LOCAL_ASSEMBLY_STATE *state, uint32_t startpos, uint32_t len, char** out);

/**
 * Print current pileup in assembly window to the screen (Debugging)
 * @param state
 * @return
 */
int local_assembly_state_print_window(LOCAL_ASSEMBLY_STATE *state);

#endif /* GENIE_REFERENCE_ENCODER_PLAINC_H */
