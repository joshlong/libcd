// test_cdex_dll.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
 
#include <iostream>
#include <cctype>
#include <string> 
#include <algorithm>
#include <string>
///#include "cdrip/CDRip.h"
 
#define MY_DRIVE 2  

#include <cdripper_interop.h>
using namespace std;

  

 // this isjust a test 
int main( )
{ 
  
 	// lets try all this stufff out for size!
	// first, can we get information: 
	printInfoOnDrives() ; 
	printInfoOnTracksForDrive(MY_DRIVE) ; 

	// then can we eject and so on? 
	eject(MY_DRIVE) ; 
	std::cout << "is the drive open? " << (isOpen( MY_DRIVE) == TRUE ? "yes":"no") 
			<< std::endl ;

	Sleep( 5 * 1000 ) ; 
	inject(MY_DRIVE ) ; 

	std::cout << "injected and ejected" << std::endl;
	Sleep( 5 * 1000 ) ; 
	std::cout << "there are " << countTracks(MY_DRIVE)
			<< " tracks." << std::endl ; 

	
	std::string    discId = getDiscId(MY_DRIVE  ) ; 

	 std::cout << "the disc id of the CD is "<< discId << std::endl; 

	ripTrackToFile(MY_DRIVE,2,L"C:/mwavs/myOutNo2.wav");  
	std::cout << "the discid is " << discId << std::endl ;  
}

