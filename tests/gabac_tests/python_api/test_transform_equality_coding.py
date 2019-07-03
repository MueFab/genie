#!/usr/bin/env python
"""Equality transform tests."""
import gabac
import logging as log
import numpy as np

import unittest


class TransformEqualityTest(unittest.TestCase):
    """Equality transform tests."""

    def _test(self, input_symbols):
        """
        Test one round trip.

        :param input_symbols:
        :return: none
        """
        (equality_flags, values) = (
            gabac.transform_equality_coding(input_symbols)
        )

        (decoded_symbols) = (
            gabac.inverse_transform_equality_coding(equality_flags, values)
        )

        self.assertTrue(np.array_equal(decoded_symbols, input_symbols))

        gabac.release(equality_flags)
        gabac.release(values)
        gabac.release(decoded_symbols)

    def test_transform_equality(self):
        """
        Test all round trips.

        :return:
        """
        log.basicConfig(
            format='%(levelname)s  %(asctime)s  %(message)s',
            level=log.INFO
        )

        input_symbols_list = [
            np.array([-4, -4, 31, 31, 31, 30, 172], np.uint64),
            np.array([], np.uint64), np.array([0, 0, 0, 0, 0, 0], np.uint64)]

        log.info(
            "testing {} input symbol sequences".format(len(input_symbols_list))
        )
        for input_symbols in input_symbols_list:
            self._test(input_symbols)


if __name__ == '__main__':
    unittest.main()
