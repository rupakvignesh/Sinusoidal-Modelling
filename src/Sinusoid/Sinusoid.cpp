
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


///////////////////////////////////////////////////////////////////////////////////
//Detecting k largest peaks
struct vi
{
    float value;int index;
};

int compare(const void *a,const void *b)
{
    struct vi *a1 = (struct vi *)a;
    struct vi *a2 = (struct vi*)b;
    if((*a1).value>(*a2).value)return -1;
    else if((*a1).value<(*a2).value)return 1;
    else return 0;
}

void pickKLargest(float* pfMag,float *pfPeakLoc, float *pfPhase, float *temp1, float *temp2, float *temp3, int N, int k){
    
    struct vi objects [N];
    for(int i=0; i<N; i++){
        objects[i].value=temp1[i];
        objects[i].index=i;
    }
    
    qsort(objects, N, sizeof(objects[0]), compare);
    
    for (int i=0; i<k; i++){
        pfMag[i] = objects[i].value;
        pfPeakLoc[i] = temp2[objects[i].index];
        pfPhase[i] = temp3[objects[i].index];
    }
}

int mod(int a, int b){
    int c = a%b;
    return (c<0) ? c + b : c;
}

float linInterp(float iploc, float fVal1, float fVal2){
    
    float i1 = iploc - floor(iploc);
    return (1-i1)*fVal1 + i1*fVal2;
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


Error_t CSinusoid::init(int iBlockSize, int iHopSize, float fSampleRateInHz, float fMaxNSines, float fMultFactor, float fAmpThresdB)
{
    
    if(m_bIsInitialized)
    {
        reset();
    }
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Setting sample rate
    m_fSampleRateHz = fSampleRateInHz;
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Setting parameter values
    setParam(CSinusoid::kNumFFT, iBlockSize);
    setParam(CSinusoid::kHopSize, iHopSize);
    setParam(CSinusoid::kMaxNSines, fMaxNSines);
    setParam(CSinusoid::kAmpThresdB, fAmpThresdB);
    setParam(CSinusoid::kMultFactor, fMultFactor);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Creating and initializing Fft
    CFft::createInstance(m_pCFft);
    m_pCFft->initInstance(m_afParams[kNumFFT],1,CFft::kWindowHamming,CFft::kNoWindow);
    m_pfAnWindow = new float [(int) m_afParams[kNumFFT]];
    m_pfSynWindow = new float [(int) m_afParams[kNumFFT]];
    
    m_pCFft->getWindow(m_pfAnWindow);
    float fSum = 0;
    for(int i = 0;i<(int) m_afParams[kNumFFT];i++)
    {
        fSum += m_pfAnWindow[i];
    }
    for(int i = 0;i<(int) m_afParams[kNumFFT];i++)
    {
        m_pfAnWindow[i] /= fSum;
        m_pfSynWindow[i] = 1/m_pfAnWindow[i];
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////////
    //Initialize Ring buffer
    
    m_pCRingbuffer = new CRingBuffer<float> ((int)m_afParams[CSinusoid::kNumFFT]);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Initializing private pointers
    m_piPeakLoc     = new int   [(int) m_afParams[CSinusoid::kNumFFT]/2 +1];
    
    m_pfIpMag       = new float [(int) m_afParams[CSinusoid::kNumFFT]/2 +1];
    m_pfIpPhase     = new float [(int) m_afParams[CSinusoid::kNumFFT]/2 +1];
    m_pfIpPeakLoc   = new float [(int) m_afParams[CSinusoid::kNumFFT]/2 +1];
    
    
    m_pfTempBuffer = new float [(int)m_afParams[CSinusoid::kNumFFT]];
    
    m_pfMagSpectrum = new float [(int) m_afParams[CSinusoid::kNumFFT]/2 +1];
    m_pfPhaseSpectrum = new float [(int) m_afParams[CSinusoid::kNumFFT]/2 +1];
    m_pfSpectrum = new CFft::complex_t [(int) m_afParams[CSinusoid::kNumFFT]];
    
    m_pfReal = new float [(int) m_afParams[CSinusoid::kNumFFT]];
    m_pfImag = new float [(int) m_afParams[CSinusoid::kNumFFT]];
    
    m_pfFinalLoc = new float [(int) m_afParams[CSinusoid::kMaxNSines]];
    m_pfFinalMag = new float [(int) m_afParams[CSinusoid::kMaxNSines]];
    m_pfFinalPhase = new float [(int) m_afParams[CSinusoid::kMaxNSines]];
    
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
    
    //Delete Ring buffer
    delete m_pCRingbuffer;
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Deletingprivate pointers
    delete m_piPeakLoc;
    
    delete m_pfIpPeakLoc;
    delete m_pfIpPhase;
    delete m_pfIpMag;
    
    delete m_pfMagSpectrum;
    delete m_pfPhaseSpectrum;
    delete m_pfSpectrum;
    
    delete m_pfReal;
    delete m_pfImag;
    
    delete m_pfTempBuffer;
    
    delete m_pfFinalPhase;
    delete m_pfFinalMag;
    delete m_pfFinalLoc;
    
    delete m_pfAnWindow;
    delete m_pfSynWindow;
    
    return kNoError;
    
}

Error_t CSinusoid::analyze(float *pfInputBuffer)
{
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Applying Analysis Window
    for(int i = 0;i<(int)m_afParams[CSinusoid::kNumFFT];i++)
    {
        m_pfTempBuffer[i] = pfInputBuffer[i]*m_pfAnWindow[i];
    }
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Rearranging input for zero phase FFT
    m_pCRingbuffer->setWriteIdx((int)m_afParams[CSinusoid::kNumFFT]/2);
    m_pCRingbuffer->put(m_pfTempBuffer,(int)m_afParams[CSinusoid::kNumFFT]);
    m_pCRingbuffer->setWriteIdx(0);
    
    for(int i = 0;i<(int)m_afParams[CSinusoid::kNumFFT];i++)
    {
        m_pfTempBuffer[i] = m_pCRingbuffer->getPostInc();
    }

    for(int i=0; i<(int)m_afParams[CSinusoid::kNumFFT]/2+1;i++){
        m_pfMagSpectrum[i] = 0;
        m_pfPhaseSpectrum[i] = 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Reset private members
    for(int i=0; i<(int)m_afParams[CSinusoid::kNumFFT];i++){
        m_pfSpectrum[i] = 0;
        m_pfReal[i] = 0;
        m_pfImag[i] = 0;
    }
    
    //Fft
    m_pCFft->doFft(m_pfSpectrum, m_pfTempBuffer);
    m_pCFft->getMagnitudeInDb(m_pfMagSpectrum, m_pfSpectrum);
    m_pCFft->getUnwrapPhase(m_pfPhaseSpectrum, m_pfSpectrum);
    
    
    //Peak detection
    peakDetection(m_pfMagSpectrum);
    
    //Peak interpolation
    peakInterp(m_pfMagSpectrum, m_pfPhaseSpectrum);
    
    return kNoError;
    
}

Error_t CSinusoid::synthesize(float *pfOutputBuffer)
{
    ///////////////////////////////////////////////////////////////////////////////////
    //Generate sine waves in frequency domain
    genspecsines_C(m_pfFinalLoc, m_pfFinalMag, m_pfFinalPhase, m_iNumPeaksDetected, m_pfReal, m_pfImag, (int)m_afParams[CSinusoid::kNumFFT]);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //Ifft
    m_pCFft->mergeRealImag(m_pfSpectrum, m_pfReal, m_pfImag);

    m_pCFft->doInvFft(pfOutputBuffer, m_pfSpectrum);
    ///////////////////////////////////////////////////////////////////////////////////
    //Apply inverse window
    m_pCRingbuffer->setWriteIdx((int)m_afParams[CSinusoid::kNumFFT]/2-1);
    m_pCRingbuffer->put(pfOutputBuffer,(int) m_afParams[CSinusoid::kNumFFT]);
    m_pCRingbuffer->setWriteIdx(0);
    for(int i = 0;i<m_afParams[CSinusoid::kNumFFT];i++)
    {
        pfOutputBuffer[i] = m_pCRingbuffer->getPostInc()*m_pfSynWindow[i]/m_afParams[CSinusoid::kNumFFT];
    }
    
    return kNoError;
}

Error_t CSinusoid::peakDetection(float *pfMagSpectrum)
{
    int k = 0;
    for(int i = 1; i< floor(m_afParams[kNumFFT]/2);i++)
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
        
        m_pfIpPeakLoc[i] = (m_piPeakLoc[i] + 0.5*(fLeftVal-fRightVal)/(fLeftVal-2*fCurrVal+fRightVal));
        m_pfIpMag[i] = fCurrVal - 0.25*(fLeftVal-fRightVal)*(m_pfIpPeakLoc[i] - (float)m_piPeakLoc[i]);
        m_pfIpPeakLoc[i] *= m_afParams[CSinusoid::kMultFactor]; // Apply Frequency shifting
        m_pfIpPhase[i] = linInterp(m_pfIpPeakLoc[i], pfPhaseSpectrum[(int)(m_pfIpPeakLoc[i])], pfPhaseSpectrum[(int)(m_pfIpPeakLoc[i])+1]);
    }
    
    //Pick k largest peaks
    if(m_iNumPeaksDetected>= m_afParams[CSinusoid::kMaxNSines])
    {
        pickKLargest(m_pfFinalMag,m_pfFinalLoc,m_pfFinalPhase,m_pfIpMag,m_pfIpPeakLoc,m_pfIpPhase, m_iNumPeaksDetected, (int)m_afParams[CSinusoid::kMaxNSines]);
        m_iNumPeaksDetected = (int)m_afParams[CSinusoid::kMaxNSines];
    }
    else
    {
        for(int i = 0;i<m_iNumPeaksDetected;i++)
        {
            m_pfFinalMag[i] = m_pfIpMag[i];
            m_pfFinalLoc[i] = m_pfIpPeakLoc[i];
            m_pfFinalPhase[i] = m_pfIpPhase[i];
        }
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



