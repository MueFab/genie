class GABAC_RETURN:
    r"""Return Codes.
    
    Values:
        SUCCESS : if success.
        FAILURE : if failed.
    """
    SUCCESS = 0
    FAILURE = 1


class GABAC_LOG_LEVEL:
    r"""
    Different logging urgency

    Values:
        TRACE   : Log every step in great detail
        DEBUG   : Intermediate results
        INFO    : Expected Results
        WARNING : Suspicious events (may be an error)
        ERROR   : Handled errors
        FATAL   : Error causing application to terminate
    """
    TRACE = 0
    DEBUG = 1
    INFO = 2
    WARNING = 3
    ERROR = 4
    FATAL = 5


class GABAC_TRANSFORM:
    r"""
    Gabac available transformations

    Values:
        gabac_transform_NONE        : Do nothing
        gabac_transform_EQUALITY    : Find equal values sequentially
        gabac_transform_MATCH       : Find larger sequence matches
        gabac_transform_RLE         : Find run lengths
        gabac_transform_LUT         : Remap symbols based on probability
        gabac_transform_DIFF        : Use differences between symbol values instead of symbols
        gabac_transform_CABAC       : core based on paramcabac
    """
    NONE = 0
    EQUALITY = 1
    MATCH = 2
    RLE = 3
    LUT = 4
    DIFF = 5
    CABAC = 6


class GABAC_BINARIZATION:
    r"""
    Binarizations for paramcabac transformation

    Values:
        BI : Binary
        TU : Truncated Unary
        EG : Exponential Golomb
        SEG  : Signed Exponential Golomb
        TEG  : Truncated Exponential Golomb
        STEG : Signed Truncated Exponential Golomb
    """
    BI = 0
    TU = 1
    EG = 2
    SEG = 3
    TEG = 4
    STEG = 5


class GABAC_CONTEXT_SELECT:
    r"""
    Context selection modes for paramcabac transformation

    Values:
        BYPASS           : Do not use arithmetic core
        ADAPTIVE_ORDER_0 : Current symbol only
        ADAPTIVE_ORDER_1 : Use current + previous symbol
        ADAPTIVE_ORDER_2 : Use current + previous + before previous symbol
    """
    BYPASS = 0
    ADAPTIVE_ORDER_0 = 1
    ADAPTIVE_ORDER_1 = 2
    ADAPTIVE_ORDER_2 = 3


class GABAC_OPERATION:
    r"""
    Enum for gabac operation

    Values
        ENCODE  : Use configuration to compress
        DECODE  : Use configuration to decompress
        ANALYZE : Find best configuration for input data
    """
    ENCODE = 0
    DECODE = 1
    ANALYZE = 2


class GABAC_STREAM_MODE:
    r"""
    Flags for different data types

    Values
        FILE    : Read/write from file
        BUFFER  : Read/write from data block
    """
    FILE = 0
    BUFFER = 1
