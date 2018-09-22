INPUT (NAME):

a file with the filename inputfile+.lut0 (order 0) or inputfile+.lut1 (order 0) or inputfile+.lut2 (order 0)
e.g.: for file ~\test.nrcomp it would be ~\test.nrcomp.lut0, ~\test.nrcomp.lut1, and  ~\test.nrcomp.lut2 respectively

CONTENT:

lot0 contains only 32-bit unsigned little endian values
lut1 contains only 16-bit unsigned little endian values
lut2 contains only 8-bit unsigned little endian values

FILE FORMAT:
lut0:
number_of_entries per LUT minus 1 (in 32-bit little endian) -> this is actually the largest value that will be transformed

followed by the values of the LUT, where the first value is the transformed value for value 0, 
the second value is the transformed value for value 1,...


lut1:
number_of_entries per LUT minus 1 (in 16-bit little endian) -> this is actually the largest value that will be transformed

(the result will be a 2-dimensional LUT of number_of_entries x number_of_entries values.)

followed by number_of_entries lut0s (without number_of_entries signalling)


lut2:
number_of_entries per LUT minus 1 (in 8-bit little endian) -> this is actually the largest value that will be transformed

the result will be a 3-dimensional LUT of number_of_entries x number_of_entries values x number_of_entries values.

followed by number_of_entries lut1s (without number_of_entries signalling)

