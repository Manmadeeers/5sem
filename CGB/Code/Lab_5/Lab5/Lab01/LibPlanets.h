#ifndef LIBPLANETS
#define LIBPLANETS 1
const double pi = 3.14159;


struct CSizeD
{
	double cx;
	double cy;
};
//-------------------------------------------------------------------------------
struct CRectD
{
	double left;
	double top;
	double right;
	double bottom;
	CRectD() { left = top = right = bottom = 0; };
	CRectD(double l, double t, double r, double b);
	void SetRectD(double l, double t, double r, double b);
	CSizeD SizeD();		// Возвращает размеры(ширина, высота) прямоугольника 
};
//-------------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy);
CMatrix CreateRotate2D(double fi);
CMatrix SpaceToWindow(CRectD& rs, CRect& rw);
void SetMyMode(CDC& dc, CRectD& RS, CRect& RW);



class CBlade
{
	CRect MainPoint;
	CRect FirstTop;
	CRect SecondTop;
	CRect FirstBootom;
	CRect SecondBootom;
	CRect EarthOrbit;
	CMatrix FTCoords;
	CMatrix STCoords;
	CMatrix FBCoords;
	CMatrix SBCoords;
	CRect RW;		   // Прямоугольник в окне
	CRectD RS;		   // Прямоугольник области в МСК
	double wPoint=10;		//угловая скорость
	//углы для 4 точек (вершин лопастей)
	double fiSB;
	double fiFB;
	double fiST;
	double fiFT;

	double dt;		   // Интервал дискретизации, сек.
public:
	CBlade();
	//void DrawTriangle(CMatrix FTCoords, CMatrix STCoords, CDC& dc);
	void SetDT(double dtx) { dt = dtx; };	// Установка интервала дискретизации
	void SetNewCoords();					// Вычисляет новые координаты планет
	void GetRS(CRectD& RSX);				// Возвращает область графика в мировой СК
	CRect GetRW() { return RW; };			// Возвращает область графика в окне	
	void Draw(CDC& dc);						// Рисование без самостоятельного пересчета координат
	void DrawBladePair(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color, CMatrix& transformMatrix);
	void DrawTriangleWithBorder(CMatrix FCoords, CMatrix SCoords, CDC& dc, COLORREF color, CMatrix& transformMatrix);
	void DrawBlade(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color);

private:
	CBrush RedBrush;
	CBrush BlueBrush;
	CBrush SunBrush;
	CMatrix BlueCoords1;  // Правая синяя
	CMatrix BlueCoords2;  // Левая синяя
	CMatrix BlueCoords3;  // Верхняя синяя
	CMatrix BlueCoords4;  // Нижняя синяя
	double fiLeft = 180;    // Левая (синяя)
	double fiRight = 0;     // Правая (синяя)
	double fiBlue1 = 0;     // Правая
	double fiBlue2 = 180;   // Левая
	double fiBlue3 = 90;    // Верхняя (дополнительная)
	double fiBlue4 = 270;   // Нижняя (дополнительная)
	CRect LeftBlade;        // Левая лопасть
	CRect RightBlade;       // Правая лопасть
	CMatrix LBCoords;       // Координаты левой лопасти
	CMatrix RBCoords;       // Координаты правой лопасти
};


#endif

