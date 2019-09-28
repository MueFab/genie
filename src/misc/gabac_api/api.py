import time
import typing as t
import ctypes as ct

from numpy import Infinity

from .c_api import (
    libgabac, 
    gabac_data_block,
    gabac_io_config, 
    gabac_stream
)
from .utils import json_to_array
from . import GABAC_OPERATION, GABAC_RETURN

class GabacDataBlock(object):

    def __init__(
        self,
        data:t.Sequence[t.Any]=None,
        word_size:int=1,
    ):

        # self.word_size = word_size
        self._data_block = gabac_data_block()

        if data is not None:
            return_code = libgabac.gabac_data_block_init(
                self._data_block,
                data,
                len(data),
                word_size,
            )

            if return_code == GABAC_RETURN.FAILURE:
                raise RuntimeError("Failed to initialize GabacDataBlock")

    def release(
        self
    ):
        return_code = libgabac.gabac_data_block_release(
            self._data_block,
        )

        if return_code == GABAC_RETURN.FAILURE:
            raise RuntimeError("Failed to release memory of GabacDataBlock")

    def resize(
        self, 
        size:int
    ):
        return_code = libgabac.gabac_data_block_resize(
            self._data_block,
            size
        )

        if return_code == GABAC_RETURN.FAILURE:
            raise RuntimeError("Failed to release memory of GabacDataBlock")

    def copy_to(
        self,
        target
    ):

        try:
            _data_block = target._data_block
        except AttributeError:
            _data_block = target

        return_code = libgabac.gabac_data_block_copy(
                _data_block,
                self._data_block
            )

        if return_code == GABAC_RETURN.FAILURE:
            raise RuntimeError("Unable to copy data of GabacDataBlock")

    def __eq__(
        self,
        target
    ):

        try:
            _data_block = target._data_block
        except AttributeError:
            _data_block = target

        if isinstance(target, GabacDataBlock):
            return_code = libgabac.gabac_data_block_equals(
                self._data_block,
                _data_block
            )

        return 1 == return_code

    def __getitem__(
        self, 
        index:int
    ):
        return libgabac.gabac_data_block_get(
            self._data_block,
            index
        )

    def __setitem__(
        self, 
        index:int, 
        value:int
    ):

        libgabac.gabac_data_block_set(
            self._data_block,
            index,
            value
        )

    # def __getattribute__(self, name):
    #     return getattr(self._data_block, name)

class GabacIoConfig(object):

    def __init__(
        self,
    ):

        self._io_config = gabac_io_config()

    def is_attr_valid(
        self,
        attr
    ):
        if not attr in ("input", "output", "log"):
            raise ValueError("Invalid attribute :{}".format(attr))

    def create_log_stream(
        self,
        attr,
        source:GabacDataBlock=None,
    ):

        self.is_attr_valid(attr)

        try:
            _data_block = source._data_block
        except AttributeError:
            _data_block = source
        
        return_code = libgabac.gabac_stream_create_buffer(
            getattr(self._io_config, attr),
            _data_block
        )

        if return_code == GABAC_RETURN.FAILURE:
            raise RuntimeError("Unable to create log stream")

    def release(
        self,
        attr
    ):
        self.is_attr_valid(attr)

        libgabac.gabac_stream_release(
            getattr(self._io_config, attr)
        )

    def len_of(
        self,
        attr
    ):
        raise NotImplementedError('Incomplete implementation')
        self.is_attr_valid(attr)

        stream = getattr(getattr(self._io_config, attr), "data")
        return stream.values_size * stream.word_size


    def release_all(
        self
    ):
        for attr in ("input", "output", "log"):
            libgabac.gabac_stream_release(
                getattr(self._io_config, attr),
            )

    # def __getattribute__(self, name):
    #     return getattr(self._data_block, name)

def is_config_valid(
    config,
    max_val,
    word_size,
):
    if isinstance(config, dict):
        config = json_to_array(config)

    return libgabac.gabac_config_is_general(
        config,
        len(config),
        max_val,
        word_size
    )

def run_gabac(
        data,
        config,
        word_size=1,
        ena_roundtrip=False,
        verbose_level=0,
    ):

        config_cchar = json_to_array(config)

        return_failure = (GABAC_RETURN.FAILURE, Infinity, Infinity)

        if not is_config_valid(config_cchar, int.from_bytes(max(data), 'big'), word_size):
            return return_failure

        try:
            in_block = GabacDataBlock(data, word_size)
        except RuntimeError as e:
            return return_failure

        if ena_roundtrip:
            try:
                copy_in_block = GabacDataBlock(word_size=word_size)
                in_block.copy_to(copy_in_block)
            except RuntimeError as e:
                in_block.release()
                return return_failure

        io_config = GabacIoConfig()
        attrs     = ['input', 'output', 'log']
        attr_datas = [in_block, None, None]

        for attr, attr_data in zip(attrs, attr_datas):
            try:
                io_config.create_log_stream(attr, attr_data)
            except RuntimeError as e:
                in_block.release()
                if ena_roundtrip:
                    copy_in_block.release()
                return return_failure

        # Encode using config
        start_time = time.time()
        if libgabac.gabac_run(
            GABAC_OPERATION.ENCODE,
            io_config._io_config,
            config_cchar,
            len(config_cchar),
        ):
            
            in_block.release()
            io_config.release_all()
            if ena_roundtrip:
                copy_in_block.release()
            
            return return_failure

        encoding_time = time.time() - start_time

        # Swap contents of output stream back into input stream to prepare decoding
        libgabac.gabac_stream_swap_block(io_config._io_config.output, in_block._data_block)
        encoded_length = in_block._data_block.values_size * in_block._data_block.word_size
        # encoded_length = io_config.len_of('output')

        if ena_roundtrip:
            libgabac.gabac_stream_swap_block(io_config._io_config.input, in_block._data_block)

            if libgabac.gabac_run(
                GABAC_OPERATION.DECODE, 
                io_config._io_config,
                config_cchar,
                len(config_cchar),   
            ):
                in_block.release()
                io_config.release_all()
                copy_in_block.release()
                
                return return_failure

            libgabac.gabac_stream_swap_block(io_config._io_config.output, in_block._data_block)

            if in_block == copy_in_block:
                in_block.release()
                io_config.release_all()
                copy_in_block.release()
                
                return GABAC_RETURN.SUCCESS, encoded_length, encoding_time
            else:
                return return_failure
        else:
            in_block.release()
            io_config.release_all()
            return GABAC_RETURN.SUCCESS, encoded_length, encoding_time
