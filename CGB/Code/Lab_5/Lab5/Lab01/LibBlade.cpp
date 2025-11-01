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
	cz.cx = fabs(right - left);	// Ширина прямоугольной области
	cz.cy = fabs(top - bottom);	// Высота прямоугольной области
	return cz;
}

//----------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy)
// Формирует матрицу для преобразования координат объекта при его смещении 
// на dx по оси X и на dy по оси Y в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при смещении начала
// системы координат на -dx оси X и на -dy по оси Y при фиксированном положении объекта 
{
	CMatrix TM(3, 3);
	TM(0, 0) = 1; TM(0, 2) = dx;
	TM(1, 1) = 1;  TM(1, 2) = dy;
	TM(2, 2) = 1;
	return TM;
}

//------------------------------------------------------------------------------------
CMatrix CreateRotate2D(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте
// на угол fi (при fi>0 против часовой стрелки)в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при повороте начала
// системы координат на угол -fi при фиксированном положении объекта 
// fi - угол в градусах
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * pi; // Перевод в радианы
	CMatrix RM(3, 3);
	RM(0, 0) = cos(ff); RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);  RM(1, 1) = cos(ff);
	RM(2, 2) = 1;
	return RM;
}


//------------------------------------------------------------------------------

CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// Возвращает матрицу пересчета координат из мировых в оконные
// RS - область в мировых координатах - double
// RW - область в оконных координатах - int
{
	CMatrix M(3, 3);
	CSize sz = RW.Size();	 // Размер области в ОКНЕ
	int dwx = sz.cx;	     // Ширина
	int dwy = sz.cy;	     // Высота
	CSizeD szd = RS.SizeD(); // Размер области в МИРОВЫХ координатах

	double dsx = szd.cx;    // Ширина в мировых координатах
	double dsy = szd.cy;    // Высота в мировых координатах

	double kx = (double)dwx / dsx;   // Масштаб по x
	double ky = (double)dwy / dsy;   // Масштаб по y

	M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;
	M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;
	M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
	return M;
}

//------------------------------------------------------------------------------

void SetMyMode(CDC& dc, CRectD& RS, CRect& RW) //Устанавливает соответствие между мировыми и оконными координатами
// Устанавливает режим отображения MM_ANISOTROPIC и его параметры
// dc - ссылка на класс CDC MFC
// RS -  область в мировых координатах - int
// RW -	 Область в оконных координатах - int  
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
	double rS = 30; // радиус круга в центре
	double RoE = 10 * rS; // радиус орбиты
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

	// Размеры для всех лопастей (одинаковые)
	double bladeSize = 5;
	FirstTop.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	SecondTop.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	FirstBootom.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	SecondBootom.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	LeftBlade.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
	RightBlade.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);

	EarthOrbit.SetRect(-RoE, RoE, RoE, -RoE);

	// Углы для лопастей
	fiFT = 35;   // Первая верхняя (красная)
	fiST = 25;   // Вторая верхняя (красная)
	fiFB = 205;  // Первая нижняя (красная)
	fiSB = 215;  // Вторая нижняя (красная)
	fiLeft = 180; // Левая (синяя)
	fiRight = 0;  // Правая (синяя)
	// Углы для синих лопастей (горизонтальные)
	fiBlue1 = 5;    // Правая горизонтальная (синяя)
	fiBlue2 = -5;    // Верхняя часть правой лопасти (синяя)
	fiBlue3 = 175;   // Левая горизонтальная (синяя)
	fiBlue4 = 185;   // Верхняя часть левой лопасти (синяя)


	wPoint = -10; //скорость вращения
	dt = 1;

	// Инициализация матриц координат
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

	// Обновление координат для синих лопастей (горизонтальные)
	//double RoV = (EarthOrbit.right - EarthOrbit.left) / 2;

	// Синие лопасти (горизонтальные)
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

	// Дополнительные синие лопасти для лучшего визуального эффекта
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
	// Получаем матрицу преобразования координат
	CMatrix transformMatrix = SpaceToWindow(RS, RW);

	// Создаем черное перо для обводки
	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = dc.SelectObject(&blackPen);

	// Преобразуем координаты центра
	CMatrix center(3);
	center(0) = 0; center(1) = 0; center(2) = 1;
	CMatrix transformedCenter = transformMatrix * center;

	// Рисуем орбиту с черной обводкой
	/*dc.SelectStockObject(NULL_BRUSH);
	dc.Ellipse(EarthOrbit);*/

	// Рисуем красные вертикальные лопасти с черной обводкой
	DrawTriangleWithBorder(FTCoords, STCoords, dc, RGB(255, 0, 0), transformMatrix);
	DrawTriangleWithBorder(FBCoords, SBCoords, dc, RGB(255, 0, 0), transformMatrix);

	// Рисуем синие горизонтальные лопасти с черной обводкой
	DrawTriangleWithBorder(BlueCoords1, BlueCoords2, dc, RGB(0, 0, 255), transformMatrix);
	DrawTriangleWithBorder(BlueCoords3, BlueCoords4, dc, RGB(0, 0, 255), transformMatrix);

	// Рисуем центр (солнце) с черной обводкой
	CBrush* pOldBrush = dc.SelectObject(&SunBrush);
	dc.Ellipse(MainPoint);
	dc.SelectObject(pOldBrush);

	// Восстанавливаем старое перо
	dc.SelectObject(pOldPen);
}

void CBlade::DrawTriangleWithBorder(CMatrix FCoords, CMatrix SCoords, CDC& dc, COLORREF fillColor, CMatrix& transformMatrix)
{
	// Применяем преобразование координат
	CMatrix center(3); center(0) = 0; center(1) = 0; center(2) = 1;
	CMatrix p1 = transformMatrix * FCoords;
	CMatrix p2 = transformMatrix * SCoords;
	CMatrix pc = transformMatrix * center;

	// Создаем кисть и перо
	CBrush brush(fillColor);
	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));

	// Выбираем наши объекты
	CPen* pOldPen = dc.SelectObject(&blackPen);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	// Определяем точки треугольника
	POINT points[3] = {
		{0, 0},
		{static_cast<int>(FCoords(0)), static_cast<int>(FCoords(1))},
		{static_cast<int>(SCoords(0)), static_cast<int>(SCoords(1))}
	};

	// Рисуем заполненный треугольник с обводкой
	dc.Polygon(points, 3);

	// Восстанавливаем старые объекты
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CBlade::DrawBladePair(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color, CMatrix& transformMatrix)
{
	CRect R;
	int d = 5; // Фиксированный размер лопастей

	// Создаем кисть и перо нужного цвета
	CBrush brush(color);
	CPen pen(PS_SOLID, 1, color);

	CPen* pOldPen = dc.SelectObject(&pen);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	// Первая лопасть
	R.SetRect(
		static_cast<int>(coords1(0) - d),
		static_cast<int>(coords1(1) + d),
		static_cast<int>(coords1(0) + d),
		static_cast<int>(coords1(1) - d)
	);
	dc.Ellipse(R);

	// Вторая лопасть
	R.SetRect(
		static_cast<int>(coords2(0) - d),
		static_cast<int>(coords2(1) + d),
		static_cast<int>(coords2(0) + d),
		static_cast<int>(coords2(1) - d)
	);
	dc.Ellipse(R);

	// Соединительные линии
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

	// Рисуем треугольник
	DrawTriangleWithBorder(coords1, coords2, dc, color, transformMatrix);

	// Восстанавливаем старые объекты
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CBlade::DrawBlade(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color)
{
	// Настройки кисти и пера
	CBrush brush(color);
	CPen pen(PS_SOLID, 1, color);

	CPen* pOldPen = dc.SelectObject(&pen);
	CBrush* pOldBrush = dc.SelectObject(&brush);

	// Размер лопастей
	int bladeSize = 5;

	// Рисуем первую лопасть
	CRect rect1(
		static_cast<int>(coords1(0) - bladeSize),
		static_cast<int>(coords1(1) + bladeSize),
		static_cast<int>(coords1(0) + bladeSize),
		static_cast<int>(coords1(1) - bladeSize)
	);
	dc.Ellipse(rect1);

	// Рисуем вторую лопасть
	CRect rect2(
		static_cast<int>(coords2(0) - bladeSize),
		static_cast<int>(coords2(1) + bladeSize),
		static_cast<int>(coords2(0) + bladeSize),
		static_cast<int>(coords2(1) - bladeSize)
	);
	dc.Ellipse(rect2);

	// Рисуем соединительные линии
	dc.MoveTo(0, 0);
	dc.LineTo(static_cast<int>(coords1(0)), static_cast<int>(coords1(1)));
	dc.MoveTo(0, 0);
	dc.LineTo(static_cast<int>(coords2(0)), static_cast<int>(coords2(1)));

	// Рисуем треугольник
	POINT triangle[3] = {
		{0, 0},
		{static_cast<int>(coords1(0)), static_cast<int>(coords1(1))},
		{static_cast<int>(coords2(0)), static_cast<int>(coords2(1))}
	};
	dc.Polygon(triangle, 3);

	// Восстанавливаем старые настройки
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}


void CBlade::GetRS(CRectD& RSX)
// RS - структура, куда записываются параметры области графика
{
	RSX.left = RS.left;
	RSX.top = RS.top;
	RSX.right = RS.right;
	RSX.bottom = RS.bottom;
}







