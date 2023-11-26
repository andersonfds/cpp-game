#ifndef COLLISION_H
#define COLLISION_H

#ifndef OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>
#endif

namespace collision
{
    struct v4d;
}

struct collision::v4d
{
    v4d(int x, int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    int x;
    int y;
    int w;
    int h;

    bool Intersects(v4d *other)
    {
        if (this->x<other->x + other->w &&this->x + this->w> other->x &&
            this->y<other->y + other->h &&this->y + this->h> other->y)
        {
            return true;
        }
        return false;
    }
};

#endif