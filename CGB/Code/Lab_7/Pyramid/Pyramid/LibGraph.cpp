#include "pch.h"

//------------------------------------------------------------------------------

CRectD::CRectD()
// Конструктор по умолчанию
{
	left = top = right = bottom = 0;
}

//------------------------------------------------------------------------------

CRectD::CRectD(double l, double t, double r, double b)
// l - левая граница
// t - верхняя граница
// r - правая граница
// b - нижняя граница
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------

void CRectD::SetRectD(double l, double t, double r, double b)
// Устанавливает координаты прямоугольника
// l - левая граница
// t - верхняя граница
// r - правая граница
// b - нижняя граница
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------

CSizeD CRectD::SizeD()
// Возвращает размер прямоугольника
{
	CSizeD cz;
	cz.cx = fabs(right - left);	// Ширина прямоугольной области
	cz.cy = fabs(top - bottom);	// Высота прямоугольной области

	return cz;
}

//------------------------------------------------------------------------------

CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// Возвращает матрицу пересчета координат из мировых в оконные
// RS - область в мировых координатах - double
// RW - область в оконных координатах - int
{
	CMatrix M(3, 3);
	CSize sz = RW.Size();		// Размер области в окне
	int dwx = sz.cx;			// Ширина
	int dwy = sz.cy;			// Высота
	CSizeD szd = RS.SizeD();	// Размер области в мировых координатах
	double dsx = szd.cx;		// Ширина в мировых координатах
	double dsy = szd.cy;		// Высота в мировых координатах
	double kx = (double)dwx / dsx;	// Масштаб по X
	double ky = (double)dwy / dsy;	// Масштаб по Y

	M(0, 0) = kx;
	M(0, 1) = 0;
	M(0, 2) = (double)RW.left - kx * RS.left;
	M(1, 0) = 0;
	M(1, 1) = -ky;
	M(1, 2) = (double)RW.bottom + ky * RS.bottom;
	M(2, 0) = 0;
	M(2, 1) = 0;
	M(2, 2) = 1;

	return M;
}

//------------------------------------------------------------------------------

void SetMyMode(CDC& dc, CRect& RS, CRect& RW)
// Устанавливает режим отображения MM_ANISOTROPIC и его параметры
// dc - ссылка на класс CDC MFC
// RS - область в мировых координатах - int
// RW - область в оконных координатах - int  
{
	int dsx = RS.right - RS.left;
	int dsy = RS.top - RS.bottom;
	int xsL = RS.left;
	int ysL = RS.bottom;
	int dwx = RW.right - RW.left;
	int dwy = RW.bottom - RW.top;
	int xwL = RW.left;
	int ywH = RW.bottom;

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(dsx, dsy);
	dc.SetViewportExt(dwx, -dwy);
	dc.SetWindowOrg(xsL, ysL);
	dc.SetViewportOrg(xwL, ywH);
}

//------------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy)
// Формирует матрицу для преобразования координат объекта при его смещении 
// на dx по оси X и на dy по оси Y в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при смещении начала
// системы координат на -dx по оси X и на -dy по оси Y при фиксированном положении объекта 
// dx - смещение по оси X
// dy - смещение по оси Y
{
	CMatrix TM(3, 3);
	TM(0, 0) = 1;
	TM(0, 2) = dx;
	TM(1, 1) = 1;
	TM(1, 2) = dy;
	TM(2, 2) = 1;

	return TM;
}

//------------------------------------------------------------------------------

CMatrix CreateTranslate3D(double dx, double dy, double dz)
// Формирует матрицу для преобразования координат объекта при его смещении 
// на dx по оси X, на dy по оси Y, на dz по оси Z в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при смещении начала
// системы координат на -dx по оси X, на -dy по оси Y, на -dz по оси Z 
// при фиксированном положении объекта 
// dx - смещение по оси X
// dy - смещение по оси Y
// dz - смещение по оси Z
{
	CMatrix TM(4, 4);

	for (int i = 0; i < 4; i++)
	{
		TM(i, i) = 1;
	}

	TM(0, 3) = dx;
	TM(1, 3) = dy;
	TM(2, 3) = dz;

	return TM;
}

//------------------------------------------------------------------------------

CMatrix CreateRotate2D(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте
// на угол fi (при fi>0 против часовой стрелки) в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при повороте
// системы координат на угол -fi при фиксированном положении объекта 
// fi - угол в градусах
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * M_PI;	// Перевод в радианы
	CMatrix RM(3, 3);
	RM(0, 0) = cos(ff);
	RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);
	RM(1, 1) = cos(ff);
	RM(2, 2) = 1;

	return RM;
}

//------------------------------------------------------------------------------

CMatrix CreateRotate3DZ(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте вокруг оси Z
// на угол fi (при fi>0 против часовой стрелки) в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при повороте
// системы координат вокруг оси Z на угол -fi при фиксированном положении объекта 
// fi - угол в градусах
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * M_PI;	// Перевод в радианы
	CMatrix RM(4, 4);
	RM(0, 0) = cos(ff);
	RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);
	RM(1, 1) = cos(ff);
	RM(2, 2) = 1;
	RM(3, 3) = 1;

	return RM;
}

//------------------------------------------------------------------------------

CMatrix CreateRotate3DX(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте вокруг оси X
// на угол fi (при fi>0 против часовой стрелки) в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при повороте
// системы координат вокруг оси X на угол -fi при фиксированном положении объекта 
// fi - угол в градусах
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * M_PI;	// Перевод в радианы
	CMatrix RM(4, 4);
	RM(0, 0) = 1;
	RM(1, 1) = cos(ff);
	RM(1, 2) = -sin(ff);
	RM(2, 1) = sin(ff);
	RM(2, 2) = cos(ff);
	RM(3, 3) = 1;

	return RM;
}

//------------------------------------------------------------------------------

CMatrix CreateRotate3DY(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте вокруг оси Y
// на угол fi (при fi>0 против часовой стрелки) в фиксированной системе координат
// --- ИЛИ ---
// Формирует матрицу для преобразования координат объекта при повороте
// системы координат вокруг оси Y на угол -fi при фиксированном положении объекта 
// fi - угол в градусах
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * M_PI;	// Перевод в радианы
	CMatrix RM(4, 4);
	RM(0, 0) = cos(ff);
	RM(0, 2) = sin(ff);
	RM(1, 1) = 1;
	RM(2, 0) = -sin(ff);
	RM(2, 2) = cos(ff);
	RM(3, 3) = 1;

	return RM;
}

//------------------------------------------------------------------------------

CMatrix VectorMult(CMatrix& V1, CMatrix& V2)
// Вычисляет векторное произведение векторов V1 и V2
{
	int b1 = (V1.cols() == 1) && (V1.rows() == 3);
	int b2 = (V2.cols() == 1) && (V2.rows() == 3);
	int b = b1 && b2;

	if (!b)
	{
		TCHAR* error = _T("VectorMult: неправильные размерности векторов! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	CMatrix W(3);
	W(0) = V1(1) * V2(2) - V1(2) * V2(1);
	W(1) = -(V1(0) * V2(2) - V1(2) * V2(0));
	W(2) = V1(0) * V2(1) - V1(1) * V2(0);

	return W;
}

//------------------------------------------------------------------------------

double ScalarMult(CMatrix& V1, CMatrix& V2)
// Вычисляет скалярное произведение векторов V1 и V2
{
	int b1 = (V1.cols() == 1) && (V1.rows() == 3);
	int b2 = (V2.cols() == 1) && (V2.rows() == 3);
	int b = b1 && b2;

	if (!b)
	{
		TCHAR* error = _T("ScalarMult: неправильные размерности векторов! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	double p = V1(0) * V2(0) + V1(1) * V2(1) + V1(2) * V2(2);

	return p;
}

//------------------------------------------------------------------------------

double ModVec(CMatrix& V)
// Вычисляет модуль вектора V
{
	int b = (V.cols() == 1) && (V.rows() == 3);

	if (!b)
	{
		TCHAR* error = _T("ModVec: неправильная размерность вектора! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	double q = sqrt(V(0) * V(0) + V(1) * V(1) + V(2) * V(2));

	return q;
}

//------------------------------------------------------------------------------

double CosV1V2(CMatrix& V1, CMatrix& V2)
// Вычисляет косинус угла между векторами V1 и V2
{
	double modV1 = ModVec(V1);
	double modV2 = ModVec(V2);
	int b = (modV1 < 1e-7) || (modV2 < 1e-7);

	if (b)
	{
		TCHAR* error = _T("CosV1V2: модуль одного или обоих векторов < 1e-7! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	int b1 = (V1.cols() == 1) && (V1.rows() == 3);
	int b2 = (V2.cols() == 1) && (V2.rows() == 3);
	b = b1 && b2;

	if (!b)
	{
		TCHAR* error = _T("CosV1V2: неправильные размерности векторов! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	double cos_f = ScalarMult(V1, V2) / (modV1 * modV2);

	return cos_f;
}

//------------------------------------------------------------------------------

double AngleV1V2(CMatrix& V1, CMatrix& V2)
// Вычисляет угол между векторами V1 и V2 в градусах
{
	double modV1 = ModVec(V1);
	double modV2 = ModVec(V2);
	int b = (modV1 < 1e-7) || (modV2 < 1e-7);

	if (b)
	{
		TCHAR* error = _T("AngleV1V2: модуль одного или обоих векторов < 1e-7! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	int b1 = (V1.cols() == 1) && (V1.rows() == 3);
	int b2 = (V2.cols() == 1) && (V2.rows() == 3);
	b = b1 && b2;

	if (!b)
	{
		TCHAR* error = _T("AngleV1V2: неправильные размерности векторов! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	double cos_f = ScalarMult(V1, V2) / (modV1 * modV2);

	if (fabs(cos_f) > 1)
	{
		TCHAR* error = _T("AngleV1V2: модуль cos(f)>1! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		exit(1);
	}

	double f;

	if (cos_f > 0)
	{
		f = acos(cos_f);
	}
	else
	{
		f = M_PI - acos(cos_f);
	}

	double fg = (f / M_PI) * 180;

	return fg;
}

//------------------------------------------------------------------------------

CMatrix CreateViewCoord(double r, double fi, double q)
// Создает матрицу пересчета точки из мировой системы координат в видовую
// r - расстояние от начала координат до точки наблюдения
// fi - азимут (отсчет от оси X), град.
// q - угол (отсчет от оси Z), град.
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * M_PI;	// Перевод в радианы
	fg = fmod(q, 360.0);
	double qq = (fg / 180.0) * M_PI;	// Перевод в радианы
	CMatrix VM(4, 4);
	VM(0, 0) = -sin(ff);
	VM(0, 1) = cos(ff);
	VM(1, 0) = -cos(qq) * cos(ff);
	VM(1, 1) = -cos(qq) * sin(ff);
	VM(1, 2) = sin(qq);
	VM(2, 0) = -sin(qq) * cos(ff);
	VM(2, 1) = -sin(qq) * sin(ff);
	VM(2, 2) = -cos(qq);
	VM(2, 3) = r;
	VM(3, 3) = 1;

	return VM;
}

//------------------------------------------------------------------------------

CMatrix SphereToCart(CMatrix& PView)
// Преобразует сферические координаты точки PView в декартовы
// PView(0) - r
// PView(1) - fi - азимут (отсчет от оси X), град.
// PView(2) - q - угол (отсчет от оси Z), град.
// Результат: R(0) - x, R(1) - y, R(2) - z	
{
	CMatrix R(3);
	double r = PView(0);
	double fi = PView(1);					// Градусы
	double q = PView(2);					// Градусы
	double fi_rad = (fi / 180.0) * M_PI;	// Перевод fi в радианы
	double q_rad = (q / 180.0) * M_PI;		// Перевод q в радианы
	R(0) = r * sin(q_rad) * cos(fi_rad);	// X-координата точки наблюдения
	R(1) = r * sin(q_rad) * sin(fi_rad);	// Y-координата точки наблюдения
	R(2) = r * cos(q_rad);					// Z-координата точки наблюдения

	return R;
}

//------------------------------------------------------------------------------

void GetProjection(CRectD& RS, CMatrix& Data, CMatrix& PView, CRectD& PR)
// Вычисляет координаты проекции охватывающего фигуру параллелепипеда на 
// плоскость XY в видовой системе координат
// RS - область на плоскости XY, на которую опирается отображаемая поверхность
// Data - матрица данных
// PView - координаты точки наблюдения в мировой сферической системе координат
// PR - проекция
{
	double Zmax = Data.MaxElement();
	double Zmin = Data.MinElement();
	CMatrix PS(4, 4);	// Точки в мировом пространстве
	PS(3, 0) = 1;
	PS(3, 1) = 1;
	PS(3, 2) = 1;
	PS(3, 3) = 1;
	CMatrix MV = CreateViewCoord(PView(0), PView(1), PView(2));	// Матрица пересчета в видовую систему координат
	PS(0, 0) = RS.left;
	PS(1, 0) = RS.top;
	PS(2, 0) = Zmax;
	PS(0, 1) = RS.left;
	PS(1, 1) = RS.bottom;
	PS(2, 1) = Zmax;
	PS(0, 2) = RS.right;
	PS(1, 2) = RS.top;
	PS(2, 2) = Zmax;
	PS(0, 3) = RS.right;
	PS(1, 3) = RS.bottom;
	PS(2, 3) = Zmax;
	CMatrix Q = MV * PS;		// Координаты верхней плоскости параллелепипеда в видовой системе координат
	CMatrix V = Q.GetRow(0);	// Строка X-координат
	double Xmin = V.MinElement();
	double Xmax = V.MaxElement();
	V = Q.GetRow(1);			// Строка Y-координат
	double Ymax = V.MaxElement();
	PS(2, 0) = Zmin;
	PS(2, 1) = Zmin;
	PS(2, 2) = Zmin;
	PS(2, 3) = Zmin;
	Q = MV * PS;				// Координаты нижней плоскости параллелепипеда в видовой системе координат
	V = Q.GetRow(1);			// Строка Y-координат
	double Ymin = V.MinElement();
	PR.SetRectD(Xmin, Ymax, Xmax, Ymin);
}