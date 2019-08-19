import time
import json
import argparse
import ctypes as ct
import itertools as it
import multiprocessing as mp

import numpy as np

from gabac_api import libgabac
from gabac_api import gabac_stream
from gabac_api import gabac_io_config
from gabac_api import gabac_data_block
from gabac_api import GABAC_BINARIZATION, GABAC_CONTEXT_SELECT, GABAC_LOG_LEVEL, GABAC_LOG_LEVEL
from gabac_api import GABAC_OPERATION, GABAC_RETURN, GABAC_STREAM_MODE, GABAC_TRANSFORM
from gabac_api import array

def transformed_seq_value_generator():
    lut_transforms_noparam = [
        False
    ]

    lut_transforms_wparam = [ 
        True
    ]

    lut_transform_params = [
        0, 
        1
    ]

    diff_codings = [
        False, 
        True
    ]
    
    binarizations_noparam = [
        #GABAC_BINARIZATION.BI, 
        #GABAC_BINARIZATION.TU,
        GABAC_BINARIZATION.SEG,
        GABAC_BINARIZATION.EG,
    ]

    binarizations_wparam = [
        GABAC_BINARIZATION.STEG,
        GABAC_BINARIZATION.TEG
    ]
    binarizations_params = [1, 2, 3, 5, 7, 9, 15, 30, 255]

    context_selections = [
        GABAC_CONTEXT_SELECT.BYPASS,
        GABAC_CONTEXT_SELECT.ADAPTIVE_ORDER_0,
        GABAC_CONTEXT_SELECT.ADAPTIVE_ORDER_1,
        GABAC_CONTEXT_SELECT.ADAPTIVE_ORDER_2,
    ]

    def lut_transform_items():
        for lut_id, lut_param in zip(lut_transforms_noparam, [0]):
            yield lut_id, lut_param
        for lut_id, lut_param in it.product(lut_transforms_wparam, lut_transform_params):
            yield lut_id, lut_param

    def diff_coding_items():
        for diff_coding in diff_codings:
            yield diff_coding

    def binarization_items():
        for binarization_id, binarization_param in it.product(
            binarizations_noparam, 
            [0]
        ):
            yield binarization_id, binarization_param
        for binarization_id, binarization_param in it.product(
            binarizations_wparam,
            binarizations_params
        ):
            yield binarization_id, binarization_param

    def context_selection_items():
        for context_selection_id in context_selections:
            yield context_selection_id

    for lut_id, lut_param in lut_transform_items():
        for diff_coding in diff_coding_items():
            for binarization_id, binarization_param in binarization_items():
                for context_selection_id in context_selection_items():
                    # yield {
                    #     'lut_transformation_enabled'    : lut_id,
                    #     'lut_transformation_parameter'  : lut_param,
                    #     'diff_coding_enabled'           : diff_coding, 
                    #     'binarization_id'               : binarization_id, 
                    #     'binarization_parameters'       : binarization_param,
                    #     'context_selection_id'          : context_selection_id
                    # }

                    yield [
                        lut_id,
                        lut_param,
                        diff_coding, 
                        binarization_id, 
                        binarization_param,
                        context_selection_id
                    ]

def callback_f_none(
    *args
):
    transformed_seq_items, data = args

    lut_id,lut_param,diff_coding, binarization_id, binarization_param,context_selection_id = \
        transformed_seq_items

    transformed_seq_conf = {
        'lut_transformation_enabled'    : lut_id,
        'lut_transformation_parameter'  : lut_param,
        'diff_coding_enabled'           : diff_coding, 
        'binarization_id'               : binarization_id, 
        'binarization_parameters'       : binarization_param,
        'context_selection_id'          : context_selection_id
    }

    conf = {
        "word_size" : 1,
        "sequence_transformation_id" : GABAC_TRANSFORM.NONE,
        "sequence_transformation_parameter" : 0,
        "transformed_sequences" : transformed_seq_conf
    }

    # GABAC_RETURN.SUCCESS, encoded_length, encoding_time

    ret_code, enc_len, enc_time = run_gabac(data, conf)

    return enc_len

def run_gabac(
        data,
        config,
        roundtrip=False
    ):

        config_cchar = array(ct.c_char, json.dumps(config, indent=4).encode('utf-8'))

        io_config = gabac_io_config()
        in_block = gabac_data_block()

        logfilename = array(ct.c_char, "log.txt")

        start_time = time.time()

        if libgabac.gabac_data_block_init(
            in_block,
            data,
            len(data),
            ct.sizeof(ct.c_char)
        ):
            return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        if roundtrip:
            copy_in_block = gabac_data_block()

            if libgabac.gabac_data_block_copy(
                copy_in_block,
                in_block
            ):
                return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        #original_length = in_block.values_size * in_block.word_size

        if libgabac.gabac_stream_create_buffer(
            io_config.input,
            in_block
        ):
            libgabac.gabac_data_block_release(in_block)
            return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        if libgabac.gabac_stream_create_buffer(
            io_config.output, 
            None
        ):
            libgabac.gabac_stream_release(io_config.input)
            return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        # Create log stream from file. You could also pass stdout instead.
        # if libgabac.gabac_stream_create_file(
        #     io_config.log,
        #     logfilename,
        #     len(logfilename),
        #     1
        # ):
        #     libgabac.gabac_stream_release(io_config.input)
        #     libgabac.gabac_stream_release(io_config.output)
        #     return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        # Log to buffer
        if libgabac.gabac_stream_create_buffer(
            io_config.log,
            None
        ):
            libgabac.gabac_stream_release(io_config.input)
            libgabac.gabac_stream_release(io_config.output)
            return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        # if libgabac.gabac_stream_create_buffer(
        #     io_config.log, 
        #     None
        # ):
        #     libgabac.gabac_stream_release(io_config.input)
        #     return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        # Encode using config
        if libgabac.gabac_run(
            GABAC_OPERATION.ENCODE,
            io_config,
            config_cchar,
            len(config_cchar),
        ):
            libgabac.gabac_data_block_release(in_block)
            libgabac.gabac_stream_release(io_config.input)
            libgabac.gabac_stream_release(io_config.output)
            libgabac.gabac_stream_release(io_config.log)
            
            return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

        encoding_time = time.time() - start_time

        # Swap contents of output stream back into input stream to prepare decoding
        libgabac.gabac_stream_swap_block(io_config.output, in_block)
        encoded_length = in_block.values_size * in_block.word_size

        if roundtrip:
            libgabac.gabac_stream_swap_block(io_config.input, in_block)

            if libgabac.gabac_run(
                GABAC_OPERATION.DECODE, 
                io_config,
                config_cchar,
                len(config_cchar),   
            ):
                libgabac.gabac_data_block_release(in_block)
                libgabac.gabac_data_block_release(copy_in_block)
                libgabac.gabac_stream_release(io_config.input)
                libgabac.gabac_stream_release(io_config.output)
                libgabac.gabac_stream_release(io_config.log)

                return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity

            libgabac.gabac_stream_swap_block(io_config.output, in_block)

            if libgabac.gabac_data_block_equals(in_block, copy_in_block):
                libgabac.gabac_data_block_release(in_block)
                libgabac.gabac_data_block_release(copy_in_block)
                libgabac.gabac_stream_release(io_config.input)
                libgabac.gabac_stream_release(io_config.output)
                libgabac.gabac_stream_release(io_config.log)
                return GABAC_RETURN.SUCCESS, encoded_length, encoding_time
            else:
                libgabac.gabac_data_block_release(in_block)
                libgabac.gabac_data_block_release(copy_in_block)
                libgabac.gabac_stream_release(io_config.input)
                libgabac.gabac_stream_release(io_config.output)
                libgabac.gabac_stream_release(io_config.log)
                return GABAC_RETURN.FAILURE, np.Infinity, np.Infinity
        else:
            libgabac.gabac_data_block_release(in_block)
            libgabac.gabac_stream_release(io_config.input)
            libgabac.gabac_stream_release(io_config.output)
            libgabac.gabac_stream_release(io_config.log)
            return GABAC_RETURN.SUCCESS, encoded_length, encoding_time


def main(args):

    with open(args.input, 'rb') as f:
        data = f.read()

    with mp.Pool(processes=args.nproc) as pool:
        res = pool.map(
            callback_f_none, 
            it.product(
                transformed_seq_value_generator(),
                data
            )
        )
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Hyperparameter Search')

    parser.add_argument('-j' , '--nproc',   required=False, default=None, type=int, help='Number of processes')
    parser.add_argument('-i' , '--input',   required=True,  type=str, help='Path to data')
    parser.add_argument('-o' , '--output',  required=True,  type=str, help='Path to result')

    args = parser.parse_args()

    main(args)