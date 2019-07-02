#!/usr/bin/env python
"""Test diff coding."""
import gabac
import logging as log
import numpy as np
import unittest


class TransformDiffTest(unittest.TestCase):
    """Test diff coding."""

    def _test(self, input_symbols):
        """
        Test one round trip.

        :param input_symbols:
        :return: none
        """
        (transformed_symbols) = (
            gabac.transform_diff_coding(input_symbols)
        )

        (decoded_symbols) = (
            gabac.inverse_transform_diff_coding(transformed_symbols)
        )

        self.assertTrue(np.array_equal(decoded_symbols, input_symbols))

        gabac.release(transformed_symbols)
        gabac.release(decoded_symbols)

    def test_roundtrip_diff_coding(self):
        """
        Test multiple round trips.

        :return: none
        """
        log.basicConfig(
            format='%(levelname)s  %(asctime)s  %(message)s',
            level=log.INFO
        )

        input_symbols_list = [np.array([4, 31, 90, 72], np.uint64),
                              np.array([], np.uint64),
                              np.array([0, 0, 0, 0, 0, 0], np.uint64)]

        log.info(
            "testing {} input symbol sequences".format(len(input_symbols_list))
        )
        for input_symbols in input_symbols_list:
            self._test(input_symbols)


if __name__ == '__main__':
    unittest.main()
