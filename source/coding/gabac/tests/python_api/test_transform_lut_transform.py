#!/usr/bin/env python
"""Test LUT transform."""
import gabac
import logging as log
import numpy as np

import unittest


class TransformLutTest(unittest.TestCase):
    """Test LUT transform."""

    def _test(self, input_symbols):
        """
        Test one round trip.

        :param input_symbols:
        :return:
        """
        (transsymbols, invlut) = (
            gabac.transform_lut_transform0(input_symbols)
        )

        (decoded_symbols) = (
            gabac.inverse_transform_lut_transform0(transsymbols, invlut)
        )

        self.assertTrue(np.array_equal(decoded_symbols, input_symbols))

        gabac.release(decoded_symbols)
        gabac.release(transsymbols)
        gabac.release(invlut)

    def test_lut_transform(self):
        """
        Test all round trips.

        :return:
        """
        log.basicConfig(
            format='%(levelname)s  %(asctime)s  %(message)s',
            level=log.INFO
        )

        input_symbols_list = [np.array([-4, 31, 90, 172], np.uint64),
                              np.array([], np.uint64),
                              np.array([0, 0, 0, 0, 0, 0], np.uint64)]

        log.info(
            "testing {} input symbol sequences".format(len(input_symbols_list))
        )
        for input_symbols in input_symbols_list:
            self._test(input_symbols)


if __name__ == '__main__':
    unittest.main()
