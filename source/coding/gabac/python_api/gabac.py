import ctypes as ct

from gabac_api import libgabac
from gabac_api import gabac_stream
from gabac_api import gabac_data_block
from gabac_api import gabac_io_config
from gabac_api import GABAC_BINARIZATION, GABAC_CONTEXT_SELECT, GABAC_LOG_LEVEL, GABAC_LOG_LEVEL
from gabac_api import GABAC_OPERATION, GABAC_RETURN, GABAC_STREAM_MODE, GABAC_TRANSFORM

class GabacStream(object):
    def __init__(self,
        write,
        filename="",
    ):
        
        self.stream = gabac_stream()
        
        libgabac.gabac_stream_create_file(
            self.stream,
            filename,
            len(filename),
        )

class GabacDataBlock(object):

    def __init__(self,
        #block,
        size:int,
        word_size:int,
        data=None,
    ):
        # uint8_t wordSize;
        # std::vector<uint8_t> data;

        self.word_size = word_size

        self.data_block = gabac_data_block()

        return_code = libgabac.gabac_data_block_init(
            #self.data_block,
            self.data_block.ctypes.data_as(ct.POINTER(gabac_data_block)),
            data,
            #data.ctypes.data_as(ct.POINTER(ct.char),
            size,
            word_size,
        )

        if return_code == GABAC_RETURN.FAILURE:
            raise("Failed to initialize GabacDataCode")

    def release(self):
        return_code = libgabac.gabac_data_block_init(
            self.data_block,
        )

        if return_code == GABAC_RETURN.FAILURE:
            raise("Failed to release memory of GabacDataCode")

    def __getitem__(self, index):
        return libgabac.gabac_data_block_get(
            self.data_block,
            index
        )

    def __setitem__(self, index, value):
        libgabac.gabac_data_block_set(
            self.data_block,
            index,
            value
        )