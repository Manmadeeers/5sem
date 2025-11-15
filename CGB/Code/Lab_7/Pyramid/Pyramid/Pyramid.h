
// Pyramid.h: основной файл заголовка для приложения Pyramid
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CPyramidApp:
// Сведения о реализации этого класса: Pyramid.cpp
//

class CPyramidApp : public CWinApp
{
public:
	CPyramidApp() noexcept;


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPyramidApp theApp;
