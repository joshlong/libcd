/*
** CDex - Open Source Digital Audio CD Extractor
**
** Copyright (C) 2006 - 2007 Georgy Berdyshev
** Copyright (C) 1999 - 2002 Albert L. Faber
**
** http://cdexos.sourceforge.net/
** http://sourceforge.net/projects/cdexos 
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ENCODERNTTVQFDLL_H_INCLUDED
#define ENCODERNTTVQFDLL_H_INCLUDED

#include "Encode.h"
#include "twinvq.h"
#include "tvqenc.h"
#include "bfile.h"


//Forward Class Declaration
class CChunkChunk;

class CEncoderNttVqfDll : public CEncoder
{
protected:
	HINSTANCE				m_hDLL;
	CUString					m_strStreamFileName;
public:

	virtual	CEncoderDlg*	GetSettingsDlg();
	// CONSTRUCTOR
	CEncoderNttVqfDll();

	// DESTRUCTOR
	virtual ~CEncoderNttVqfDll();

	// METHODS
	void GetDLLVersionInfo();
	virtual CDEX_ERR InitEncoder( CTaskInfo* pTask );
	virtual CDEX_ERR OpenStream(CUString strOutFileName,DWORD dwSampleRate,WORD nChannels);
	virtual CDEX_ERR EncodeChunk(PSHORT pbsInSamples,DWORD dwNumSamples);
	virtual CDEX_ERR CloseStream();
	virtual CDEX_ERR DeInitEncoder();
private:
	headerInfo		m_setupInfo;
	encSpecificInfo m_encInfo;
	INDEX			m_index;
	FLOAT*			m_pfBuffer;
	CChunkChunk*	m_pTwinChunk;
	BFILE*			m_pbFile;
	CHAR			m_lpszHeaderInfoDir[ BUFSIZ ];

};



#endif
