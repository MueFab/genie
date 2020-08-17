#include "gabac/gabac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

/* Defining some example input data */

const char input_data1[] =
    "\x01\x00\x00\x00"
    "\x02\x00\x00\x00"
    "\x03\x00\x00\x00"
    "\x04\x00\x00\x00"
    "\x05\x00\x00\x00"
    "\x06\x00\x00\x00"
    "\x07\x00\x00\x00"
    "\x08\x00\x00\x00"
    "\x09\x00\x00\x00"
    "\x0a\x00\x00\x00";

const char input_data2[] =
    "\x01\x00\x00\x00"
    "\x01\x00\x00\x00"
    "\x01\x00\x00\x00"
    "\x03\x00\x00\x00"
    "\x03\x00\x00\x00"
    "\x03\x00\x00\x00"
    "\x04\x00\x00\x00"
    "\x04\x00\x00\x00"
    "\x04\x00\x00\x00"
    "\x05\x00\x00\x00";

const char config_json[] =
    "{\n"
    "\"word_size\": 1,\n"
    "\"sequence_transformation_id\": 0,\n"
    "\"sequence_transformation_parameter\": 0,\n"
    "\"transformed_sequences\":\n"
    "[\n"
    "{\n"
    "\"lut_transformation_enabled\": false,\n"
    "\"diff_coding_enabled\": false,\n"
    "\"binarization_id\": 0,\n"
    "\"binarization_parameters\":\n"
    "[\n"
    "8\n"
    "],\n"
    "\"context_selection_id\": 1\n"
    "}\n"
    "]\n"
    "}";

/* Utility just for this example */
void print_block(const gabac_data_block* block) {
    int i;
    for (i = 0; i < block->values_size; ++i) {
        printf("%lu ", gabac_data_block_get(block, i));
    }
    printf("\n");
}

/* Example how to use the transformation interface */
int example_transformations(void) {
    /* For parameter and necessary number of streams consult c_interface.h */
    gabac_data_block blocks[2];
    const uint64_t parameters_RLE[] = {255};
    const uint64_t parameters_CABAC[] = {gabac_binarization_TU, 2, gabac_context_select_ADAPTIVE_ORDER_0, sizeof(int)};

    /********************************************/
    printf("--> Test Transformations\n");
    printf("***Init blocks...\n");

    /* Allocate data block with 4 byte block size and the appriate length. The example data is copied into the block. */
    if (gabac_data_block_init(&blocks[0], input_data2, sizeof(input_data2) / sizeof(int), sizeof(int))) {
        printf("Block 0 init failed!\n");
        return -1;
    }

    /* Allocate an empty data block (will be used to contain the second stream of RLE-Coding) */
    if (gabac_data_block_init(&blocks[1], NULL, 0, sizeof(uint8_t))) {
        gabac_data_block_release(&blocks[0]);
        printf("Block 1 init failed!\n");
        return -1;
    }

    printf("Block 0:\n");
    print_block(&blocks[0]);
    printf("Block 1:\n");
    print_block(&blocks[1]);

    /********************************************/
    printf("***Executing RLE-Coding!\n");

    /* Execute the actual RLE transformation. blocks[0] and blocks[1] will now contain the transformed streams */
    if (gabac_execute_transform(gabac_transform_RLE, parameters_RLE, FALSE, blocks)) {
        printf("RLE transform failed!\n");
        goto ERROR;
    }

    print_block(&blocks[0]);
    print_block(&blocks[1]);

    /********************************************/
    printf("***Executing Diff-Coding on block 0!\n");
    if (gabac_execute_transform(gabac_transform_DIFF, NULL, FALSE, blocks)) {
        printf("Diff core failed!\n");
        goto ERROR;
    }
    print_block(&blocks[0]);

    /********************************************/
    printf("***Executing CABAC-Coding on block 1!\n");
    if (gabac_execute_transform(gabac_transform_CABAC, parameters_CABAC, FALSE, &blocks[1])) {
        printf("Cabac core failed!\n");
        goto ERROR;
    }
    print_block(&blocks[1]);

    /********************************************/
    printf("***Executing CABAC-Decoding on block 1!\n");

    /* Start decoding now */
    if (gabac_execute_transform(gabac_transform_CABAC, parameters_CABAC, TRUE, &blocks[1])) {
        printf("Cabac decoding failed!\n");
        goto ERROR;
    }
    print_block(&blocks[1]);

    /********************************************/
    printf("***Executing Diff-Decoding on block 0!\n");
    if (gabac_execute_transform(gabac_transform_DIFF, NULL, TRUE, blocks)) {
        printf("Inverse diff transform failed!\n");
        goto ERROR;
    }
    print_block(&blocks[0]);

    /********************************************/
    printf("***Executing RLE-Decoding!\n");

    /* After this last decoding step you should retrieve the raw example data again */
    if (gabac_execute_transform(gabac_transform_RLE, parameters_RLE, TRUE, blocks)) {
        printf("Inverse diff core failed!\n");
        goto ERROR;
    }
    print_block(&blocks[0]);
    print_block(&blocks[1]);

    /********************************************/
    printf("***Releasing blocks...\n");

    /* Every allocated ressource should be released again to avoid memory leaks. */
    gabac_data_block_release(&blocks[0]);
    gabac_data_block_release(&blocks[1]);

    return 0;

ERROR:
    gabac_data_block_release(&blocks[0]);
    gabac_data_block_release(&blocks[1]);
    return -1;
}

/* Example how to use the gabac run interface */
int example_run(void) {
    /* Containing io config */
    gabac_io_config io_config;

    /* Data block to feed input stream buffer */
    gabac_data_block in_block;

    const char* logfilename = "log.txt";

    printf("--> Test full run\n");
    printf("*** Init streams...\n");

    /* We will let gabac compress its own configuration. Notice that the -1 is to cut of the \0 at the end of the stream
     */
    if (gabac_data_block_init(&in_block, config_json, sizeof(config_json) - 1, sizeof(uint8_t))) {
        printf("*** Could not allocate buffer!\n");
        return -1;
    }

    print_block(&in_block);

    /* Swap newly created input data block into a newly created input stream */
    if (gabac_stream_create_buffer(&io_config.input, &in_block)) {
        printf("*** Could not allocate in stream!\n");
        gabac_data_block_release(&in_block);
        return -1;
    }

    /* Create empty output stream */
    if (gabac_stream_create_buffer(&io_config.output, NULL)) {
        printf("*** Could not allocate out stream!\n");
        gabac_stream_release(&io_config.input);
        return -1;
    }

    /* Create log stream from file. You could also pass stdout instead. */
    if (gabac_stream_create_file(&io_config.log, logfilename, strlen(logfilename), TRUE)) {
        printf("*** Could not allocate log stream!\n");
        gabac_stream_release(&io_config.input);
        gabac_stream_release(&io_config.output);
        return -1;
    }

    printf("*** Run gabac encode...\n");

    /* Encode using configs! */
    if (gabac_run(gabac_operation_ENCODE, &io_config, config_json, sizeof(config_json) - 1)) {
        printf("*** Gabac encode failed!\n");
        goto ERROR;
    }

    /* Swap contents of output stream back into input stream to prepare decoding */
    gabac_stream_swap_block(&io_config.output, &in_block);
    print_block(&in_block);
    gabac_stream_swap_block(&io_config.input, &in_block);

    printf("*** Run gabac decode...\n");

    /* Decode! */
    if (gabac_run(gabac_operation_DECODE, &io_config, config_json, sizeof(config_json) - 1)) {
        printf("*** Gabac decode failed!\n");
        goto ERROR;
    }

    /* Retrieve results, you should end up with your input data again. */
    gabac_stream_swap_block(&io_config.output, &in_block);
    print_block(&in_block);

    printf("*** Release...\n");

    /* Free all ressources */
    gabac_data_block_release(&in_block);
    gabac_stream_release(&io_config.input);
    gabac_stream_release(&io_config.output);
    gabac_stream_release(&io_config.log);
    return 0;

ERROR:
    gabac_data_block_release(&in_block);
    gabac_stream_release(&io_config.input);
    gabac_stream_release(&io_config.output);
    gabac_stream_release(&io_config.log);
    return -1;
}

int main() {
    if (example_transformations()) {
        return -1;
    }
    if (example_run()) {
        return -1;
    }
    return 0;
}