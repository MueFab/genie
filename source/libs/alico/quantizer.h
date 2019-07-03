#ifndef _QUANTIZER_H_
#define _QUANTIZER_H_

#include <stdint.h>

#include "pmf.h"
#include "distortion.h"
#include "util.h"

#define QUANTIZER_MAX_ITER		100

/**
 * Structure holding information about a quantizer, which just maps input symbols
 * to output symbols for a specific alphabet
 */
struct quantizer_t {
	const struct alphabet_t *__restrict__ alphabet;
	struct alphabet_t *__restrict__ output_alphabet;
	symbol_t *__restrict__ q;
    double ratio;
	double mse;
};

// Memory management
struct quantizer_t *alloc_quantizer(const struct alphabet_t *);
void free_quantizer(struct quantizer_t *);

// Generates a quantizer via optimization
struct quantizer_t *generate_quantizer(struct pmf_t *__restrict__ pmf, struct distortion_t *__restrict__ dist, uint32_t states);

// Calculate the output pmf when the quantizer is applied to the input pmf
struct pmf_t *apply_quantizer(struct quantizer_t *__restrict__ q, struct pmf_t *__restrict__ pmf, struct pmf_t *__restrict__ output);

// Find the output alphabet of a quantizer
void find_output_alphabet(struct quantizer_t *);

// Display/debugging
void print_quantizer(struct quantizer_t *);

#endif
