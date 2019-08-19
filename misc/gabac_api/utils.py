import ctypes as ct

from .api import libgabac

libc = ct.CDLL("libc.so.6")

def array(dtype, data):
    if isinstance(data, int):
        arr_dtype = data * dtype
        return arr_dtype()
    elif isinstance(data, (list, tuple, bytes)):
        arr_dtype = dtype * len(data)
        return arr_dtype(*data)
    elif isinstance(data, str):
        arr_dtype = dtype * len(data)
        return arr_dtype(*data.encode())
    elif isinstance(data, dict):
        raise TypeError("Not yet implemented for type dictionary")
    else:
        raise TypeError("Incorrect datatype of data")

def print_array(arr):
    for val in arr:
        if isinstance(val, bytes):
            print("{}".format(val.decode()), end='')
        else:
            print("{}".format(val), end='')
    print()

def print_block(block):
    for i in range(block.values_size):
        # print("{: 2d}".format(libgabac.gabac_data_block_get(ct.byref(block), i)), end='')
        libc.printf(b"%lu ", libgabac.gabac_data_block_get(ct.byref(block), i))
    # print()
    libc.printf(b"\n")

def get_block_values(block):
    values = ""
    for i in range(block.values_size):
        values += "{:02d}".format(libgabac.gabac_data_block_get(ct.byref(block), i))
    return values

def are_blocks_equal(block1, block2):
    if block1.values_size == block2.values_size:
        for i in range(block1.values_size):
            if libgabac.gabac_data_block_get(ct.byref(block1), i) != libgabac.gabac_data_block_get(ct.byref(block2), i):
                print(i)
                return False
        return True
    else:
        return False