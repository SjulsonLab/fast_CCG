import ctypes
import numpy as np
import matplotlib.pyplot as plt

# Load the shared library
lib = ctypes.CDLL('./libCCGEngine.so')

# Set the argument types for the C function
lib.CCGEngine.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_uint), ctypes.c_double, ctypes.c_uint, ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_uint)]

# Generate some random test data
nSpikes = 1000
Times = np.sort(np.random.rand(nSpikes))
Marks = np.random.randint(1, 5, nSpikes)
BinSize = 0.01
HalfBins = 50
CountArraySize = np.max(Marks)**2 * (2*HalfBins + 1)
print(CountArraySize)


# Initialize output arrays
Count = np.zeros((CountArraySize,), dtype=np.uint32)
print(Count.size)
Pairs = np.zeros((2*nSpikes,), dtype=np.uint32)  # Make large enough for worst case

# Convert numpy arrays to ctypes arrays
Times_ctypes = Times.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
Marks_ctypes = Marks.ctypes.data_as(ctypes.POINTER(ctypes.c_uint))
Count_ctypes = Count.ctypes.data_as(ctypes.POINTER(ctypes.c_uint))
Pairs_ctypes = Pairs.ctypes.data_as(ctypes.POINTER(ctypes.c_uint))

# printing stuff
# Marks = Marks.cumsum(0)
#print(Times)
print(Marks)

# Call the C function
print('calling C function...')
lib.CCGEngine(nSpikes, Times_ctypes, Marks_ctypes, BinSize, HalfBins, Count_ctypes, Pairs_ctypes)

# Plot results
plt.figure(figsize=(10,6))
plt.bar(np.arange(CountArraySize), Count)
plt.xlabel('Index')
plt.ylabel('Count')
plt.title('CCGEngine Result')
plt.show()
