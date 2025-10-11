
// MyProt2DView.cpp: реализация класса CMyProt2DView
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#include "MyProt2D.h"

#include "MyProt2DDoc.h"
#include "MyProt2DView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyProt2DView

IMPLEMENT_DYNCREATE(CMyProt2DView, CView)

BEGIN_MESSAGE_MAP(CMyProt2DView, CView)
	ON_COMMAND(ID_TESTS_F1, &CMyProt2DView::OnTestsF1)
	ON_COMMAND(ID_TESTS_F2, &CMyProt2DView::OnTestsF2)
	ON_COMMAND(ID_TESTS_F3, &CMyProt2DView::OnTestsF3)
END_MESSAGE_MAP()

// Создание или уничтожение CMyProt2DView

CMyProt2DView::CMyProt2DView() noexcept
{
	// TODO: добавьте код создания

}

CMyProt2DView::~CMyProt2DView()
{
}

BOOL CMyProt2DView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

#pragma region математика

double CMyProt2DView::MyF1(double x)
{
	if (x == 0) return 1;
	return sin(x) / x;
}

double CMyProt2DView::MyF2(double x)
{
	return sqrt(fabs(x)) * sin(x);
}

#pragma endregion



// Рисование CMyProt2DView


void CMyProt2DView::OnDraw(CDC* pDC)
{
	if (m_ShowShapes)
	{
		CRect rc; GetClientRect(&rc);
		const int cx = rc.Width() / 2;
		const int cy = rc.Height() / 2;
		int R = min(rc.Width(), rc.Height()) / 2 - 150;
		if (R < 10) R = 10;

		CPen penOct(PS_SOLID, 3, RGB(255, 0, 0));
		CPen* oldPen = pDC->SelectObject(&penOct);
		HGDIOBJ oldBrush = pDC->SelectStockObject(NULL_BRUSH);

		POINT pts[8]; 
		double a = 0.0;
		
		for (int i = 0; i < 8; ++i) {
			pts[i].x = cx + int(R * cos(a));
			pts[i].y = cy + int(R * sin(a));
			a += pi / 4;
		}
		pDC->Polygon(pts, 8);

		CPen penCircle(PS_SOLID, 2, RGB(0, 0, 255));
		pDC->SelectObject(&penCircle);
		pDC->Ellipse(cx - R, cy - R, cx + R, cy + R);

		pDC->SelectObject(oldPen);
		pDC->SelectObject(oldBrush);
		return;
	}

	Graph.Draw(*pDC, 1, 1);
}


// Обработчики сообщений CMyPlot2DView
void CMyProt2DView::OnTestsF1()
{
	double xL = -3 * pi;
	double xH = -xL;
	double dx = pi / 36;
	int N = (xH - xL) / dx;
	X.RedimMatrix(N + 1);
	Y.RedimMatrix(N + 1);
	for (int i = 0; i <= N; i++)
	{
		X(i) = xL + i * dx;
		Y(i) = MyF1(X(i));
	}
	PenLine.Set(PS_SOLID, 1, RGB(255, 0, 0));
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 255));
	RW.SetRect(500, 150, 900, 550);
	Graph.SetParams(X, Y, RW);
	Graph.SetPenLine(PenLine);
	Graph.SetPenAxis(PenAxis);
	this->Invalidate();
}

void CMyProt2DView::OnTestsF2()
{
	double xL = -4 * pi;
	double xH = -xL;
	double dx = pi / 36;
	int N = (xH - xL) / dx;
	X.RedimMatrix(N + 1);
	Y.RedimMatrix(N + 1);
	for (int i = 0; i <= N; i++)
	{
		X(i) = xL + i * dx;
		Y(i) = MyF2(X(i));
	}
	PenLine.Set(PS_DASHDOT, 1, RGB(255, 0, 0));
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 0));
	RW.SetRect(500, 150, 900, 550);
	Graph.SetParams(X, Y, RW);
	Graph.SetPenLine(PenLine);
	Graph.SetPenAxis(PenAxis);
	this->Invalidate();
}

void CMyProt2DView::OnTestsF3()
{
	m_ShowShapes = true;
	Invalidate();
}


// Диагностика CMyProt2DView

#ifdef _DEBUG
void CMyProt2DView::AssertValid() const
{
	CView::AssertValid();
}

void CMyProt2DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG