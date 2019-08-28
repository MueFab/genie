import ctypes as ct
import typing as t

from .c_api import libgabac
from .c_api import gabac_data_block, gabac_io_config, gabac_stream
from .const import GABAC_RETURN

class GabacDataBlock(object):

    def __init__(
        self,
        word_size:int,
        data:t.Sequence[t.Any]=None,
    ):

        self.word_size = word_size
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
        return_code = libgabac.gabac_data_block_init(
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

    def swap(
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

class GabacIoConfig(object):

    def __init__(
        self,
    ):

        self._io_config = gabac_io_config()

    def create_log_stream(
        self,
        attr,
        source:GabacDataBlock=None,
    ):

        if not attr in ("input", "output", "log"):
            raise ValueError("Invalid attribute :{}".format(attr))

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
