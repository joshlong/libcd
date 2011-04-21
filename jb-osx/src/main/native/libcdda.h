
/**  
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @author Josh Long (sort of)
 
 The very large majority of the code in this project - the Linux and OSX implementations and 
 soon on the Windows implementation - is based on, or derived from reading (and USING!) works like 
 discid, libcdio, and cdparanoia.
 
 These projects are written by C programmers who are a million times more adept than I, and I am forever grateful
 that they shared their code. If you're reading this and find it helpful, consider showing your
 support to one of those projects.
 
 */


/**
 Clean, C-only implementation of the MacIoCtlDisc header.
 
 
 Defintions:
  
   deviceName : 'disk1', for example, on OSX 10.6 on a standard MacBook Pro.
 
   	
 */

 
 
int open_cd_drive_door(char *deviceName);

char * get_raw_device_path(char * deviceName);

char * disc_id(char * deviceName) ;

int track_count( char * deviceName);

void  read_track_to_wav_file( char * deviceName, unsigned track, char * whereToDumpWavData)   ;