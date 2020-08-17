from sys import platform


def libc_path():
    if platform.startswith('linux'):
        # TODO(Jan): implement this with 'gcc --print-file-name=libc.so'
        return ""
    elif platform.startswith('darwin'):
        return "/usr/lib/system/libsystem_c.dylib"
    else:
        return "/usr/lib64/lic.so"  # This is our best guess


def dynamic_lib_extension():
    if platform.startswith('linux'):
        return ".so"
    elif platform.startswith('darwin'):
        return ".dylib"
    else:
        return ".so"  # This is our best guess
