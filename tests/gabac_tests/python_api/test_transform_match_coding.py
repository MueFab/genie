#!/usr/bin/env python
"""Test match transform coding."""

import gabac
import logging as log
import numpy as np
import unittest

g_window_sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256]


class MatchCodingTest(unittest.TestCase):
    """Test match transform coding."""

    def _test(self, input_symbols):
        """
        Test one round trip.

        :param input_symbols:
        :return: none
        """
        global g_window_sizes

        for window_size in g_window_sizes:
            (pointers,
             lengths,
             raw_values) = (
                gabac.transform_match_coding(input_symbols, window_size)
            )

            (decoded_symbols) = (
                gabac.inverse_transform_match_coding(pointers,
                                                     lengths,
                                                     raw_values)
            )

            self.assertTrue(np.array_equal(decoded_symbols, input_symbols))

            gabac.release(pointers)
            gabac.release(lengths)
            gabac.release(raw_values)
            gabac.release(decoded_symbols)

    def test_match_coding(self):
        """
        Test all round trips.

        :return:
        """
        global g_window_sizes

        log.basicConfig(
            format='%(levelname)s  %(asctime)s  %(message)s',
            level=log.INFO
        )

        input_symbols_list = [
            np.array([4, 0, 0, 31, 0, 0, 90, 172], np.uint64),
            np.array([], np.uint64), np.array([0, 0, 0, 0, 0, 0], np.uint64)]

        log.info(
            "testing {} input symbol sequences with {} window sizes".format(
                len(input_symbols_list), len(g_window_sizes))
        )
        for input_symbols in input_symbols_list:
            self._test(input_symbols)


if __name__ == '__main__':
    unittest.main()
