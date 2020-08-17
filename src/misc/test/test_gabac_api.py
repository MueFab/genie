# from new_gabac import gabac_execute_transform
import ctypes as ct
import unittest

from ..gabac_api import (
    libgabac,
    libc,
    gabac_data_block,
    gabac_io_config,
    GABAC_BINARIZATION,
    GABAC_CONTEXT_SELECT,
    GABAC_OPERATION,
    GABAC_RETURN,
    GABAC_TRANSFORM,
    array,
    print_block,
    get_block_values,
    run_gabac
)


class PythonApiTest(unittest.TestCase):
    DEBUG_LEVEL = 1

    input_data1 = array(
        ct.c_char,
        (
            b"\x01\x00\x00\x00"
            b"\x02\x00\x00\x00"
            b"\x03\x00\x00\x00"
            b"\x04\x00\x00\x00"
            b"\x05\x00\x00\x00"
            b"\x06\x00\x00\x00"
            b"\x07\x00\x00\x00"
            b"\x08\x00\x00\x00"
            b"\x09\x00\x00\x00"
            b"\x0a\x00\x00\x00"
        )
    )

    input_data2 = array(
        ct.c_char,
        (
            b"\x01\x00\x00\x00"
            b"\x01\x00\x00\x00"
            b"\x01\x00\x00\x00"
            b"\x03\x00\x00\x00"
            b"\x03\x00\x00\x00"
            b"\x03\x00\x00\x00"
            b"\x04\x00\x00\x00"
            b"\x04\x00\x00\x00"
            b"\x04\x00\x00\x00"
            b"\x05\x00\x00\x00"
        )
    )

    config_json_py = {
        "word_size": 1,
        "sequence_transformation_id": 3,
        "sequence_transformation_parameter": 2,
        "transformed_sequences": [
            {
                "lut_transformation_enabled": True,
                "lut_transformation_parameter": 0,
                "diff_coding_enabled": False,
                "binarization_id": 2,
                "binarization_parameters": [],
                "context_selection_id": 2
            },
            {
                "lut_transformation_enabled": False,
                "lut_transformation_parameter": 0,
                "diff_coding_enabled": True,
                "binarization_id": 3,
                "binarization_parameters": [],
                "context_selection_id": 2
            }
        ]
    }

    # config_json_raw = json.dumps(config_json_py).encode(
    #     'utf-8'
    # )
    # config_json_ptr = ct.c_char_p(
    #     config_json_raw
    # )

    config_json_ptr = array(
        ct.c_char,
        (
            b"{"
            b"\"word_size\": 1,"
            b"\"sequence_transformation_id\": 0,"
            b"\"sequence_transformation_parameter\": 0,"
            b"\"transformed_sequences\":"
            b"["
            b"{"
            b"\"lut_transformation_enabled\": false,"
            b"\"diff_coding_enabled\": false,"
            b"\"binarization_id\": 0,"
            b"\"binarization_parameters\":"
            b"["
            b"8"
            b"],"
            b"\"context_selection_id\": 1"
            b"}"
            b"]"
            b"}"
        )
    )

    def test_api(self):

        print('Test transformation')
        self.assertEqual(GABAC_RETURN.SUCCESS, self._example_transformations(self.input_data1))
        self.assertEqual(GABAC_RETURN.SUCCESS, self._example_transformations(self.input_data2))
        # print('Test run')
        # self.assertEqual(GABAC_RETURN.SUCCESS, self._example_run())
        print('Test run')
        return_values = run_gabac(self.input_data1, self.config_json_py, ena_roundtrip=True)
        self.assertEqual(GABAC_RETURN.SUCCESS, return_values[0])

    def _example_transformations(self, input_data):
        blocks = array(gabac_data_block, 2)
        parameters_RLE = array(ct.c_uint64, [255])
        parameters_CABAC = array(
            ct.c_uint64,
            [
                GABAC_BINARIZATION.TU,
                2,
                GABAC_CONTEXT_SELECT.ADAPTIVE_ORDER_0,
                ct.sizeof(ct.c_int)
            ]
        )

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Init blocks...\n")

        # Allocate data block with 4 byte block size and the appriate length. 
        # The example data is copied 
        if libgabac.gabac_data_block_init(
                ct.byref(blocks[0]),
                input_data,
                ct.sizeof(input_data) // ct.sizeof(ct.c_int),
                ct.sizeof(ct.c_int)
        ):
            libc.printf(b"Block 0 init failed!\n")
            return -1

        # Allocate an empty data block (will be used to contain the second stream of RLE-Coding)
        if libgabac.gabac_data_block_init(
                ct.byref(blocks[1]),
                None,
                0,
                ct.sizeof(ct.c_uint8)
        ):
            libgabac.gabac_data_block_release(blocks[0])
            libc.printf(b"Block 1 init failed!")
            return -1

        # Addition to c_example
        original_values = [get_block_values(block) for block in blocks]

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"Block 0:\n")
            print_block(blocks[0])
            libc.printf(b"Block 1:\n")
            print_block(blocks[1])

        # /********************************************/
        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Executing RLE-Coding!\n")

        # Execute the actual RLE transformation. 
        # blocks[0] and blocks[1] will now contain the transformed streams
        if libgabac.gabac_execute_transform(
                GABAC_TRANSFORM.RLE,
                parameters_RLE,
                # GABAC_TRANSFORM.EQUALITY,
                # array(ct.c_uint64, []),
                GABAC_OPERATION.ENCODE,
                blocks
        ) == GABAC_RETURN.FAILURE:
            libc.printf(b"RLE transform failed!\n")
            libgabac.gabac_data_block_release(blocks[0])
            libgabac.gabac_data_block_release(blocks[1])
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(blocks[0])
            print_block(blocks[1])

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Executing Diff-Coding on block 0!\n")
        if libgabac.gabac_execute_transform(
                GABAC_TRANSFORM.DIFF,
                None,
                GABAC_OPERATION.ENCODE,
                blocks
        ):
            libc.printf(b"Diff core failed!\n")
            libgabac.gabac_data_block_release(blocks[0])
            libgabac.gabac_data_block_release(blocks[1])
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(blocks[0])

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Executing CABAC-Coding on block 1!\n")
        if libgabac.gabac_execute_transform(
                GABAC_TRANSFORM.CABAC,
                parameters_CABAC,
                GABAC_OPERATION.ENCODE,
                ct.byref(blocks[1])
        ):
            libc.printf(b"Cabac core failed!\n")
            libgabac.gabac_data_block_release(blocks[0])
            libgabac.gabac_data_block_release(blocks[1])
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(blocks[1])

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Executing CABAC-Decoding on block 1!\n")
        if libgabac.gabac_execute_transform(
                GABAC_TRANSFORM.CABAC,
                parameters_CABAC,
                GABAC_OPERATION.DECODE,
                ct.byref(blocks[1])
        ):
            libc.printf(b"Cabac decoding failed!\n")
            libgabac.gabac_data_block_release(blocks[0])
            libgabac.gabac_data_block_release(blocks[1])
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(blocks[1])

        # Diff-Decoding on block 0
        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Executing Diff-Decoding on block 0!\n")
        if libgabac.gabac_execute_transform(
                GABAC_TRANSFORM.DIFF,
                None,
                GABAC_OPERATION.DECODE,
                blocks
        ):
            libc.printf(b"Inverse diff transform failed!\n")
            libgabac.gabac_data_block_release(blocks[0])
            libgabac.gabac_data_block_release(blocks[1])
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(blocks[0])

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Executing RLE-Decoding!\n")
        # After this last decoding step you should retrieve the raw example data again
        if libgabac.gabac_execute_transform(
                GABAC_TRANSFORM.RLE,
                parameters_RLE,
                GABAC_OPERATION.DECODE,
                blocks
        ):
            libc.printf(b"Inverse diff core failed!\n")
            libgabac.gabac_data_block_release(blocks[0])
            libgabac.gabac_data_block_release(blocks[1])
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(blocks[0])
            print_block(blocks[1])

        enc_dec_values = [get_block_values(block) for block in blocks]

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"***Releasing blocks...\n")
        libgabac.gabac_data_block_release(blocks[0])
        libgabac.gabac_data_block_release(blocks[1])

        if enc_dec_values[0] == original_values[0]:
            return GABAC_RETURN.SUCCESS
        else:
            return GABAC_RETURN.FAILURE

    def _example_run(self):
        # Init IO configuration
        io_config = gabac_io_config()

        # Data block for input stream buffer
        in_block = gabac_data_block()

        logfilename = array(ct.c_char, "log.txt")

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"--> Test full run\n")
            libc.printf(b"*** Init streams...\n")

        # We will let gabac compress its own configuration.
        # Notice that offset -1 is to cut of the \0 at the end of the stream
        if libgabac.gabac_data_block_init(
                in_block,
                ### With cchar
                self.config_json_ptr,
                ct.sizeof(self.config_json_ptr),
                ct.sizeof(ct.c_char)
                # ### With raw
                # self.config_json_raw,
                # len(self.config_json_raw),
                # ct.sizeof(ct.c_char),
        ):
            libc.printf(b"*** Could not allocate buffer!\n")
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            print_block(in_block)
        original_values = get_block_values(in_block)

        # Swap newly created input data block into a newly created input stream
        if libgabac.gabac_stream_create_buffer(
                io_config.input,
                in_block
        ):
            libc.printf(b"*** Could not allocate in stream!\n")
            libgabac.gabac_data_block_release(in_block)
            return GABAC_RETURN.FAILURE

        # Create empty output stream
        if libgabac.gabac_stream_create_buffer(
                io_config.output,
                None
        ):
            libc.printf(b"*** Could not allocate out stream!\n")
            libgabac.gabac_stream_release(io_config.input)
            return GABAC_RETURN.FAILURE

        # Create log stream from file. You could also pass stdout instead.
        if libgabac.gabac_stream_create_file(
                io_config.log,
                logfilename,
                len(logfilename),
                1
        ):
            libc.printf(b"*** Could not allocate log stream!\n")
            libgabac.gabac_stream_release(io_config.input)
            libgabac.gabac_stream_release(io_config.output)
            return GABAC_RETURN.FAILURE

        if self.DEBUG_LEVEL > 1:
            libc.printf(b"*** Run gabac encode...\n")

        # Encode using config
        if libgabac.gabac_run(
                GABAC_OPERATION.ENCODE,
                io_config,
                ### With cchar
                self.config_json_ptr,
                len(self.config_json_ptr),
                # ### With ptr
                # self.config_json_raw,
                # len(self.config_json_raw)-1,
        ):
            libc.printf(b"*** Gabac encode failed!\n")
            libgabac.gabac_data_block_release(in_block)
            libgabac.gabac_stream_release(io_config.input)
            libgabac.gabac_stream_release(io_config.output)
            libgabac.gabac_stream_release(io_config.log)
            return GABAC_RETURN.FAILURE

        # Swap contents of output stream back into input stream to prepare decoding
        libgabac.gabac_stream_swap_block(io_config.output, in_block)
        if self.DEBUG_LEVEL > 1:
            print_block(in_block)
        libgabac.gabac_stream_swap_block(io_config.input, in_block)

        # Decode
        if self.DEBUG_LEVEL > 1:
            libc.printf(b"*** Run gabac decode...\n")
        if libgabac.gabac_run(
                GABAC_OPERATION.DECODE,
                io_config,
                ### With cchar
                self.config_json_ptr,
                len(self.config_json_ptr),
        ):
            libc.printf(b"*** Gabac decode failed!\n")
            libgabac.gabac_data_block_release(in_block)
            libgabac.gabac_stream_release(io_config.input)
            libgabac.gabac_stream_release(io_config.output)
            libgabac.gabac_stream_release(io_config.log)
            return GABAC_RETURN.FAILURE

        # Retrieve results, you should end up with your input data again
        libgabac.gabac_stream_swap_block(io_config.output, in_block)
        if self.DEBUG_LEVEL > 1:
            print_block(in_block)

        enc_dec_values = get_block_values(in_block)

        # Free all ressources
        if self.DEBUG_LEVEL > 1:
            libc.printf(b"*** Release...\n")
        libgabac.gabac_data_block_release(in_block)
        libgabac.gabac_stream_release(io_config.input)
        libgabac.gabac_stream_release(io_config.output)
        libgabac.gabac_stream_release(io_config.log)

        if enc_dec_values == original_values:
            return GABAC_RETURN.SUCCESS
        else:
            return GABAC_RETURN.FAILURE


if __name__ == '__main__':
    unittest.main()
