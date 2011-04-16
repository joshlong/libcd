
#include "stdafx.h"
#include "TaskInfo.h"
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <direct.h>
#include <math.h>
#include "cdex.h"
#include "Audiofile.h"
#include "ID3Tag.h"
#include "Filename.h"
#include "Encode.h"
#include "AsyncEncoder.h"
#include "ISndStream.h"
#include "SndStreamFactory.h"
#include "cdinfo.h"
#include "cdrip/CDRip.h"

#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>

/// #define CDRIPPER_INTEROP_API extern "C" __declspec(dllexport)

/// internally among the cpp/h the token means, 'export'
/// for the client of the dll, the header file will mean 
/// 'import' because they wont define the constant

#include "cdripper_interop.h"  

using namespace std;

// forward reference so I can define the thing at 
// the bottom since its such a monstrousity 
DWORD privateRipTrackToFile( int driveNo , int trackNo,  LPCWSTR output);

 
 
  /* we use this to convert the strings into hexadecimal. I lifted this from: 
	http://www.arachnoid.com/cpptutor/student3.html
  
  */
string convBase(unsigned long v, long base)
{
	string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}

void init(int d)
{	
 //	init before anything else
// TransportLayer
// 0 = ASPI drivers
// 1 = Native NT scsi drivers
	CR_Init(1,1) ; 
	CR_SetActiveCDROM(d) ; 

} 


   /** tells if the given track is an audio track or not (and thus, ifi ts worth ripping */

  BOOL isAudioTrack(int discNo, int trackNo)
{
	 // TODO this should be doable with the CDInfo / TrackInfo combo

	BOOL answer =  FALSE ;

	init(discNo) ;
	CR_LockCD(TRUE) ;
	auto_ptr<CDInfo > cdInfo  (new CDInfo);

	cdInfo->ReadToc() ; 
	answer = cdInfo->IsAudioTrack(trackNo) ;
	CR_LockCD(FALSE) ;
	CR_DeInit() ;
 

	return answer ;
}

std::string  getDiscId(int d)
{	


	DWORD answer =  0 ;

	init(d) ;
	CR_LockCD(TRUE) ;
	auto_ptr<CDInfo > cdInfo  (new CDInfo);

	cdInfo->ReadToc() ; 
	answer = cdInfo->CalculateDiscID() ;
	CR_LockCD(FALSE) ;
	CR_DeInit() ;

	string  sDiscId =  convBase(answer, 16)  ;

	transform ( 
		sDiscId.begin(), 
		sDiscId.end(), 
		sDiscId.begin(),   
		(int(*)(int))  toupper
	);
	const char* p = sDiscId.c_str(); 
	///std::cout<< "the value of discid is "<< p<<std::endl;
	
	//char* q = const_cast<char*>(p);
	return p ; 
}

BOOL ripTrackToFile( int driveNo , int trackNo,  LPCWSTR output)
{ 
	DWORD result = privateRipTrackToFile(driveNo,trackNo,output) ; 
	return result == ((DWORD) 0 );
}

void printInfoOnTracksForDrive(int drive) 
{ 
	
	init(drive) ; // 

	long readTOC = CR_ReadToc() ;
	CR_LockCD(TRUE) ;   
	long count = CR_GetNumTocEntries() ;
	long tocEntryLong = 0 ;
	for( tocEntryLong=0; tocEntryLong < count ; tocEntryLong++ )
	{
		TOCENTRY tocEntry = CR_GetTocEntry( tocEntryLong ) ;
		TOCENTRY tocEntryForNextTrack = CR_GetTocEntry( tocEntryLong + 1) ; // this apparently works? 
		DWORD startSector = tocEntry.dwStartSector ;
		DWORD stopSector = tocEntryForNextTrack.dwStartSector -1 ;  

		int trackNo = tocEntry.btTrackNumber;
		byte flag = tocEntry.btFlag ;
		std::cout	
			<< "the track number is "<< trackNo <<  "; "
			<< "start sector "<<  startSector << "; " 
			<< "stop sector "<<  stopSector << "; " 
			<< "type " << 
			(flag & CDROMDATAFLAG ? "Data" : "Audio" )
			<< std::endl ;

	}
	CR_LockCD(FALSE) ; // unlock 
	CR_DeInit() ; 
} 

int  countDevices() 
{ 
	CR_Init(1,1 ) ; 
	int cdRomsDetected =  CR_GetNumCDROM() ;
	
	CR_DeInit() ;   
	  return cdRomsDetected; 
}

std::string   getProductNameForDevice( int device)
{ 
	CR_Init(1,1 ) ; 
	int cdRomsDetected = CR_GetNumCDROM() ;
	//std::cout << "there are "<< cdRomsDetected<< " cd drives detected."<<std::endl ; 
	
	int nDrive = 0;
	int nActiveCdDrive = CR_GetActiveCDROM() ; 
	char lpszInfo[4096];
	// well resuse the pointed later 
	CDROMPARAMS cdParams;
	std::string name ; 
	 
//	std::cout<< "the active drive is "<< nActiveCdDrive <<std::endl ; 
	for( nDrive =0  ;  nDrive < cdRomsDetected ; nDrive++) {
		CR_SetActiveCDROM(nDrive) ; 
		CR_GetCDROMParameters( &cdParams)  ; 
		CR_GetDetailedDriveInfo(lpszInfo,sizeof(lpszInfo)); 
		if(nDrive == device)
		{
			name = 	cdParams.lpszProductID  ;
			break; 
		}
			
	}
	// on my particular system this corresponds to the drive containing the trax I want to rip

	CR_SetActiveCDROM(nActiveCdDrive) ;   // the Sony, on my laptop, need to find some
						///way to make this easier to get insight into, 
						//	maybe add a Java method that can list all cd drives so the user of the client can choose and the valye will be whats 'keyed' into all api calls 
	
	CR_DeInit() ; 

	return name; 
}

 /** this is mainly to provide debug information/context
 */
void printInfoOnDrives()
{
	CR_Init(1,1 ) ; 
	int cdRomsDetected = CR_GetNumCDROM() ;
	std::cout << "there are "<< cdRomsDetected<< " cd drives detected."<<std::endl ; 
	
	int nDrive = 0;
	int nActiveCdDrive = CR_GetActiveCDROM() ; 
	char lpszInfo[4096];
	// well resuse the pointed later 
	CDROMPARAMS cdParams;

	std::cout<< "the active drive is "<< nActiveCdDrive <<std::endl ; 
	for( nDrive =0  ;  nDrive < cdRomsDetected ; nDrive++) {
		CR_SetActiveCDROM(nDrive) ; 
		CR_GetCDROMParameters( &cdParams)  ; 
		CR_GetDetailedDriveInfo(lpszInfo,sizeof(lpszInfo)); 
		std::cout<< "looking at drive " << nDrive << std::endl; 
		std::cout << 
			"Product ID:\t"	<<	cdParams.lpszProductID  << ":" << std::endl <<
			"CDROMID:\t"	<<	cdParams.lpszCDROMID	<< ":" << std::endl <<
			"Vendor ID:\t"	<<	cdParams.lpszVendorID   << ":" << std::endl <<
			"Lun ID:\t"		<<  cdParams.btLunID		<< ":" << std::endl <<
			"Adapter ID:\t" <<	cdParams.btAdapterID	<< ":" << std::endl <<
			"Target ID:\t" <<	cdParams.btTargetID		<< ":" << std::endl <<
			"Advanced Information:" << std::endl << lpszInfo << ":" << std::endl 
			<< "-----------------------------------------------------------------" << std::endl; 
		
			
	}
	// on my particular system this corresponds to the drive containing the trax I want to rip

	CR_SetActiveCDROM(nActiveCdDrive) ;   // the Sony, on my laptop, need to find some
						///way to make this easier to get insight into, 
						//	maybe add a Java method that can list all cd drives so the user of the client can choose and the valye will be whats 'keyed' into all api calls 
	
	CR_DeInit() ; 
}



BOOL inject(int drive )  
{
	init(drive) ;
 
	BOOL answer = FALSE ;
	 
	CDMEDIASTATUS   mediaStatus; // enum 
	if(CR_EjectCD(TRUE) )
	{
		CR_IsMediaLoaded( mediaStatus) ;
		if( mediaStatus == CDMEDIA_NOT_PRESENT_TRAY_OPEN)
		{
			
		answer =	CR_EjectCD(FALSE);
		}
	}
	 CR_DeInit() ;

	return answer ;
} 

BOOL isOpen(int drive) 
{
	init(drive) ;

	BOOL answer = FALSE ; 
	 
	CDMEDIASTATUS   mediaStatus; // enum 
	 
	CR_IsMediaLoaded( mediaStatus) ;
	
	CR_DeInit() ;
	if( mediaStatus == CDMEDIA_PRESENT)
	{
     	answer = FALSE ;
	} 
	else if (mediaStatus ==CDMEDIA_NOT_PRESENT_TRAY_OPEN)
	{
	 answer = TRUE ;
	} 

	 return answer ; // we dont know one way or another 

}

BOOL eject(int drive )  
{	
	init(drive);
	BOOL answer = FALSE ;
	CDMEDIASTATUS   mediaStatus; // enum 
	answer=CR_EjectCD(TRUE) ;
	CR_DeInit() ;

	return answer  ;
}

int    countTracks(  int  drive)  
{    
	CR_DeInit();

	init( drive) ; 
	CR_ReadToc() ;
	long answer=CR_GetNumTocEntries();
	CR_DeInit();
	
	return answer; 
	
}
DWORD privateRipTrackToFile( int driveNo , int trackNo,  LPCWSTR output)
{ 
 
	//announceMacros() ; 
	
	ULONG prc;
	CR_Init(1,1 ) ; 
	CR_SetActiveCDROM(driveNo);
	CR_ReadToc() ;
	CR_LockCD(TRUE) ;
	long		nBufferSize = 0;
	LONG nNumBytesRead = 0;
	LONG nOffset = 0;
	CDEX_ERR	bReturn = CDEX_OK;
	BOOL		bPause = FALSE;	
	TOCENTRY tocEntryForTrackOne = CR_GetTocEntry(trackNo) ;
	TOCENTRY tocEntryForTrackTwo = CR_GetTocEntry(trackNo+1) ;
	BOOL exclude = FALSE ;
	
	// ----------------
	///stuff for ripper
		/// we need these variables when get the encoder for ripping to .WAV
	// they get updated when we call getCDRpInfo
	int				nSampleFreq		= 44100;
	int				nChannels		= 2;
	int				nBitsPerChannel = 16;
		DWORD			dwNumberOfSamples = 0; 

	// we need a CTAskInfo 
	CTaskInfo newTask;
	newTask.SetFullFileName( CUString(output));
	///newTask.SetOutDir(  L"C:/mwavs");
	newTask.SetStartSector( tocEntryForTrackOne.dwStartSector ) ;
	newTask.SetEndSector( tocEntryForTrackTwo.dwStartSector -1 ) ;



	ENCODER_TYPES nEncoderType = ENCODER_WAV ;
	auto_ptr<CEncoder> pEncoder( EncoderObjectFactory( nEncoderType ) );
		// Initialize the encoder
	if ( CDEX_OK != pEncoder->InitEncoder( &newTask ) )
	{
		return CDEX_FILEOPEN_ERROR;
	}
	
	// Open conversion stream of encoder
	if ( CDEX_OK != pEncoder->OpenStream(	newTask.GetFullFileNameNoExt() , 
		nSampleFreq, nChannels ) )
	{
		return CDEX_FILEOPEN_ERROR;
	}
	// Step 2: Get the requested buffer size of the output stream
	DWORD dwSampleBufferSize = pEncoder->GetSampleBufferSize();

	// Step 3: Setup the ripper
	//LONG nBufferSize;


	///now we start the actual writing 
	
	if (CR_OpenRipper(	&nBufferSize,
		newTask.GetStartSector(),
		newTask.GetEndSector(),
                        TRUE
						)==CDEX_OK)
	{
		BOOL f =(BOOL)  FALSE; 
		CAsyncEncoder	feeder( pEncoder.get(),  (BOOL&)f, dwSampleBufferSize, 256 );
		LONG			nNumBytesRead = 0;
		LONG			nOffset = 0;
	
		// create the stream buffer, allocate on enocder frame additional memory
        // allocate extra memory for offset correction
		auto_ptr<BYTE> pbtBufferStream( new BYTE[ nBufferSize + dwSampleBufferSize * sizeof( SHORT ) + 16383 ] );

		// Get a pointer to the buffer
		BYTE* pbtStream = pbtBufferStream.get();

		CDEX_ERR ripErr;
	
		 // vars we need for keeping track of data during rip
		LONG m_nJitterPos = 0; 
		int mPercent = 0; 
		int m_nJitterErrors =0 ;
	 
//		QueryPerformanceCounter( &liRipStart );

		// Read all chunks
		while (	( CDEX_RIPPING_DONE  != ( ripErr = CR_RipChunk( pbtStream + nOffset, &nNumBytesRead, f ) ) ) 
				 )
		{
			SHORT*	psEncodeStream=(SHORT*)pbtStream;
			DWORD	dwSamplesToConvert= ( nNumBytesRead + nOffset ) / sizeof( SHORT );
  
			 

			// Get progress indication
			mPercent = CR_GetPercentCompleted();

			// Get relative jitter position
			 m_nJitterPos = CR_GetJitterPosition();

			// Get the number of jitter errors
			 m_nJitterErrors = CR_GetNumberOfJitterErrors();

			// Get the Peak Value
			newTask.SetPeakValue( CR_GetPeakValue() );

			// Convert the samples with the encoder
			while ( dwSamplesToConvert >= dwSampleBufferSize )
			{
				 

				dwNumberOfSamples += dwSampleBufferSize;

				// add samples to feeder
				if( CDEX_OK != feeder.Add( psEncodeStream, dwSampleBufferSize ) )
				{
					return CDEX_ERROR;
				}

				// Decrease the number of samples to convert
				dwSamplesToConvert -= dwSampleBufferSize;

				// Increase the sample buffer pointer
				psEncodeStream += dwSampleBufferSize;
			}

			// Copy the remaing bytes up front, if necessary
			if ( dwSamplesToConvert > 0 )
			{
				// Calculate the offset in bytes
				nOffset = dwSamplesToConvert * sizeof( SHORT );
				// Copy up front
				memcpy( pbtStream, psEncodeStream, nOffset );
			}
			else
			{
				nOffset = 0;
			}
		}
		if ( nOffset  )
		{
			dwNumberOfSamples += nOffset / sizeof( SHORT );
			if(feeder.Add((SHORT*)pbtStream, nOffset / sizeof( SHORT ) )!= CDEX_OK )
			{
				return CDEX_ERROR;
			}
		}
		feeder.WaitForFinished();
        CR_CloseRipper( &prc );
	}



	// ----------------
	///stuff for ripper

	CR_LockCD(FALSE) ;
	CR_DeInit( ) ; 
	return (DWORD) 0;
}