#if !defined(__Sinusoid_hdr__)
#define __Sinusoid_hdr__

#include "ErrorDef.h"
#include "RingBuffer.h"
class CFft;
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
    float   getParam(SinusoidParam_t eParam) const;
    Error_t analyze(float* fInputBuffer);
    Error_t synthesize(float *pfOutputBuffer);
    Error_t process();
    
    
    virtual ~CSinusoid(){};
    
    
    
private:
    bool m_isInParamRange (SinusoidParam_t eParam, float fParamValue);
    bool m_bIsInitialized;
    float m_fSampleRateHz;
    float m_afParams[kNumSinusoidParams];
    float m_aafParamRange[kNumSinusoidParams][2];
    int iNumFrames;
    float *m_pfTempBuffer;
    
    CFft *m_pCFft;
    float *m_pfAnWindow;
    float *m_pfSynWindow;
    
    float *m_pfIpPeakLoc;
    float *m_pfIpPeakLocInHz;
    float *m_pfIpMag;
    float *m_pfIpPhase;
    
    CRingBuffer<float> *m_pCRingbuffer;
    
    
    Error_t peakDetection(float* pfFFTMag);
    int m_iNumPeaksDetected;
    int *m_piPeakLoc;
    
    Error_t peakInterp(float* pfFFTMag, float* pfFFTPhase);
    
    
//    Error_t generateSynthWindow();
//    float *m_pfSynthWindow;
    
};

#endif
