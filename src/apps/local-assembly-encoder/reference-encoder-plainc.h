#ifndef GENIE_REFERENCE_ENCODER_PLAINC_H_
#define GENIE_REFERENCE_ENCODER_PLAINC_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t cr_buf_max_size; /* Clause 7.3.2.3; maximum size in bytes of crBuf */
    char **crBuf; /* Buffer for decoded reads */
    uint32_t crBufSize; /* Current size of crBuf in bytes */
    uint32_t crBufNumReads; /* Number of decoded reads in crBuf */

    /* Auxiliary variables */
    uint32_t crBufNumReadsCapacity; /* Maximum number of decoded reads before memory for crBuf must be reallocated */
    uint32_t *crBufReadMappingPos; /* Decoded reads starting positions */
    uint32_t *crBufReadLen; /* Decoded reads lengths */
} LOCAL_ASSEMBLY_STATE;

/**
 * Allocate new local assembly state
 * @param state
 * @return Error code (0 in case of success)
 */
int local_assembly_state_create(LOCAL_ASSEMBLY_STATE **state);

/**
 * Initialize local assembly state
 * @param state
 * @param _cr_buf_max_size
 * @param initial_capacity Initial capacity in # decoded reads
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

#endif /* GENIE_REFERENCE_ENCODER_PLAINC_H_ */
