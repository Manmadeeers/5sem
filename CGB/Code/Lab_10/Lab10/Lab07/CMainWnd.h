#pragma once
#include "resource.h"
#include "CMatrix.h"
#include <float.h> // DBL_MAX, DBL_MIN
#include <math.h>
#include <vector>
#include <fstream>

#include "LibGraph.h"
#include "LibSurface.h"


class CMainWnd : public CFrameWnd
{
private:
	CRect WinRect; // Область в окне
	int Index;
	CMatrix PView;
	CMatrix PView2;
	CMenu menu;
	COLORREF color;
	DECLARE_MESSAGE_MAP()
	int OnCreate(LPCREATESTRUCT);

public:
	CMainWnd::CMainWnd()
	{
		Create(NULL, L"Lab10", WS_OVERLAPPEDWINDOW, CRect(0, 0, 600, 600), NULL, NULL);
		Index = 0;
		PView.RedimMatrix(3);
		PView2.RedimMatrix(3);
		PView(0) = 10; PView(1) = 30; PView(2) = 60;
		PView2(0) = 10; PView2(1) = 45; PView2(2) = 45;
		color = RGB(255, 255, 0);
	}

	void OnPaint();
	void OnSize(UINT nType, int cx, int cy);

	void OnDiffuseModel();
	void OnMirrorModel();
	void Exit();
	void dataCam();
	void dataLight();
	void colR();
	void colG();
	void colB();
	void colY();
};

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_SPHERE_DIFFUSE, OnDiffuseModel)
	ON_COMMAND(ID_SPHERE_MIRROR, OnMirrorModel)
	ON_COMMAND(ID_EXIT, Exit)
	ON_COMMAND(ID_40012, dataCam)
	ON_COMMAND(ID_40011, dataLight)
	ON_COMMAND(ID_40014, colR)
	ON_COMMAND(ID_40015, colG)
	ON_COMMAND(ID_40016, colB)
	ON_COMMAND(ID_40017, colY)
END_MESSAGE_MAP()
int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	menu.LoadMenu(IDR_MENU1); // Загрузить меню из файла ресурса
	SetMenu(&menu); // Установить меню
	return 0;
}
void CMainWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	WinRect.SetRect(200, cy-155, cx-155, 155);
}


void CMainWnd::OnPaint()
{
	CPaintDC dc(this);
	CString ss;
	ss.Format(L"Наблюдатель: r=%.1f, fi=%.1f, th=%.1f", PView(0), PView(1), PView(2));
	dc.TextOutW(0, 0, ss);
	ss.Format(L"Свет: r=%.1f, fi=%.1f, th=%.1f", PView2(0), PView2(1), PView2(2));
	dc.TextOutW(0, 50, ss);
	if (Index == 1)	///диф
		DrawLightSphere(dc, 4.5, PView, PView2, WinRect, color, 0);
	if (Index == 2)	///зерк
		DrawLightSphere(dc, 4.5, PView, PView2, WinRect, color, 1);
}



void CMainWnd::OnDiffuseModel()
{
	Index = 1;
	Invalidate();
}
void CMainWnd::OnMirrorModel()
{
	Index = 2;
	Invalidate();
}
void CMainWnd::Exit()
{
	DestroyWindow();
}

void CMainWnd::dataCam() {
	std::ifstream fin("dataView.txt");
	fin >> PView(1) >> PView(2);
	Invalidate();
}

void CMainWnd::dataLight() {
	std::ifstream fin("dataLight.txt");
	fin >> PView2(1) >> PView2(2);
	Invalidate();
}

void CMainWnd::colR() {
	color = RGB(255, 0, 0);
	Invalidate();
}
void CMainWnd::colG() {
	color = RGB(0, 255, 0);
	Invalidate();
}
void CMainWnd::colB() {
	color = RGB(0, 0, 255);
	Invalidate();
}
void CMainWnd::colY() {
	color = RGB(255, 255, 0);
	Invalidate();
}