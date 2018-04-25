
#include <iostream>

#include "Sinusoid.h"
#include "ErrorDef.h"
#include "Fft.h"
#include "Synthesis.h"


#ifdef __cplusplus
extern "C"
{
#endif
    
    #include "utilFunctions.h"
    
#ifdef __cplusplus
}
#endif


int mod(int a, int b){
    int c = a%b;
    return (c<0) ? c + b : c;
}

float linInterp(float iploc, float fVal1, float fVal2){
    
    float i1 = iploc - floor(iploc);
    return (1-i1)*fVal2 + i1*fVal1;
}

void applyWindow(float *pfOutputBuffer, const int iNumFrames){
    
    //Apply triangle
    for (int i=0; i<iNumFrames/2; i++){
        pfOutputBuffer[i] *= (float)i/((float)iNumFrames/2);
    }
    for (int i=0; i<iNumFrames/2; i++){
        pfOutputBuffer[iNumFrames-i-1] *= (float)i/((float)iNumFrames/2);
    }
    
    //Apply inverse hamming
    for (int i=0; i<iNumFrames; i++){
        pfOutputBuffer[i] /= (0.53836 - 0.46164*cos(2*3.14*i/((float)iNumFrames)));
    }
    
}

Error_t CSinusoid::create(CSinusoid *&pCSinusoid)
{
    
    if (pCSinusoid) {
        return kUnknownError;
    }
    pCSinusoid = new CSinusoid ();
    return kNoError;
}

Error_t CSinusoid::destroy(CSinusoid *&pCSinusoid)
{
    
    if(!pCSinusoid)
        return kUnknownError;
    delete pCSinusoid;
    pCSinusoid = 0;
    return kNoError;
}


Error_t CSinusoid::init(int iBlockSize, int iHopSize, float fSampleRateInHz, float fMaxNSines, float fMinSinDur, float fFreqDevOffset, float fFreqDevSlope, float fAmpThresdB)
{
    
    if(m_bIsInitialized)
    {
        reset();
    }
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Setting sample rate
    m_fSampleRateHz = fSampleRateInHz;

    ///////////////////////////////////////////////////////////////////////////////////
    //Setting parameter ranges
    m_aafParamRange[CSinusoid::kNumFFT][0] = 0;
    m_aafParamRange[CSinusoid::kNumFFT][1] = 4096;
    
    m_aafParamRange[CSinusoid::kHopSize][0] = 0;
    m_aafParamRange[CSinusoid::kHopSize][0] = iBlockSize;
    
    m_aafParamRange[CSinusoid::kMaxNSines][0] = 0;
    m_aafParamRange[CSinusoid::kMaxNSines][1] = 300;
    
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Setting parameter values
    setParam(CSinusoid::kNumFFT, iBlockSize);
    setParam(CSinusoid::kHopSize, iHopSize);
    setParam(CSinusoid::kMaxNSines, fMaxNSines);
    setParam(CSinusoid::kFreqDevSlope, fFreqDevSlope);
    setParam(CSinusoid::kFreqDevOffset, fFreqDevOffset);
    setParam(CSinusoid::kAmpThresdB, fAmpThresdB);
    setParam(CSinusoid::kMinSinDur, fMinSinDur);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Creating and initializing Fft
    CFft::createInstance(m_pCFft);
    m_pCFft->initInstance(m_afParams[kNumFFT],2,CFft::kWindowHamming);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Initializing private pointers
    m_piPeakLoc     = new int   [(int)m_afParams[CSinusoid::kMaxNSines]];
    m_pfIpMag       = new float [(int)m_afParams[CSinusoid::kMaxNSines]];
    m_pfIpPhase     = new float [(int)m_afParams[CSinusoid::kMaxNSines]];
    m_pfIpPeakLoc   = new float [(int)m_afParams[CSinusoid::kMaxNSines]];

    
    m_bIsInitialized = true;
    
    
    return kNoError;
}

Error_t CSinusoid::reset ()
{
    if(!m_bIsInitialized)
    {
        return kNotInitializedError;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Deleting 
    CFft::destroyInstance(m_pCFft);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Deletingprivate pointers
    delete m_piPeakLoc;
    delete m_pfIpPeakLoc;
    delete m_pfIpPhase;
    delete m_pfIpMag;
    
    return kNoError;
    
}

Error_t CSinusoid::analyze(float *pfInputBuffer)
{
    ///////////////////////////////////////////////////////////////////////////////////
    //Initializing variables
    float *pfMagSpectrum = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    float *pfPhaseSpectrum = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    CFft::complex_t *pfSpectrum = new CFft::complex_t [(int) m_afParams[CSinusoid::kNumFFT] *2];
    
    //Fft
    m_pCFft->doFft(pfSpectrum, pfInputBuffer);
    m_pCFft->getMagnitude(pfMagSpectrum, pfSpectrum);
    m_pCFft->getPhase(pfPhaseSpectrum, pfSpectrum);
    
    
    //Peak detection
    peakDetection(pfMagSpectrum);
    
    //Peak interpolation
    peakInterp(pfMagSpectrum, pfPhaseSpectrum);
    
    return kNoError;
    
}

Error_t CSinusoid::synthesize(float *pfOutputBuffer)
{
    ///////////////////////////////////////////////////////////////////////////////////
    //Generate sine waves in frequency domain
    float *pfReal = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    float *pfImag = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    genspecsines_C(m_pfIpPeakLoc, m_pfIpMag, m_pfIpPhase, m_iNumPeaksDetected, pfReal, pfImag, (int)m_afParams[CSinusoid::kNumFFT]);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Ifft
    CFft::complex_t *pfSpectrum = new CFft::complex_t [(int) m_afParams[CSinusoid::kNumFFT] *2];
    m_pCFft->mergeRealImag(pfSpectrum, pfReal, pfImag);
    m_pCFft->doInvFft(pfOutputBuffer, pfSpectrum);
    ///////////////////////////////////////////////////////////////////////////////////
    //Apply inverse window
    
    applyWindow(pfOutputBuffer, (int) m_afParams[CSinusoid::kNumFFT]);
    
    return kNoError;
}

Error_t CSinusoid::peakDetection(float *pfMagSpectrum)
{
    int k = 0;
//    float fMinOfMax = 0;
    for(int i = 1; i<m_afParams[kNumFFT]-1;i++)
    {
        if(pfMagSpectrum[i]>m_afParams[CSinusoid::kAmpThresdB] && pfMagSpectrum[i]>pfMagSpectrum[i-1] && pfMagSpectrum[i]>pfMagSpectrum[i+1])
        {
            m_piPeakLoc[k] = i;
            k++;
        }
    }
    m_iNumPeaksDetected = k;
    return kNoError;
    
}


Error_t CSinusoid::peakInterp(float *pfMagSpectrum, float *pfPhaseSpectrum)
{
    float fCurrVal = 0;
    float fLeftVal = 0;
    float fRightVal = 0;
    for(int i = 0;i<m_iNumPeaksDetected;i++)
    {
        fCurrVal = pfMagSpectrum[m_piPeakLoc[i]];
        fLeftVal = pfMagSpectrum[m_piPeakLoc[i]-1];
        fRightVal = pfMagSpectrum[m_piPeakLoc[i]+1];
        m_pfIpPeakLoc[i] = m_piPeakLoc[i] + 0.5*(fLeftVal-fRightVal)*(fLeftVal-2*fCurrVal+fRightVal);
        m_pfIpMag[i] = fCurrVal - 0.25*(fLeftVal-fRightVal)*(m_pfIpPeakLoc[i] - m_piPeakLoc[i]);
        
        //to do //Need to do linear interpolation for phase Python code: ipphase = np.interp(iploc, np.arange(0, pX.size), pX)
        m_pfIpPhase[i] = linInterp(m_pfIpPeakLoc[i], pfPhaseSpectrum[m_piPeakLoc[i]-1], pfPhaseSpectrum[m_piPeakLoc[i]]);
    }
    
    return kNoError;
}

Error_t CSinusoid::setParam(CSinusoid::SinusoidParam_t eParam, float fParamValue)
{
    ///////////////////////////////////////////////////////////////////////////////////
    //Implementation without checking the parameter range
    m_afParams[eParam] = fParamValue;
    
    return kNoError;
}

float CSinusoid::getParam(CSinusoid::SinusoidParam_t eParam) const
{
    if(!m_bIsInitialized)
    {
        return kNotInitializedError;
    }
    return m_afParams[eParam];
}



