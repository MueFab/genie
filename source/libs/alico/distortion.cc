#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "distortion.h"
#include "util.h"

/**
 * Allocates memory for a distortion matrix
 */
struct distortion_t *alloc_distortion_matrix(uint8_t symbols) {
	struct distortion_t *rtn = (struct distortion_t *) calloc(1, sizeof(struct distortion_t));
	rtn->symbols = symbols;
	rtn->distortion = (double *) calloc(symbols*symbols, sizeof(double));
	return rtn;
}

/**
 * Deallocates memory from a distortion matrix
 */
void free_distortion_matrix(struct distortion_t *d) {
	free(d->distortion);
	free(d);
}

/**
 * Public facing method for allocating distortion matrices
 */
struct distortion_t *generate_distortion_matrix(uint8_t symbols, int type) {
	switch (type) {
		case DISTORTION_MANHATTAN:
			return gen_manhattan_distortion(symbols);
		case DISTORTION_MSE:
			return gen_mse_distortion(symbols);
		case DISTORTION_LORENTZ:
			return gen_lorentzian_distortion(symbols);
		default:
			printf("Invalid distortion type %d specified.", type);
			exit(1);
	}
}

/**
 * Generate a distortion matrix according to the Manhattan distance (L1) metric
 */
struct distortion_t *gen_manhattan_distortion(uint8_t symbols) {
	struct distortion_t *rtn = alloc_distortion_matrix(symbols);
	uint8_t x, y;

	for (x = 0; x < symbols; ++x) {
		for (y = 0; y < symbols; ++y) {
			rtn->distortion[x + y*symbols] = abs(x - y);
		}
	}

	return rtn;
}

/**
 * Generates a distortion matrix according to the MSE (L2) metric
 */
struct distortion_t *gen_mse_distortion(uint8_t symbols) {
	struct distortion_t *rtn = alloc_distortion_matrix(symbols);
	uint8_t x, y;

	for (x = 0; x < symbols; ++x) {
		for (y = 0; y < symbols; ++y) {
			rtn->distortion[x + y*symbols] = (x - y)*(x - y);
		}
	}

	return rtn;
}

/**
 * Generates a distortion matrix according to the lorentzian (log-L1) metric
 */
struct distortion_t *gen_lorentzian_distortion(uint8_t symbols) {
	struct distortion_t *rtn = alloc_distortion_matrix(symbols);
	uint8_t x, y;

	for (x = 0; x < symbols; ++x) {
		for (y = 0; y < symbols; ++y) {
			rtn->distortion[x + y*symbols] = log2( 1.0 + (double)(abs((int)(x - y))) );
		}
	}

	return rtn;
}

double compute_distortion(uint32_t x, uint32_t y, uint8_t DIS){
    
    switch (DIS) {
        case DISTORTION_LORENTZ:
            return log2( 1.0 + (double)(abs((int)(x - y))));
        case DISTORTION_MANHATTAN:
            return abs((int)(x - y));
        case DISTORTION_MSE:
            return (x - y)*(x - y);
            
        default:
            printf("DISTORTION NOT RECOGNIZED\n");
            return 0xffffffff;
    }
}

/**
 * Retrieve the distortion for a pair (x, y). Generally x is the true value and
 * y is the reconstructed value. Handles the matrix->linear array indexing
 */
double get_distortion(struct distortion_t *dist, uint8_t x, uint8_t y) {
	return dist->distortion[x + dist->symbols*y];
}
