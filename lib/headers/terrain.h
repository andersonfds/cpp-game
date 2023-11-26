#ifndef TERRAIN_H
#define TERRAIN_H

#ifndef OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>
#endif

#include <collision.h>

namespace terrain
{
    class terrain;
}

class terrain::terrain
{

public:
    void Create(int width, int height, int particleCount, int particleMin)
    {
        this->grassParticles = generateParticles(particleCount, particleMin, width, height);
        this->stoneParticles = generateParticles(particleCount, particleMin, width, height);
        this->dirtParticles = generateParticles(particleCount, particleMin, width, height);
    }

    void Update(olc::PixelGameEngine *pge, float fElapsedTime)
    {
        pge->Clear(olc::DARK_GREEN);
        DrawParticles(pge, grassParticles, olc::GREEN);
        DrawParticles(pge, stoneParticles, olc::DARK_GREY);
        DrawParticles(pge, dirtParticles, olc::VERY_DARK_YELLOW);
        DrawHoles(pge, olc::VERY_DARK_YELLOW);
    }

    void DrawParticles(olc::PixelGameEngine *pge, std::vector<olc::vi2d> particles, olc::Pixel color)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            bool isOdd = i % 2 == 0;
            if (isOdd)
                pge->Draw(particles[i], color);
            else
                pge->FillCircle(particles[i], i % 2, color);
        }
    }

    void DrawHoles(olc::PixelGameEngine *pge, olc::Pixel color)
    {
        for (int i = 0; i < this->holes.size(); i++)
        {
            olc::vi2d pos = {this->holes[i].x + 10, this->holes[i].y + 10};
            pge->FillCircle(pos, 10, olc::VERY_DARK_RED);
            pge->FillCircle(pos, 7, color);
        }
    }

    void AddHole(olc::vi2d pos)
    {
        collision::v4d *hole = new collision::v4d(pos.x - 10, pos.y - 10, 20, 20);

        if (CollidesWithHole(hole))
        {
            return;
        }

        this->holes.push_back(*hole);
    }

    bool HasHoles()
    {
        return this->holes.size() > 0;
    }

    bool CollidesWithHole(collision::v4d *box)
    {
        for (int i = 0; i < this->holes.size(); i++)
        {
            if (box->Intersects(&this->holes[i]))
            {
                return true;
            }
        }
        return false;
    }

private:
    std::vector<olc::vi2d> generateParticles(int size, int min, int w, int h)
    {
        std::vector<olc::vi2d> result;

        for (int i = 0; i < size; i++)
        {
            int x = rand() % (w - min + 1) + min;
            int y = rand() % (h - min + 1) + min;
            result.push_back({x, y});
        }
        return result;
    }

private:
    std::vector<olc::vi2d> grassParticles;
    std::vector<olc::vi2d> dirtParticles;
    std::vector<olc::vi2d> stoneParticles;
    std::vector<collision::v4d> holes = {};
    bool isDay = true;
};

#endif