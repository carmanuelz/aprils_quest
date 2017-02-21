#include "AStarFinder.h"
#include "Urho3D/IO/File.h"
#include "Urho3D/Graphics/DebugRenderer.h"

AStarFinder::AStarFinder(Context* context): Component(context)
{

}

void AStarFinder::RegisterObject(Context* context)
{
	context->RegisterFactory<AStarFinder>();
}

void AStarFinder::LoadMap(int** blockgrid)
{
    width = 40;
    height = 50;
    tilesize = 0.5;


    Grid = new Nodo**[height];
    for (int i = 0; i < height; ++i)
    {
        Grid[i] = new Nodo*[width];
        for(int j = 0 ; j < width; j++) {
            Grid[i][j] = new Nodo;
        }
    }

    for (int j = 0; j < height; j++ )
    {
        for (int k = 0; k < width; k++)
        {
            Grid[j][k]->x = k;
            Grid[j][k]->y = j;
            if (!blockgrid[j][k]) {
                Grid[j][k]->walkable = false;
            }
        }
    }
    
    for (int j = height-1; j >=0 ; j-- )
    {
        for (int k = 0; k < width; k++)
        {
            if (blockgrid[j][k]) {
                std::cout<<" ";
            } else {
                std::cout<<"O";
            }
        }
        std::cout<<std::endl;
    }
}

void AStarFinder::drawdebug()
{
    DebugRenderer* debug = GetComponent<DebugRenderer>();
    for(int i = 0; i < width; i++)
    {
        debug->AddLine( Vector3(i*tilesize, 0,0),
                        Vector3(i*tilesize, width*tilesize, 0),
                        Color(0, 1, 1, 1),
                        false );
    }

    for(int j = 0; j < height; j++)
    {
        debug->AddLine( Vector3(0, j*tilesize,0),
                        Vector3(height*tilesize, j*tilesize, 0),
                        Color(0, 1, 1, 1),
                        false );
    }
}

bool AStarFinder::isWalkableAt(int x, int y)
{
    return isInside(x, y) && Grid[y][x]->walkable;
};

bool AStarFinder::isInside(int x, int y)
{
    return (x >= 0 && x < width) && (y >= 0 && y < height);
};

void AStarFinder::setWalkableAt(int x, int y, bool walkable)
{
    Grid[y][x]->walkable = walkable;
};

/**
 * Get the neighbors of the given node.
 *
 *     offsets      diagonalOffsets:
 *  +---+---+---+    +---+---+---+
 *  |   | 0 |   |    | 0 |   | 1 |
 *  +---+---+---+    +---+---+---+
 *  | 3 |   | 1 |    |   |   |   |
 *  +---+---+---+    +---+---+---+
 *  |   | 2 |   |    | 3 |   | 2 |
 *  +---+---+---+    +---+---+---+
 *
 */
Vector<Nodo*> AStarFinder::getNeighbors(Nodo* node)
{
    int x, y;
    bool s0 = false, d0 = false,
         s1 = false, d1 = false,
         s2 = false, d2 = false,
         s3 = false, d3 = false;
    x = node->x;
    y = node->y;
    Vector<Nodo*> neighbors;

    if (isWalkableAt(x,y-1))
    {
        neighbors.Push(Grid[y-1][x]);
        s0 = true;
    }

    if (isWalkableAt(x+1,y))
    {
        neighbors.Push(Grid[y][x+1]);
        s1 = true;
    }

    if (isWalkableAt(x,y+1))
    {
        neighbors.Push(Grid[y+1][x]);
        s2 = true;
    }

    if (isWalkableAt(x-1,y))
    {
        neighbors.Push(Grid[y][x-1]);
        s3 = true;
    }

    d0 = s3 && s0;
    d1 = s0 && s1;
    d2 = s1 && s2;
    d3 = s2 && s3;

    if (d0 && isWalkableAt(x-1,y-1))
        neighbors.Push(Grid[y - 1][x - 1]);

    if (d1 && isWalkableAt(x+1,y-1))
        neighbors.Push(Grid[y - 1][x + 1]);

    if (d2 && isWalkableAt(x+1,y+1))
        neighbors.Push(Grid[y + 1][x + 1]);

    if (d3 && isWalkableAt(x-1,y+1))
        neighbors.Push(Grid[y + 1][x - 1]);

    return neighbors;
}

bool AStarFinder::compareNode(Nodo* A, Nodo* B)
{
    return A->x == B->x && A->y == B->y;
}

Vector<IntVector2*> AStarFinder::backtrace(Nodo* node)
{
    IntVector2* currentnode = new IntVector2(node->parent->x_,node->parent->y_);
    Vector<IntVector2*> btrace;
    btrace.Push(new IntVector2(node->x,node->y));
    while(currentnode->x_ != -1 && currentnode->y_ != -1)
    {
        btrace.Push(currentnode);
        currentnode = new IntVector2(Grid[currentnode->y_][currentnode->x_]->parent->x_,Grid[currentnode->y_][currentnode->x_]->parent->y_);
    }
    return btrace;
}

Vector<IntVector2*> AStarFinder::findPath(IntVector2 Pstart, IntVector2 Pend)
{
    return findPath(Pstart.x_, Pstart.y_, Pend.x_, Pend.y_);
}

bool AStarFinder::compareByLF(const Nodo* a, const Nodo* b)
{
    return a->f > b->f;
}

Vector<IntVector2*> AStarFinder::findPath(int startX, int startY, int endX, int endY)
{
    resetGrid();
    std::vector<Nodo*> OpenList;
    Nodo* StartNode = Grid[startY][startX];
    Nodo* EndNode = Grid[endY][endX];

    StartNode->g = 0;
    StartNode->f = 0;

    OpenList.push_back(StartNode);
    StartNode->opened = true;
    Nodo *node;
    while(!OpenList.empty())
    {
        std::sort(OpenList.begin(),OpenList.end(),compareByLF);
        node = OpenList.back();
        OpenList.pop_back();
        node->closed = true;

        if(compareNode(node,EndNode))
        {
            break;
        }

        Vector<Nodo*> Vecinos = getNeighbors(node);
        for ( int i=0; i < Vecinos.Size() ; i++)
        {
            Nodo* neighbor = Vecinos[i];
            if (neighbor->closed)
                continue;
            int x = neighbor->x, y = neighbor->y, dif = 0;
            if((x - node->x == 0 || y - node->y == 0))
                dif = 10;
            else
                dif = 14;
            float ng = node->g + dif;

            if (!neighbor->opened || ng < neighbor->g)
            {
                neighbor->g = ng;
                neighbor->parent->x_ = node->x;
                neighbor->parent->y_ = node->y;
                if (!neighbor->opened)
                {
                    neighbor->h = 10 * (abs(x - endX)+abs(y - endY));
                    OpenList.push_back(neighbor);
                    neighbor->opened = true;
                }
                neighbor->f = neighbor->g + neighbor->h;
            }
        }
    }
    //return smoothenPath(backtrace(node));
    return backtrace(node);
}

Vector<IntVector2*> AStarFinder::getLine(int x0, int y0, int x1, int y1)
{
    int sx, sy, dx, dy, err, e2;
    Vector<IntVector2*> line;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);

    if(x0 < x1)
        sx = 1;
    else
        sx =-1;

    if(y0 < y1)
        sy = 1;
    else
        sy =-1;

    err = dx - dy;

    while (true)
    {
        line.Push(new IntVector2(x0, y0));
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = 2 * err;
        if (e2 > -dy)
        {
            err = err - dy;
            x0 = x0 + sx;
        }
        if (e2 < dx)
        {
            err = err + dx;
            y0 = y0 + sy;
        }
    }
    return line;
}
Vector<IntVector2*> AStarFinder::smoothenPath(Vector<IntVector2*> path)
{
    Vector<IntVector2*> newPath;
    Vector<IntVector2*> line;
    int len = path.Size();
    if(len>1)
    {
        int x0 = path[0]->x_,        // path start x
            y0 = path[0]->y_,        // path start y
            x1 = path[len - 1]->x_,  // path end x
            y1 = path[len - 1]->y_,  // path end y
            sx, sy,                 // current start coordinate
            ex, ey,                 // current end coordinate
            lx, ly;
        bool blocked;

        sx = x0;
        sy = y0;
        lx = path[1]->x_;
        ly = path[1]->y_;
        newPath.Push(new IntVector2(sx, sy));

        for (int i = 2; i < len; ++i)
        {
            IntVector2* coord = new IntVector2(path[i]->x_,path[i]->y_);
            ex = coord->x_;
            ey = coord->y_;
            line = getLine(sx, sy, ex, ey);
            blocked = false;
            for (int j = 1; j < line.Size(); ++j)
            {
                if (!isWalkableAt(line[j]->x_, line[j]->y_) ||
                    ((line[j]->x_ > line[j-1]->x_ && line[j]->y_ > line[j-1]->y_ && (!isWalkableAt(line[j]->x_, line[j]->y_-1) || !isWalkableAt(line[j]->x_-1, line[j]->y_)))||
                     (line[j]->x_ < line[j-1]->x_ && line[j]->y_ > line[j-1]->y_ && (!isWalkableAt(line[j]->x_, line[j]->y_-1) || !isWalkableAt(line[j]->x_+1, line[j]->y_)))||
                     (line[j]->x_ < line[j-1]->x_ && line[j]->y_ < line[j-1]->y_ && (!isWalkableAt(line[j]->x_+1, line[j]->y_) || !isWalkableAt(line[j]->x_, line[j]->y_+1)))||
                     (line[j]->x_ > line[j-1]->x_ && line[j]->y_ < line[j-1]->y_ && (!isWalkableAt(line[j]->x_, line[j]->y_+1) || !isWalkableAt(line[j]->x_-1, line[j]->y_)))))
                {
                    blocked = true;
                    newPath.Push(new IntVector2(lx, ly));
                    sx = lx;
                    sy = ly;
                    break;
                }
            }
            if (!blocked)
            {
                lx = ex;
                ly = ey;
            }
        }
        newPath.Push(new IntVector2(x1, y1));
    }
    return newPath;
}

void AStarFinder::resetGrid()
{
    for (int j = 0; j < height; j++ )
    {
        for (int k = 0; k < width; k++)
        {
            Grid[j][k]->reset();
        }
    }
}


