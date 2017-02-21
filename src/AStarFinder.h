#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <math.h>
#include "Urho3D/Core/Object.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Resource/JSONValue.h"

using namespace Urho3D;

struct Nodo
{
    bool walkable = true;
    bool closed = false, opened = false;
    int x = 0, y = 0, f = 0, g = 0, h = 0;
    IntVector2 *parent = new IntVector2(-1,-1);

    void reset()
    {
        closed = false;
        opened = false;
        f = 0;
        g = 0;
        h = 0;
        parent->x_ = -1;
        parent->y_ = -1;
    }
};

class AStarFinder : public Component
{
URHO3D_OBJECT(AStarFinder, Component);
public:
    AStarFinder(Context* context);
    static void RegisterObject(Context* context);
    void LoadMap(int **blockgrid);
    Vector<IntVector2*> findPath(IntVector2 Pstart, IntVector2 Pend);
    Vector<IntVector2*> findPath(int startX, int startY, int endX, int endY);
    bool isWalkableAt(int x,int y);
    void drawdebug();
    float tilesize;
private:
    int width;
    int height;
    int **blockgrid;
    Nodo ***Grid;
    Vector<Nodo*> getNeighbors(Nodo* node);
    Vector<IntVector2*> backtrace(Nodo* node);
    bool isInside(int x, int y);
    void setWalkableAt(int x, int y,bool walkable);
    bool compareNode(Nodo* A, Nodo* B);
    static bool compareByLF(const Nodo* a,const Nodo* b);
    Vector<IntVector2*> getLine(int x0, int y0, int x1, int y1);
    Vector<IntVector2*> smoothenPath(Vector<IntVector2*> path);
    void resetGrid();
};
