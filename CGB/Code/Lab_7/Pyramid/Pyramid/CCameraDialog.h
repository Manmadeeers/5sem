#pragma once
#include "afxdialogex.h"


// Диалоговое окно CCameraDialog

class CCameraDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraDialog)

public:
	CCameraDialog(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~CCameraDialog();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()

public:
	double m_r;    // Радиус
	double m_fi;   // Азимут (phi)
	double m_q;    // Угол возвышения (theta)

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
