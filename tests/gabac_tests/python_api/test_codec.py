#!/usr/bin/env python
"""Encoding / Decoding tests."""
import gabac
import logging as log
import numpy as np
import unittest


class CodecTest(unittest.TestCase):
    """Encoding / Decoding tests."""

    def _test(self,
              input_symbols,
              binarization_id,
              binarization_parameters,
              context_selection_id):
        """
        Test single configuration.

        :param self:
        :param input_symbols:
        :param binarization_id:
        :param binarization_parameters:
        :param context_selection_id:
        :return:
        """
        log.info(
            "binarization_id: {}, "
            "binarization_parameters: {}, "
            "context_selection_id: {}".format(
                binarization_id,
                binarization_parameters,
                context_selection_id
            )
        )

        (bitstream) = gabac.encode(
            input_symbols,
            binarization_id,
            binarization_parameters,
            context_selection_id
        )

        (decoded_symbols) = gabac.decode(
            bitstream,
            binarization_id,
            binarization_parameters,
            context_selection_id
        )

        self.assertTrue(np.array_equal(decoded_symbols, input_symbols))

        gabac.release(bitstream)
        gabac.release(decoded_symbols)

    def test_roundtrip_config(self):
        """
        Test all configurations.

        :return: none
        """
        log.basicConfig(
            format='%(levelname)s  %(asctime)s  %(message)s',
            level=log.INFO
        )

        log.info("testing BI binarization")
        binarization_id = 0
        for cLength in [1, 8, 16, 32]:
            binarization_parameters = np.array([cLength], np.intc)
            if cLength == 1:
                input_symbols = np.arange(0, 2, 1, np.uint64)
            elif cLength == 8:
                input_symbols = np.arange(0, 256, 1, np.uint64)
            elif cLength == 16:
                # This assures that 65535 is also checked :-)
                input_symbols = np.arange(0, 65536, 15, np.uint64)
            else:
                # This assures that 4294967295 is also checked :-)
                input_symbols = np.arange(0, 4294967296, 65535, np.uint64)
            for context_selection_id in [0, 1, 2, 3]:
                self._test(input_symbols,
                           binarization_id,
                           binarization_parameters,
                           context_selection_id)

        log.info("testing TU binarization")
        binarization_id = 1
        for cMax in [1, 32]:
            binarization_parameters = np.array([cMax], np.intc)
            input_symbols = np.arange(0, (cMax + 1), 1, np.uint64)
            for context_selection_id in [0, 1, 2, 3]:
                self._test(input_symbols,
                           binarization_id,
                           binarization_parameters,
                           context_selection_id)

        log.info("testing EG binarization")
        binarization_id = 2
        binarization_parameters = np.array([], np.intc)
        input_symbols = np.arange(0, 32768, 1, np.uint64)
        for context_selection_id in [0, 1, 2, 3]:
            self._test(input_symbols,
                       binarization_id,
                       binarization_parameters,
                       context_selection_id)

        log.info("testing SEG binarization")
        binarization_id = 3
        binarization_parameters = np.array([], np.intc)
        input_symbols = np.arange(-16383, 16384, 1, np.uint64)
        for context_selection_id in [0, 1, 2, 3]:
            self._test(input_symbols,
                       binarization_id,
                       binarization_parameters,
                       context_selection_id)

        log.info("testing TEG binarization")
        binarization_id = 4
        for cTruncExpGolParam in [1, 32]:
            binarization_parameters = np.array([cTruncExpGolParam], np.intc)
            input_symbols = np.arange(0, (32768 + cTruncExpGolParam), 1,
                                      np.uint64)
            for context_selection_id in [0, 1, 2, 3]:
                self._test(input_symbols,
                           binarization_id,
                           binarization_parameters,
                           context_selection_id)

        log.info("testing STEG binarization")
        binarization_id = 5
        for cSignedTruncExpGolParam in [1, 32]:
            binarization_parameters = np.array([cSignedTruncExpGolParam],
                                               np.intc)
            input_symbols = np.arange(
                (-16383 - cSignedTruncExpGolParam),
                (16384 + cSignedTruncExpGolParam),
                1,
                np.uint64
            )
            for context_selection_id in [0, 1, 2, 3]:
                self._test(input_symbols,
                           binarization_id,
                           binarization_parameters,
                           context_selection_id)


if __name__ == '__main__':
    unittest.main()
