
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
        sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;
    
    float                   **ppfOutputBuffer = 0;

    CAudioFileIf            *phAudioFile = 0;

    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    float                   fSampleRateInHz = 44100;
    float                   fMaxDelayInSec = 0.01;
    float                   fDelayInSec    = 0.01;
    float                   fWidthInSec    = 0.01;
    float                   fModFreqInHz   = 10;
    int                     iCounter       = 0;
    CAudioFileIf            *pCInstance = 0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else if(argc == 2)
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        
    }
    else if(argc == 3)
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        fDelayInSec     = std::stof(argv[2]);
        fWidthInSec     = fDelayInSec;
    }
    else if(argc == 4)
    {
        sInputFilePath  = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        fDelayInSec     = std::stof(argv[2]);
        fWidthInSec     = fDelayInSec;
        fModFreqInHz    = std::stof(argv[3]);
    }
    else if(argc ==5)
    {
        sInputFilePath  = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        fDelayInSec     = std::stof(argv[2]);
        fModFreqInHz    = std::stof(argv[3]);
        fWidthInSec     = std::stof(argv[4]);;

    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output wav file
    
    CAudioFileIf::create(pCInstance);
    pCInstance->openFile("Output.wav",CAudioFileIf::kFileWrite,&stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];
    
    ppfOutputBuffer= new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfOutputBuffer[i] = new float[kBlockSize];
    

    //////////////////////////////////////////////////////////////////////////////
    // Set Vibrato parameters
    
    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        pCInstance->writeData(ppfOutputBuffer, iNumFrames);
        cout << "\r" << "reading and writing";

        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfOutputBuffer[c][i] << "\t";
            }
            hOutputFile << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(pCInstance);

 

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

