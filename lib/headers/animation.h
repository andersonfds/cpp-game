#ifndef ANIMATION_H
#define ANIMATION_H

#ifndef OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>
#endif

namespace anim
{
    class seq;
}

class anim::seq
{

public:
    seq(std::map<std::string, std::vector<std::string>> definition)
    {
        this->definition = definition;
        this->sprites = std::map<std::string, std::vector<std::unique_ptr<olc::Sprite>>>();
    }

    void Create()
    {
        if (this->created)
        {
            return;
        }

        for (auto const &[key, val] : this->definition)
        {
            std::vector<std::unique_ptr<olc::Sprite>> frames;
            for (auto const &frame : val)
            {
                std::unique_ptr<olc::Sprite> sprite;
                sprite = std::make_unique<olc::Sprite>(frame, this->pack);
                frames.push_back(std::move(sprite));
            }
            this->sprites[key] = std::move(frames);
        }

        UpdateFrame();
    }

    void SetFrameRate(int rate)
    {
        if (rate <= 0 || rate == this->frameRate)
        {
            return;
        }

        this->frameRate = rate;
    }

    void SetAnimation(std::string name)
    {
        if (this->current != nullptr && *this->current == name)
        {
            return;
        }

        this->current = std::make_unique<std::string>(name);
        this->currentFrame = 0;
        UpdateFrame();
    }

    void Update(float fElapsedTime)
    {
        if (animating == false)
        {
            return;
        }

        this->frameRateAccumulator += fElapsedTime;
        if (this->frameRateAccumulator >= 1.0f / this->frameRate)
        {
            this->frameRateAccumulator -= 1.0f / this->frameRate;
            this->currentFrame++;
            if (this->currentFrame >= this->sprites[*this->current].size())
            {
                this->currentFrame = 0;
            }
            UpdateFrame();
        }
    }

    olc::Sprite *GetFrame()
    {
        if (this->current == nullptr)
        {
            return nullptr;
        }

        if (this->frame == nullptr)
        {
            UpdateFrame();
        }

        return this->frame.get();
    }

    void SetPack(olc::ResourcePack *pack)
    {
        this->pack = pack;
    }

    bool HasPack()
    {
        return this->pack != nullptr;
    }

private:
    void UpdateFrame()
    {
        if (this->current == nullptr || this->current->empty() || this->sprites[*this->current].empty())
        {
            return;
        }

        auto &sprite = this->sprites[*this->current][this->currentFrame];
        this->frame = std::make_unique<olc::Sprite>(sprite->width, sprite->height);
        for (int y = 0; y < sprite->height; y++)
        {
            for (int x = 0; x < sprite->width; x++)
            {
                olc::Pixel pixel = sprite->GetPixel(x, y);
                this->frame->SetPixel(x, y, pixel);
            }
        }
    }

    olc::vi2d GetSize()
    {
        if (this->current == nullptr || this->current->empty() || this->sprites[*this->current].empty())
        {
            return olc::vi2d(0, 0);
        }

        auto &sprite = this->sprites[*this->current][this->currentFrame];
        return olc::vi2d(sprite->width, sprite->height);
    }

public:
    bool animating = true;

private:
    std::map<std::string, std::vector<std::string>> definition;
    std::map<std::string, std::vector<std::unique_ptr<olc::Sprite>>> sprites;
    std::unique_ptr<std::string> current;
    std::unique_ptr<olc::Sprite> frame;
    float frameRateAccumulator = 0.0f;
    int currentFrame = 0;
    int frameRate = 60;
    bool created = false;
    olc::ResourcePack *pack;
};

#endif