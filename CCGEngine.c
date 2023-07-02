/* This is a bare-bones C program whose purpose is to compute
   multi-unit cross correlograms quickly.  Not intended for
   use on its own.  It is designed to be wrapped by a Python
   function.
   
   CCGEngine(int nSpikes, double* Times, unsigned long* Marks, double BinSize, unsigned int HalfBins, unsigned long* Count, unsigned long* Pairs)

   nSpikes is the number of spikes
   Times is an array of nSpikes doubles holding the spike times
   Marks is an array holding a number labeling each cell (start at 1, don't use zero!)
   BinSize is a number giving the size of the CCG bins in the same units as Times
   HalfBins is the number of bins to compute - so there are a total of nBins = 1+2*HALFBINS bins
   Count is a long int array holding the counts in each bin.
      It is like a 3d array, indexed by [nBins*nMarks*Mark1 + nBins*Mark2 + Bin]
   Pairs contains the spike numbers of every pair of spikes included in the ccg.


   from Python:
   lib.CCGEngine(nSpikes, Times_ctypes, Marks_ctypes, BinSize, HalfBins, Count_ctypes, Pairs_ctypes)

   NB The spikes MUST be sorted.

   The original mex-file says: 
   "If you think this program is anal, you're right.  Use the MATLAB wrapper instead."

   This hold for the Python/ctypes version as well.


   Tranlated from the original MATLAB/C code by Michaël Zugaro (2012)
   by Luke Sjulson, 2023-07-02

 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define BOUNDSCHECKING_ON // can comment out this line to disable array bounds checking; not sure how much speed that will add
#define STRLEN 10000
#define PAIRBLOCKSIZE 1000000000

unsigned int *Pairs;
unsigned int PairCnt, PairSz;

void AddPair(unsigned int n1, unsigned int n2) {

    if (PairSz==0) {
        Pairs = (unsigned int*)malloc(PAIRBLOCKSIZE*sizeof(unsigned int));
        PairSz = PAIRBLOCKSIZE;
        if (!Pairs) printf("Could not allocate memory for pairs\n");
    }
    if(PairCnt>=PairSz) {
        printf("Too many pairs - increase PAIRBLOCKSIZE in CCGEngine.c\n");
    }
    Pairs[PairCnt++] = n1;
    Pairs[PairCnt++] = n2;
}


void CCGEngine(int nSpikes, double* Times, unsigned long* Marks, double BinSize, unsigned int HalfBins, unsigned long* Count, unsigned long* Pairs) {

    unsigned int nMarks, i, CountArraySize, CountIndex;
    double FurthestEdge;
    unsigned int CenterSpike, Mark1, Mark2, Bin;
    int SecondSpike;
    double Time1, Time2;
    char errstr[STRLEN];

    PairCnt = 0; PairSz = 0;
	/* count nMarks */
	nMarks = 0;
    for(i=0; i<nSpikes; i++) {
        unsigned int Mark = Marks[i];
        if (Mark>nMarks) nMarks = Mark;
        if (Mark==0) {
        	printf("ERROR: NO ZEROS ALLOWED IN MARKS\n");
        	abort();
        }
        // printf("i = %d", i);
    }
    
    unsigned int nBins = 1+2*HalfBins;
    FurthestEdge = BinSize * (HalfBins + 0.5);
    CountArraySize = nMarks * nMarks * nBins;

    // now for the main program
    for(CenterSpike=0; CenterSpike<nSpikes; CenterSpike++) {
        Mark1 = Marks[CenterSpike];
        Time1 = Times[CenterSpike];

        // go backwards from the CenterSpike
        for(SecondSpike=CenterSpike-1; SecondSpike>=0; SecondSpike--) {

        	Time2 = Times[SecondSpike];

			/* check if we have left the interesting region */
			if(fabs(Time1 - Time2) > FurthestEdge) break;

			/* calculate bin */
			Bin = HalfBins + (int)(floor(0.5+(Time2-Time1)/BinSize));

			Mark2 = Marks[SecondSpike];
			CountIndex = nBins*nMarks*(Mark1-1) + nBins*(Mark2-1) + Bin;
            Time2 = Times[SecondSpike];

            if(fabs(Time1 - Time2) > FurthestEdge) break;
            Bin = HalfBins + (int)(floor(0.5+(Time2-Time1)/BinSize));
            Mark2 = Marks[SecondSpike];
            CountIndex = nBins*nMarks*(Mark1-1) + nBins*(Mark2-1) + Bin;
            #ifdef BOUNDSCHECKING_ON
			if (CountIndex<0 || CountIndex >= CountArraySize) {
				printf("Error! t1 %f t2 %f m1 %d m2 %d Bin %d, index %d out of bounds",
					Time1, Time2, Mark1, Mark2, Bin, CountIndex);
				abort();
			}
			#endif
            Count[CountIndex]++;
            AddPair(CenterSpike, SecondSpike);
        }

        // now go forwards from the CenterSpike
        for(SecondSpike=CenterSpike+1; SecondSpike<nSpikes; SecondSpike++) {
            Time2 = Times[SecondSpike];

            if(fabs(Time1 - Time2) >= FurthestEdge) break;

            Bin = HalfBins + (unsigned int)(floor(0.5+(Time2-Time1)/BinSize));
            Mark2 = Marks[SecondSpike];
            CountIndex = nBins*nMarks*(Mark1-1) + nBins*(Mark2-1) + Bin;
            #ifdef BOUNDSCHECKING_ON
			if (CountIndex<0 || CountIndex >= CountArraySize) {
				printf("Error! t1 %f t2 %f m1 %d m2 %d Bin %d, index %d out of bounds",
					Time1, Time2, Mark1, Mark2, Bin, CountIndex);
				abort();
			}
			#endif

            Count[CountIndex]++;
            AddPair(CenterSpike, SecondSpike);
        }

    }
    if (Pairs){
        memcpy(Pairs, (void *)Pairs, PairCnt*sizeof(unsigned int));
        //free(Pairs);
    }
}
