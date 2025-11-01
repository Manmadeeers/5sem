#include "stdafx.h"

CRectD::CRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}
//------------------------------------------------------------------------------
void CRectD::SetRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------
CSizeD CRectD::SizeD()
{
	CSizeD cz;
	cz.cx = fabs(right - left);	// ������ ������������� �������
	cz.cy = fabs(top - bottom);	// ������ ������������� �������
	return cz;
}

//----------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy)
// ��������� ������� ��� �������������� ��������� ������� ��� ��� �������� 
// �� dx �� ��� X � �� dy �� ��� Y � ������������� ������� ���������
// --- ��� ---
// ��������� ������� ��� �������������� ��������� ������� ��� �������� ������
// ������� ��������� �� -dx ��� X � �� -dy �� ��� Y ��� ������������� ��������� ������� 
{
	CMatrix TM(3, 3);
	TM(0, 0) = 1; TM(0, 2) = dx;
	TM(1, 1) = 1;  TM(1, 2) = dy;
	TM(2, 2) = 1;
	return TM;
}

//------------------------------------------------------------------------------------
CMatrix CreateRotate2D(double fi)
// ��������� ������� ��� �������������� ��������� ������� ��� ��� ��������
// �� ���� fi (��� fi>0 ������ ������� �������)� ������������� ������� ���������
// --- ��� ---
// ��������� ������� ��� �������������� ��������� ������� ��� �������� ������
// ������� ��������� �� ���� -fi ��� ������������� ��������� ������� 
// fi - ���� � ��������
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * pi; // ������� � �������
	CMatrix RM(3, 3);
	RM(0, 0) = cos(ff); RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);  RM(1, 1) = cos(ff);
	RM(2, 2) = 1;
	return RM;
}


//------------------------------------------------------------------------------

CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// ���������� ������� ��������� ��������� �� ������� � �������
// RS - ������� � ������� ����������� - double
// RW - ������� � ������� ����������� - int
{
	CMatrix M(3, 3);
	CSize sz = RW.Size();	 // ������ ������� � ����
	int dwx = sz.cx;	     // ������
	int dwy = sz.cy;	     // ������
	CSizeD szd = RS.SizeD(); // ������ ������� � ������� �����������

	double dsx = szd.cx;    // ������ � ������� �����������
	double dsy = szd.cy;    // ������ � ������� �����������

	double kx = (double)dwx / dsx;   // ������� �� x
	double ky = (double)dwy / dsy;   // ������� �� y

	M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;
	M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;
	M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
	return M;
}

//------------------------------------------------------------------------------

void SetMyMode(CDC& dc, CRectD& RS, CRect& RW) //������������� ������������ ����� �������� � �������� ������������
// ������������� ����� ����������� MM_ANISOTROPIC � ��� ���������
// dc - ������ �� ����� CDC MFC
// RS -  ������� � ������� ����������� - int
// RW -	 ������� � ������� ����������� - int  
{
	double dsx = RS.right - RS.left;
	double dsy = RS.top - RS.bottom;
	double xsL = RS.left;
	double ysL = RS.bottom;

	int dwx = RW.right - RW.left;
	int dwy = RW.bottom - RW.top;
	int xwL = RW.left;
	int ywH = RW.bottom;

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt((int)dsx, (int)dsy);
	dc.SetViewportExt(dwx, -dwy);
	dc.SetWindowOrg((int)xsL, (int)ysL);
	dc.SetViewportOrg(xwL, ywH);
}

CBlade::CBlade()
{
	double rS = 30; // ������ ����� � ������
	double RoE = 10 * rS; // ������ ������
	RS.SetRectD(-RoE, RoE, RoE, -RoE);
	RW.SetRect(0, 0, 600, 600);
	MainPoint.SetRect(-rS, rS, rS, -rS);

	BlueCoords1.RedimMatrix(3);
	BlueCoords2.RedimMatrix(3);
	BlueCoords3.RedimMatrix(3);
	BlueCoords4.RedimMatrix(3);

	RedBrush.CreateSolidBrush(RGB(255, 0, 0));
	BlueBrush.CreateSolidBrush(RGB(0, 0, 255));
	SunBrush.CreateSolidBrush(RGB(0, 128, 0));

	// ������� ��� ���� �������� (����������)
	double bladeSize = 5;
	FirstTop.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	SecondTop.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	FirstBootom.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	SecondBootom.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	LeftBlade.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	RightBlade.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);

	EarthOrbit.SetRect(-RoE, RoE, RoE, -RoE);

	// ���� ��� ��������
	fiFT = 35;   // ������ ������� (�������)
	fiST = 25;   // ������ ������� (�������)
	fiFB = 205;  // ������ ������ (�������)
	fiSB = 215;  // ������ ������ (�������)
	fiLeft = 180; // ����� (�����)
	fiRight = 0;  // ������ (�����)
	// ���� ��� ����� �������� (��������������)
	fiBlue1 = 5;    // ������ �������������� (�����)
	fiBlue2 = -5;    // ������� ����� ������ ������� (�����)
	fiBlue3 = 175;   // ����� �������������� (�����)
	fiBlue4 = 185;   // ������� ����� ����� ������� (�����)


	wPoint = -10; //�������� ��������
	dt = 1;

	// ������������� ������ ���������
	FTCoords.RedimMatrix(3);
	STCoords.RedimMatrix(3);
	FBCoords.RedimMatrix(3);
	SBCoords.RedimMatrix(3);
	LBCoords.RedimMatrix(3);
	RBCoords.RedimMatrix(3);
}

void CBlade::SetNewCoords()
{
	double RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	double ff = (fiFT / 90.0) * pi;
	double x = RoV * cos(ff);
	double y = RoV * sin(ff);
	FTCoords(0) = x;
	FTCoords(1) = y;
	FTCoords(2) = 1;
	fiFT += wPoint * dt;
	CMatrix P = CreateRotate2D(fiFT);
	FTCoords = P * FTCoords;

	RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiST / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	STCoords(0) = x;
	STCoords(1) = y;
	STCoords(2) = 1;
	fiST += wPoint * dt;
	P = CreateRotate2D(fiST);
	STCoords = P * STCoords;

	RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiFB / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	FBCoords(0) = x;
	FBCoords(1) = y;
	FBCoords(2) = 1;
	fiFB += wPoint * dt;
	P = CreateRotate2D(fiFB);
	FBCoords = P * FBCoords;

	RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiSB / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	SBCoords(0) = x;
	SBCoords(1) = y;
	SBCoords(2) = 1;
	fiSB += wPoint * dt;
	P = CreateRotate2D(fiSB);
	SBCoords = P * SBCoords;

	// ���������� ��������� ��� ����� �������� (��������������)
	//double RoV = (EarthOrbit.right - EarthOrbit.left) / 2;

	// ����� ������� (��������������)
	ff = (fiBlue1 / 90.0) * pi;
	BlueCoords1(0) = RoV * cos(ff);
	BlueCoords1(1) = RoV * sin(ff);
	BlueCoords1(2) = 1;
	fiBlue1 += wPoint * dt;
	P = CreateRotate2D(fiBlue1);
	BlueCoords1 = P * BlueCoords1;

	ff = (fiBlue2 / 90.0) * pi;
	BlueCoords2(0) = RoV * cos(ff);
	BlueCoords2(1) = RoV * sin(ff);
	BlueCoords2(2) = 1;
	fiBlue2 += wPoint * dt;
	P = CreateRotate2D(fiBlue2);
	BlueCoords2 = P * BlueCoords2;

	// �������������� ����� ������� ��� ������� ����������� �������
	ff = (fiBlue3 / 90.0) * pi;
	BlueCoords3(0) = RoV * cos(ff);
	BlueCoords3(1) = RoV * sin(ff);
	BlueCoords3(2) = 1;
	fiBlue3 += wPoint * dt;
	P = CreateRotate2D(fiBlue3);
	BlueCoords3 = P * BlueCoords3;

	ff = (fiBlue4 / 90.0) * pi;
	BlueCoords4(0) = RoV * cos(ff);
	BlueCoords4(1) = RoV * sin(ff);
	BlueCoords4(2) = 1;
	fiBlue4 += wPoint * dt;
	P = CreateRotate2D(fiBlue4);
	BlueCoords4 = P * BlueCoords4;
}

void CBlade::Draw(CDC& dc)
{
	// �������� ������� �������������� ���������
	CMatrix transformMatrix = SpaceToWindow(RS, RW);

	// ������� ������ ���� ��� �������
	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = dc.SelectObject(&blackPen);

	// ����������� ���������� ������
	CMatrix center(3);
	center(0) = 0; center(1) = 0; center(2) = 1;
	CMatrix transformedCenter = transformMatrix * center;

	// ������ ������ � ������ ��������
	/*dc.SelectStockObject(NULL_BRUSH);
	dc.Ellipse(EarthOrbit);*/

	// ������ ������� ������������ ������� � ������ ��������
	DrawTriangleWithBorder(FTCoords, STCoords, dc, RGB(255, 0, 0), transformMatrix);
	DrawTriangleWithBorder(FBCoords, SBCoords, dc, RGB(255, 0, 0), transformMatrix);

	// ������ ����� �������������� ������� � ������ ��������
	DrawTriangleWithBorder(BlueCoords1, BlueCoords2, dc, RGB(0, 0, 255), transformMatrix);
	DrawTriangleWithBorder(BlueCoords3, BlueCoords4, dc, RGB(0, 0, 255), transformMatrix);

	// ������ ����� (������) � ������ ��������
	CBrush* pOldBrush = dc.SelectObject(&SunBrush);
	dc.Ellipse(MainPoint);
	dc.SelectObject(pOldBrush);

	// ��������������� ������ ����
	dc.SelectObject(pOldPen);
}

void CBlade::DrawTriangleWithBorder(CMatrix FCoords, CMatrix SCoords, CDC& dc, COLORREF fillColor, CMatrix& transformMatrix)
{
	// ��������� �������������� ���������
	CMatrix center(3); center(0) = 0; center(1) = 0; center(2) = 1;
	CMatrix p1 = transformMatrix * FCoords;
	CMatrix p2 = transformMatrix * SCoords;
	CMatrix pc = transformMatrix * center;

	// ������� ����� � ����
	CBrush brush(fillColor);
	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));

	// �������� ���� �������
	CPen* pOldPen = dc.SelectObject(&blackPen);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	// ���������� ����� ������������
	POINT points[3] = {
		{0, 0},
		{static_cast<int>(FCoords(0)), static_cast<int>(FCoords(1))},
		{static_cast<int>(SCoords(0)), static_cast<int>(SCoords(1))}
	};

	// ������ ����������� ����������� � ��������
	dc.Polygon(points, 3);

	// ��������������� ������ �������
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CBlade::DrawBladePair(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color, CMatrix& transformMatrix)
{
	CRect R;
	int d = 5; // ������������� ������ ��������

	// ������� ����� � ���� ������� �����
	CBrush brush(color);
	CPen pen(PS_SOLID, 1, color);

	CPen* pOldPen = dc.SelectObject(&pen);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	// ������ �������
	R.SetRect(
		static_cast<int>(coords1(0) - d),
		static_cast<int>(coords1(1) + d),
		static_cast<int>(coords1(0) + d),
		static_cast<int>(coords1(1) - d)
	);
	dc.Ellipse(R);

	// ������ �������
	R.SetRect(
		static_cast<int>(coords2(0) - d),
		static_cast<int>(coords2(1) + d),
		static_cast<int>(coords2(0) + d),
		static_cast<int>(coords2(1) - d)
	);
	dc.Ellipse(R);

	// �������������� �����
	dc.MoveTo(0, 0);
	dc.LineTo(
		static_cast<int>(coords1(0)),
		static_cast<int>(coords1(1))
	);
	dc.MoveTo(0, 0);
	dc.LineTo(
		static_cast<int>(coords2(0)),
		static_cast<int>(coords2(1))
	);

	// ������ �����������
	DrawTriangleWithBorder(coords1, coords2, dc, color, transformMatrix);

	// ��������������� ������ �������
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CBlade::DrawBlade(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color)
{
	// ��������� ����� � ����
	CBrush brush(color);
	CPen pen(PS_SOLID, 1, color);

	CPen* pOldPen = dc.SelectObject(&pen);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	// ������ ��������
	int bladeSize = 5;

	// ������ ������ �������
	CRect rect1(
		static_cast<int>(coords1(0) - bladeSize),
		static_cast<int>(coords1(1) + bladeSize),
		static_cast<int>(coords1(0) + bladeSize),
		static_cast<int>(coords1(1) - bladeSize)
	);
	dc.Ellipse(rect1);

	// ������ ������ �������
	CRect rect2(
		static_cast<int>(coords2(0) - bladeSize),
		static_cast<int>(coords2(1) + bladeSize),
		static_cast<int>(coords2(0) + bladeSize),
		static_cast<int>(coords2(1) - bladeSize)
	);
	dc.Ellipse(rect2);

	// ������ �������������� �����
	dc.MoveTo(0, 0);
	dc.LineTo(static_cast<int>(coords1(0)), static_cast<int>(coords1(1)));
	dc.MoveTo(0, 0);
	dc.LineTo(static_cast<int>(coords2(0)), static_cast<int>(coords2(1)));

	// ������ �����������
	POINT triangle[3] = {
		{0, 0},
		{static_cast<int>(coords1(0)), static_cast<int>(coords1(1))},
		{static_cast<int>(coords2(0)), static_cast<int>(coords2(1))}
	};
	dc.Polygon(triangle, 3);

	// ��������������� ������ ���������
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}


void CBlade::GetRS(CRectD& RSX)
// RS - ���������, ���� ������������ ��������� ������� �������
{
	RSX.left = RS.left;
	RSX.top = RS.top;
	RSX.right = RS.right;
	RSX.bottom = RS.bottom;
}







