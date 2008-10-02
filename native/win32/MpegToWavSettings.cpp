/*
** CDex - Open Source Digital Audio CD Extractor
**
** Copyright (C) 2006 - 2007 Georgy Berdyshev
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


// MpegToWavSettings.cpp : implementation file


#include "stdafx.h"
#include "cdex.h"
#include "MpegToWavSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMpegToWavSettings dialog


CMpegToWavSettings::CMpegToWavSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CMpegToWavSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMpegToWavSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMpegToWavSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMpegToWavSettings)
	DDX_Control(pDX, IDC_OUTFREQ, m_OutFrequency);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMpegToWavSettings, CDialog)
	//{{AFX_MSG_MAP(CMpegToWavSettings)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMpegToWavSettings message handlers
