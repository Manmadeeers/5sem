
// MyProt2DView.h: интерфейс класса CMyProt2DView
//

#pragma once
enum class ViewMode {None, GraphF1, GraphF3, Shapes};

class CMyProt2DView : public CView
{
protected: // создать только из сериализации
	CMyProt2DView() noexcept;
	DECLARE_DYNCREATE(CMyProt2DView)

// Атрибуты
public:
	int Index;
	int N_Bezier;
	CMatrix X, Y;
	CRect RW;	// Прямоугольник в окне
	CRectD RS;	// Прямоугольник области в МСК
	CPlot2D Graph;	// график
	CMyPen PenLine, // Перо для линий
	PenAxis;	// Перо для осей
	bool m_ShowShapes = false;

// Операции
public:
	double MyF1(double x);
	double MyF2(double x);



// Переопределение
public:
	virtual void OnDraw(CDC* pDC);  // переопределено для отрисовки этого представления
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Реализация
public:
	virtual ~CMyProt2DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	void OnTestsF1();
	void OnTestsF2();
	void OnTestsF3();

	DECLARE_MESSAGE_MAP()
};