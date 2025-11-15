#include "pch.h"

////////////////////////// Конструктор ///////////////////////////////////
CPyramid::CPyramid()
{
    // 6 вершин: A, B, C (нижнее основание), A1, B1, C1 (верхнее основание)
    Vertices.RedimMatrix(4, 6);
    NormalBottom.RedimMatrix(3);
    NormalTop.RedimMatrix(3);

    // Нижнее основание (треугольник ABC)
    // ВАЖНО: обход ПРОТИВ часовой стрелки при виде СВЕРХУ (снаружи)
    // чтобы нормаль смотрела вниз (наружу)
    Vertices(0, 0) = 15;   // A: x=15, y=0, z=0
    Vertices(1, 0) = 0;
    Vertices(2, 0) = 0;

    Vertices(0, 1) = -7.5;  // B: x=-7.5, y=13, z=0
    Vertices(1, 1) = 13;    // Поменяли местами B и C!
    Vertices(2, 1) = 0;

    Vertices(0, 2) = -7.5;  // C: x=-7.5, y=-13, z=0
    Vertices(1, 2) = -13;
    Vertices(2, 2) = 0;

    // Верхнее основание (треугольник A1B1C1)
    // Обход ПРОТИВ часовой стрелки при виде сверху
    Vertices(0, 3) = 7.5;   // A1: x=7.5, y=0, z=20
    Vertices(1, 3) = 0;
    Vertices(2, 3) = 20;

    Vertices(0, 4) = -3.75;  // B1: x=-3.75, y=6.5, z=20
    Vertices(1, 4) = 6.5;    // Поменяли местами B1 и C1!
    Vertices(2, 4) = 20;

    Vertices(0, 5) = -3.75;  // C1: x=-3.75, y=-6.5, z=20
    Vertices(1, 5) = -6.5;
    Vertices(2, 5) = 20;

    // Однородные координаты
    for (int i = 0; i < 6; i++)
        Vertices(3, i) = 1;

    // Нормали к основаниям (направлены НАРУЖУ)
    NormalBottom(0) = 0;  NormalBottom(1) = 0;  NormalBottom(2) = -1; // Вниз
    NormalTop(0) = 0;     NormalTop(1) = 0;     NormalTop(2) = 1;     // Вверх
}

////////////////////////// Draw(...) ///////////////////////////////
void CPyramid::Draw(CDC& dc, CMatrix& PView, CRect& RW)
{
    CMatrix ViewCart = SphereToCart(PView);
    CMatrix MV = CreateViewCoord(PView(0), PView(1), PView(2));
    CMatrix ViewVert = MV * Vertices;

    CRectD RectView;
    GetRect(ViewVert, RectView);
    CMatrix MW = SpaceToWindow(RectView, RW);

    CPoint MasVert[6];
    CMatrix V(3);
    V(2) = 1;

    for (int i = 0; i < 6; i++)
    {
        V(0) = ViewVert(0, i);
        V(1) = ViewVert(1, i);
        V = MW * V;
        MasVert[i].x = (int)V(0);
        MasVert[i].y = (int)V(1);
    }

    double smBottom = ScalarMult(NormalBottom, ViewCart);
    double smTop = ScalarMult(NormalTop, ViewCart);

    // Проверка видимости боковых граней
    bool sideVisible[3];
    for (int i = 0; i < 3; i++)
    {
        int next = (i + 1) % 3;

        CMatrix V1 = Vertices.GetCol(i, 0, 2);
        CMatrix V2 = Vertices.GetCol(next, 0, 2);
        CMatrix V3 = Vertices.GetCol(i + 3, 0, 2);

        CMatrix Edge1 = V2 - V1;
        CMatrix Edge2 = V3 - V1;
        CMatrix Normal = VectorMult(Edge1, Edge2);

        double sm = ScalarMult(Normal, ViewCart);
        sideVisible[i] = (sm > 0);
    }

    CPen PenSolid(PS_SOLID, 2, RGB(0, 0, 0));
    CPen PenDashed(PS_DASH, 1, RGB(128, 128, 128));
    CPen* pOldPen;

    // Рисуем рёбра нижнего основания
    for (int i = 0; i < 3; i++)
    {
        int next = (i + 1) % 3;

        bool edgeVisible = (smBottom > 0) || sideVisible[i];

        pOldPen = dc.SelectObject(edgeVisible ? &PenSolid : &PenDashed); /// &PenDashed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        dc.MoveTo(MasVert[i]);
        dc.LineTo(MasVert[next]);
        dc.SelectObject(pOldPen);
    }

    // Рисуем рёбра верхнего основания
    for (int i = 0; i < 3; i++)
    {
        int next = (i + 1) % 3;

        bool edgeVisible = (smTop > 0) || sideVisible[i];

        pOldPen = dc.SelectObject(edgeVisible ? &PenSolid : &PenDashed); /// &PenDashed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        dc.MoveTo(MasVert[i + 3]);
        dc.LineTo(MasVert[next + 3]);
        dc.SelectObject(pOldPen);
    }

    // Рисуем боковые рёбра (вертикальные)
    for (int i = 0; i < 3; i++)
    {
        int prev = (i + 2) % 3;
        bool edgeVisible = sideVisible[i] || sideVisible[prev];

        pOldPen = dc.SelectObject(edgeVisible ? &PenSolid : &PenDashed); /// &PenDashed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        dc.MoveTo(MasVert[i]);
        dc.LineTo(MasVert[i + 3]);
        dc.SelectObject(pOldPen);
    }
}

////////////////////////// Draw1(...) /////////////////////////////// удаление граней
void CPyramid::Draw1(CDC& dc, CMatrix& PView, CRect& RW)
{
    CMatrix ViewCart = SphereToCart(PView);
    CMatrix MV = CreateViewCoord(PView(0), PView(1), PView(2));
    CMatrix ViewVert = MV * Vertices;

    CRectD RectView;
    GetRect(ViewVert, RectView);
    CMatrix MW = SpaceToWindow(RectView, RW);

    CPoint MasVert[6];
    CMatrix V(3);
    V(2) = 1;

    for (int i = 0; i < 6; i++)
    {
        V(0) = ViewVert(0, i);
        V(1) = ViewVert(1, i);
        V = MW * V;
        MasVert[i].x = (int)V(0);
        MasVert[i].y = (int)V(1);
    }

    // Структура для хранения информации о гранях
    struct FaceInfo {
        int index;
        double depth;
        bool visible;
    };

    FaceInfo faces[3];

    // Проверка видимости боковых граней
    for (int i = 0; i < 3; i++)
    {
        int next = (i + 1) % 3;

        // Боковая грань: нижняя вершина i -> следующая нижняя next -> 
        // -> верхняя next+3 -> верхняя i+3
        CMatrix V1 = Vertices.GetCol(i, 0, 2);        // Нижняя i
        CMatrix V2 = Vertices.GetCol(next, 0, 2);     // Нижняя next
        CMatrix V3 = Vertices.GetCol(i + 3, 0, 2);    // Верхняя i+3

        CMatrix Edge1 = V2 - V1;  // Вектор вдоль нижнего ребра
        CMatrix Edge2 = V3 - V1;  // Вектор вверх к верхней вершине
        CMatrix Normal = VectorMult(Edge1, Edge2);

        double sm = ScalarMult(Normal, ViewCart);

        double avgZ = (ViewVert(2, i) + ViewVert(2, next) +
            ViewVert(2, next + 3) + ViewVert(2, i + 3)) / 4.0;

        faces[i].index = i;
        faces[i].depth = avgZ;
        faces[i].visible = (sm > 0);
    }

    // Сортировка граней по глубине
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (faces[i].depth > faces[j].depth) {
                FaceInfo temp = faces[i];
                faces[i] = faces[j];
                faces[j] = temp;
            }
        }
    }

    // Проверка видимости оснований
    double smBottom = ScalarMult(NormalBottom, ViewCart);
    double smTop = ScalarMult(NormalTop, ViewCart);

    CPen Pen(PS_SOLID, 2, RGB(0, 0, 0));
    CPen* pOldPen = dc.SelectObject(&Pen);

    // Рисуем ТОЛЬКО видимые грани с заливкой

    // Нижнее основание
    if (smBottom > 0)
    {
        CBrush BrushBottom(RGB(90, 110, 130));
        CBrush* pOldBrush = dc.SelectObject(&BrushBottom);
        CPoint BottomVert[3] = { MasVert[0], MasVert[1], MasVert[2] };
        dc.Polygon(BottomVert, 3);
        dc.SelectObject(pOldBrush);
    }

    // Верхнее основание
    if (smTop > 0)
    {
        CBrush BrushTop(RGB(180, 220, 180));
        CBrush* pOldBrush = dc.SelectObject(&BrushTop);
        CPoint TopVert[3] = { MasVert[3], MasVert[4], MasVert[5] };
        dc.Polygon(TopVert, 3);
        dc.SelectObject(pOldBrush);
    }

    // Боковые грани - рисуем от дальних к ближним
    for (int f = 0; f < 3; f++)
    {
        int i = faces[f].index;

        // Рисуем ТОЛЬКО если грань видима
        if (faces[f].visible)
        {
            int next = (i + 1) % 3;

            CBrush Brush(RGB(160, 140, 190));
            CBrush* pOldBrush = dc.SelectObject(&Brush);

            CPoint FaceVert[4];
            FaceVert[0] = MasVert[i];
            FaceVert[1] = MasVert[next];
            FaceVert[2] = MasVert[next + 3];
            FaceVert[3] = MasVert[i + 3];

            dc.Polygon(FaceVert, 4);
            dc.SelectObject(pOldBrush);
        }
    }

    dc.SelectObject(pOldPen);
}

////////////////////////// DrawCurrent(...) ///////////////////////////////
void CPyramid::DrawCurrent(CDC& dc, CMatrix& PView, CRect& RW)
{
    Draw1(dc, PView, RW);
}

////////////////////////// DrawCamera(...) ///////////////////////////////
void CPyramid::DrawCamera(CDC& dc, CMatrix& PView, CRect& RW)
{
    Draw1(dc, PView, RW);
}

////////////////////////// GetRect(...) ///////////////////////////
void CPyramid::GetRect(CMatrix& Vert, CRectD& RectView)
{
    CMatrix V = Vert.GetRow(0);
    double xMin = V.MinElement();
    double xMax = V.MaxElement();

    V = Vert.GetRow(1);
    double yMin = V.MinElement();
    double yMax = V.MaxElement();

    RectView.SetRectD(xMin, yMax, xMax, yMin);
}