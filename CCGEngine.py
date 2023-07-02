# This is a python wrapper for CCGEngine.c
# It is completely untested output from GPT4
#
# I'm hoping someone can test this!
#
# Thanks,
# Luke

import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load the shared library
lib = ctypes.CDLL('./libCCGEngine.so')

# Set the argument types for the C function
lib.CCGEngine.argtypes = [ctypes.c_int, 
                          ctypes.POINTER(ctypes.c_double), 
                          ctypes.POINTER(ctypes.c_uint), 
                          ctypes.c_double, 
                          ctypes.c_uint, 
                          ctypes.POINTER(ctypes.c_uint), 
                          ctypes.POINTER(ctypes.c_uint)]


def CCGEngine(times, groups, binSize_Fs, halfBins):
    # Initialize output arrays
    CountArraySize = np.max(groups)**2 * (2*halfBins + 1)

    # Initialize output arrays
    Count = np.zeros((CountArraySize,), dtype=np.uint32)
    Pairs = np.zeros((2*len(times),), dtype=np.uint32)  # Make large enough for worst case


	# Convert numpy arrays to ctypes arrays
	Times_ctypes = Times.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
	Marks_ctypes = Marks.ctypes.data_as(ctypes.POINTER(ctypes.c_uint))
	Count_ctypes = Count.ctypes.data_as(ctypes.POINTER(ctypes.c_uint))
	Pairs_ctypes = Pairs.ctypes.data_as(ctypes.POINTER(ctypes.c_uint))

	# call the C function
	lib.CCGEngine(len(times), Times_ctypes, Marks_ctypes, BinSize, HalfBins, Count_ctypes, Pairs_ctypes)

    # Reshape Count to (nBins, n, n)
    n = np.max(groups)
    nBins = 2*halfBins+1
    Count = np.reshape(Count, [nBins, n, n])
    
    # Pad Count if necessary
    if n < np.max(groups):
        Count = np.pad(Count, ((0, 0), (0, np.max(groups) - n), (0, np.max(groups) - n)), mode='constant')

    return Count

