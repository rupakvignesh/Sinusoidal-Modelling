
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>

#ifndef UTILFUNCTIONS_H 
    
#define UTILFUNCTIONS_H

#define BH_SIZE 1001
#define BH_SIZE_BY2 501
#define MFACTOR 100


#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#define oneTOtwo(i,j, nCol) ((i)*nCol + (j))

#define MAGSILENCE -120
#define F0CANDPERPEAK 6
#define F0NUMPEAK 3                //number of top peaks which should be considerd for generating f0 candidates
#define TWM_p 0.5                  //weighting by frequency value
#define TWM_q 1.4                  //weighting related to magnitude of peaks
#define TWM_r 0.5                  //scaling related to magnitude of peaks
#define TWM_rho 0.33               //weighting of MP error
#define MAXNPEAKS 10               // maximum number of peaks used for TWM 

void genbh92lobe_C(float *x, float *y, int N);
void genspecsines_C(float *iploc, float *ipmag, float *ipphase, int n_peaks, float *real, float*imag, int size_spec);
void maxValArg(float *data, int dLen, float *max_val, int*max_ind);
void minValArg(float *data, int dLen, float *min_val, int*min_ind);
void computeTWMError(float **peakMTX1, int nCols1, float **peakMTX2, int nCols2, int maxnpeaks, float * pmag, float *f0Error, int nF0Cands, int PMorMP);
int nearestElement(float val, float *data, int len, float *min_val);
int TWM_C(float *pfreq, float *pmag, int nPeaks, float *f0c, int nf0c, float *f0, float *f0error);

#endif  //UTILFUNCTIONS_H
