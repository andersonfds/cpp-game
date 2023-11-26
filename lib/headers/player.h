#ifndef PLAYER_H
#define PLAYER_H

#include <character.h>

namespace character
{
    class player;
}

std::map<std::string, std::vector<std::string>> definition = {
    {"idle", {"./gfx/lia1.png"}},
    {"walk", {"./gfx/lia0.png", "./gfx/lia2.png"}},
};

std::map<std::string, std::vector<std::string>> andersonDefinition = {
    {"idle", {"./gfx/and1.png", "./gfx/and2.png"}},
    {"dead", {"./gfx/and3.png"}},
};

std::map<std::string, std::vector<std::string>> shovelDefinition = {
    {"idle", {"./gfx/shovel.png"}},
};

std::map<std::string, std::vector<std::string>> dogDefinition = {
    {"idle", {"./gfx/dog0.png", "./gfx/dog1.png"}},
};

std::map<std::string, std::vector<std::string>> treeDefinition = {
    {"idle", {"./gfx/tree0.png"}},
};

class character::player : public character::character
{

public:
    player(std::string name) : character::character(new anim::seq(definition))
    {
        this->name = name;
        this->SetState("idle", 10, true);
    }

    player(std::string name, std::map<std::string, std::vector<std::string>> definition) : character::character(new anim::seq(definition))
    {
        this->name = name;
        this->SetState("idle", 10, true);
    }

    void EnableControls()
    {
        this->controlsEnabled = true;
    }

    void Draw(olc::PixelGameEngine *pge, float fElapsedTime, int scale = 1)
    {
        this->Update(fElapsedTime);
        Control(pge, fElapsedTime);

        olc::vf2d *pos = this->GetPosition();
        olc::Sprite *sprite = this->GetSprite();

        if (this->debug)
        {
            this->Debug(pge);

            pge->DrawString(pos->x, pos->y - 10, this->name, olc::RED);
        }

        pge->SetPixelMode(olc::Pixel::MASK);
        pge->DrawSprite(pos->x, pos->y, sprite, scale, this->isFacingLeft);
        pge->SetPixelMode(olc::Pixel::NORMAL);
    }

    void Control(olc::PixelGameEngine *pge, float fElapsedTime)
    {
        if (!controlsEnabled)
        {
            return;
        }

        if (pge->GetKey(olc::Key::LEFT).bHeld)
        {
            this->MoveLeft(fElapsedTime);
        }
        else if (pge->GetKey(olc::Key::RIGHT).bHeld)
        {
            this->MoveRight(fElapsedTime);
        }
        else if (pge->GetKey(olc::Key::UP).bHeld)
        {
            this->MoveUp(fElapsedTime);
        }
        else if (pge->GetKey(olc::Key::DOWN).bHeld)
        {
            this->MoveDown(fElapsedTime);
        }
        else
        {
            this->Idle();
        }
    }

public:
    void Walk()
    {
        character::character::Walk();
        this->SetState("walk", 10, false);
    }

    void Run()
    {
        character::character::Run();
        this->SetState("walk", 5, false);
    }

    void Idle()
    {
        this->SetState("idle", 10, false);
    }

private:
    void MoveLeft(float fElapsedTime)
    {
        this->Walk();
        this->SetPosition({this->GetPosition()->x - (this->GetSpeed() * fElapsedTime), this->GetPosition()->y});
        this->isFacingLeft = true;
    }

    void MoveRight(float fElapsedTime)
    {
        this->Walk();
        this->SetPosition({this->GetPosition()->x + (this->GetSpeed() * fElapsedTime), this->GetPosition()->y});
        this->isFacingLeft = false;
    }

    void MoveUp(float fElapsedTime)
    {
        this->Walk();
        this->SetPosition({this->GetPosition()->x, this->GetPosition()->y - (this->GetSpeed() * fElapsedTime)});
    }

    void MoveDown(float fElapsedTime)
    {
        this->Walk();
        this->SetPosition({this->GetPosition()->x, this->GetPosition()->y + (this->GetSpeed() * fElapsedTime)});
    }

public:
    bool debug = false;
    int XP = 0;
    int coins = 0;

    std::string GetName()
    {
        return this->name;
    }

private:
    std::string name;
    bool controlsEnabled = false;
};

#endif
