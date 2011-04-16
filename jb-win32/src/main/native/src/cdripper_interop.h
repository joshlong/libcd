

//#include <iostream>
//#include <cctype>
#include <string> 

#include <windows.h>
#ifdef CDRIPPER_INTEROP_API 
#define CDRIPPER_INTEROP_API  extern "C" __declspec(dllexport)
#else
#define CDRIPPER_INTEROP_API  extern "C"  __declspec(dllimport)  
#endif



#ifdef __cplusplus
 extern "C" { 
#endif 
	 
CDRIPPER_INTEROP_API int  countDevices ()  ;
CDRIPPER_INTEROP_API BOOL isOpen(int drive);
CDRIPPER_INTEROP_API int countTracks(int d);
CDRIPPER_INTEROP_API BOOL eject(int d); 
CDRIPPER_INTEROP_API BOOL inject(int d); 
CDRIPPER_INTEROP_API BOOL isAudioTrack(int d, int t) ;
CDRIPPER_INTEROP_API std::string  getDiscId(int d);
CDRIPPER_INTEROP_API BOOL ripTrackToFile( int driveNo , int trackNo,  LPCWSTR output) ;
CDRIPPER_INTEROP_API void printInfoOnDrives() ; 
CDRIPPER_INTEROP_API  void printInfoOnTracksForDrive(int drive)  ; 
CDRIPPER_INTEROP_API   std::string   getProductNameForDevice( int device) ;
#ifdef __cplusplus
}
#endif
 
 