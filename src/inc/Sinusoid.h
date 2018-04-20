#if !defined(__Sinusoid_hdr__)
#define __Sinusoid_hdr__

#include "ErrorDef.h"

class CSinusoid
{
public:
    enum SinusoidParam_t
    {
        kNumFFT,
        kHopSize,
        kMaxNSines,
        kMinSinDur,
        kFreqDevOffset,
        kFreqDevSlope,
        kAmpThresdB, 
        
        kNumSinusoidParams
    };
    static Error_t create(CSinusoid *& pCSinusoid);
    static Error_t destroy(CSinusoid *& pCSinusoid);
    Error_t init( int iBlockSize, int iHopSize, float fSampleRateInHz, float fMaxNSines, float fMinSinDur, float fFreqDevOffset, float fFreqDevSlope, float fAmpThresdB);
    Error_t reset();
    Error_t setParam(SinusoidParam_t eParam, float fParamValue);
    float   getParam() const;
    Error_t analyse(float* fInputBuffer, int iNumFrames);
    Error_t synthesize();
    Error_t process();
    
    CSinusoid();
    ~CSinusoid();
    
    
    
private:
    bool m_isInParamRange (SinusoidParam_t eParam, float fParamValue);
    bool m_bIsInitialized;
    float m_fSampleRateHz;
    float m_afParams[kNumSinusoidParams];
    float m_aafParamRange[kNumSinusoidParams][2];
    int iNumFrames;
    
    float **m_ppfXTFreq;
    float **m_ppfXTMag;
    float **m_ppfXTPhase;
    
    
    Error_t peakDetection(float* pfFFTMag);
    int m_iNumPeaksDetected;
    float *m_pfPeakLoc;
    
    Error_t peakInterp(float* pfFFTMag, float* pfFFTPhase);
    
    
};

#endif
