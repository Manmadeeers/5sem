
// PyramidView.h: интерфейс класса CPyramidView
//

#pragma once


class CPyramidView : public CView
{
protected: // создать только из сериализации
	CPyramidView() noexcept;
	DECLARE_DYNCREATE(CPyramidView)

// Атрибуты
public:
	CPyramidDoc* GetDocument() const;
	CPyramid PIR;       // Усечённая пирамида
	CRect WinRect;      // Область в окне
	CMatrix PView;      // Точка наблюдения в сферической СК
	int Index;          // Индекс режима отображения


// Операции
public:

// Переопределение
public:
	virtual void OnDraw(CDC* pDC);  // переопределено для отрисовки этого представления
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Реализация
public:
	virtual ~CPyramidView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPyramidCurrent();        // Текущее положение камеры
	afx_msg void OnPyramidWithRemoval();    // С удалением невидимых граней
	afx_msg void OnPyramidWithoutRemoval(); // Без удаления невидимых граней
	afx_msg void OnPyramidSetCamera();      // Установка положения камеры по умолчанию
	afx_msg void OnCameraFromFile();        // Чтение координат из файла
	afx_msg void OnCameraFromDialog();      // Ввод координат через диалог
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // версия отладки в PyramidView.cpp
inline CPyramidDoc* CPyramidView::GetDocument() const
   { return reinterpret_cast<CPyramidDoc*>(m_pDocument); }
#endif

