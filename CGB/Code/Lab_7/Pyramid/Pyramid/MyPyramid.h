#pragma once
#include "pch.h"

class CPyramid
{
private:
    CMatrix Vertices;      // Координаты вершин (6 вершин: 3 нижних + 3 верхних)
    CMatrix NormalBottom;  // Вектор внешней нормали к нижнему основанию
    CMatrix NormalTop;     // Вектор внешней нормали к верхнему основанию

    void GetRect(CMatrix& Vert, CRectD& RectView);

public:
    CPyramid();
    void Draw(CDC& dc, CMatrix& PView, CRect& RW);           // Без удаления невидимых граней
    void Draw1(CDC& dc, CMatrix& PView, CRect& RW);          // С удалением невидимых граней
    void DrawCurrent(CDC& dc, CMatrix& PView, CRect& RW);    // Текущее положение камеры
    void DrawCamera(CDC& dc, CMatrix& PView, CRect& RW);     // Положение камеры по умолчанию
};