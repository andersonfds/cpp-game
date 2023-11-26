#ifndef CHARACTER_H

#ifndef OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>
#endif
#include <animation.h>
#include <collision.h>

namespace character
{
    class character;
}

class character::character
{

public:
    character(anim::seq *animation)
    {
        this->animation = animation;
    }

public:
    void SetState(std::string state, int frameRate = 10, bool recomputeBoundingBox = false)
    {
        this->animation->SetAnimation(state);
        this->animation->SetFrameRate(frameRate);
        this->animation->Create();

        this->box->w = this->animation->GetFrame()->width;
        this->box->h = this->animation->GetFrame()->height;

        this->animation->animating = true;
    }

    void Damage(int amount)
    {
        this->health -= amount;
        if (this->health <= 0)
        {
            this->health = 0;
            TakeLife();
        }
    }

    void TakeLife()
    {
        if (this->lives > 0)
        {
            this->lives--;
            this->health = 100;
        }
    }

    olc::Sprite *GetSprite()
    {
        olc::Sprite *sprite = this->animation->GetFrame();
        return sprite;
    }

    void Debug(olc::PixelGameEngine *pge)
    {
        pge->DrawRect(this->box->x, this->box->y, this->box->w, this->box->h, olc::RED);
    }

    void Kill(std::string state = "dead")
    {
        this->lives = 0;
        this->health = 0;
        this->SetState(state);
        this->animation->animating = false;
        this->speed = 0.0f;
    }

    bool IsDead()
    {
        return this->lives <= 0;
    }

    void Update(float fElapsedTime)
    {
        this->animation->Update(fElapsedTime);
    }

    void Walk()
    {
        if (this->speed != 50.0f)
            this->speed = 50.0f;
    }

    void Run()
    {
        if (this->speed != 100.0f)
            this->speed = 100.0f;
    }

    void Stop()
    {
        if (this->speed != 0.0f)
            this->speed = 0.0f;
    }

    float GetSpeed()
    {
        return this->speed;
    }

    float SetPosition(olc::vf2d position = {0.0f, 0.0f})
    {
        this->position->x = position.x;
        this->position->y = position.y;

        this->box->x = this->position->x;
        this->box->y = this->position->y;

        return this->position->x;
    }

    olc::vf2d *GetPosition()
    {
        return this->position;
    }

    int GetLives()
    {
        return this->lives;
    }

    void SetPack(olc::ResourcePack *pack)
    {
        this->animation->SetPack(pack);
    }

public:
    collision::v4d *box = new collision::v4d(0, 0, 0, 0);
    bool isFacingLeft = false;

private:
    olc::vf2d *position = new olc::vf2d(0, 0);
    int health = 100;
    int lives = 3;
    float speed = 0.0f;
    anim::seq *animation;
};

#endif