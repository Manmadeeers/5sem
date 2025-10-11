
// MyProt2D.h: основной файл заголовка для приложения MyProt2D
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CMyProt2DApp:
// Сведения о реализации этого класса: MyProt2D.cpp
//

class CMyProt2DApp : public CWinApp
{
public:
	CMyProt2DApp() noexcept;


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMyProt2DApp theApp;
