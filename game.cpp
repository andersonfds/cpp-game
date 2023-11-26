#define OLC_PGE_APPLICATION
#include <cassert>
#include <olcPixelGameEngine.h>
#include <player.h>

class Animation
{
public:
    Animation(olc::vi2d size, std::vector<std::string> sprites, float speed)
    {
        assert(sprites.size() > 0);
        mFrameRate = 1 / speed;
        mSpriteSize = size;
        mSize = sprites.size();
        mSprites = std::vector<std::unique_ptr<olc::Sprite>>(mSize);

        for (int i = 0; i < sprites.size(); i++)
        {
            mSprites[i] = std::make_unique<olc::Sprite>(sprites[i]);
        }
    }

public:
    olc::vi2d mSpriteSize;

private:
    std::vector<std::unique_ptr<olc::Sprite>> mSprites;
    float mFrameRate;
    float mAccumulator = 0.0f;
    int mIndex = 0;
    int mSize = 0;
    bool mAnimate = true;

public:
    olc::Sprite *GetSprite()
    {
        return mSprites[mIndex].get();
    }

    void SetAnimate(bool animate)
    {
        mAnimate = animate;
    }

    void SetFrame(int index)
    {
        mIndex = index;
    }

    void Update(float fElapsedTime)
    {
        if (!mAnimate)
            return;

        mAccumulator += fElapsedTime;
        if (mAccumulator >= mFrameRate)
        {
            mAccumulator -= mFrameRate;
            mIndex = (mIndex + 1) % mSize;
        }
    }
};

class Person
{
public:
    Person(olc::vi2d size, std::vector<std::string> sprites, float speed, olc::vf2d position)
    {
        assert(sprites.size() > 0);
        mAnimation = new Animation(size, sprites, speed);
        mPosition = position;
    }

    void Draw(olc::PixelGameEngine *pge, float fElapsedTime)
    {
        mAnimation->Update(fElapsedTime);
        if (!isDead)
        {
            ComputeBoundingBox();
        }
        pge->DrawSprite(mPosition, mAnimation->GetSprite(), 1, mIsLeft);
    }

    bool IsHit(olc::vf2d topLeft, olc::vf2d size)
    {
        olc::vf2d position = GetPosition();
        olc::vf2d size2 = GetSize();

        if (position.x < topLeft.x + size.x &&
            position.x + size2.x > topLeft.x &&
            position.y < topLeft.y + size.y &&
            position.y + size2.y > topLeft.y)
        {
            return true;
        }

        return false;
    }

    void Die(std::string sprite)
    {
        isDead = true;
        mAnimation = new Animation(olc::vi2d(16, 16), {sprite}, 1.0f);
        mAnimation->SetAnimate(false);
        computedBoundingBox = olc::vi2d(0, 0);
    }

    void GetShape(olc::PixelGameEngine *pge)
    {
        pge->DrawRect(GetPosition(), GetSize(), olc::RED);
    }

    olc::vi2d GetPosition()
    {
        olc::vi2d topLeftC = olc::vi2d(mPosition.x + computedBoundingBox.x, mPosition.y);
        return olc::vi2d(topLeftC.x, topLeftC.y);
    }

    olc::vi2d GetSize()
    {
        olc::vi2d size = olc::vi2d(mAnimation->mSpriteSize.x - computedBoundingBox.y, mAnimation->mSpriteSize.y);
        return olc::vi2d(size.x, size.y);
    }

public:
    olc::vf2d mPosition;
    float mIsLeft = false;
    float mVelocity = 40.0f;
    Animation *mAnimation;
    bool isDead = false;

private:
    olc::vf2d computedBoundingBox;

    void ComputeBoundingBox()
    {
        olc::Sprite *sprite = mAnimation->GetSprite();

        int minLeft = sprite->width;
        int maxRight = 0;

        // iterate through the sprite's width and height
        // to find the left and right most pixels

        int previousPixelX = 0;

        for (int y = 0; y < sprite->height; y++)
        {
            for (int x = 0; x < sprite->width; x++)
            {
                olc::Pixel pixel = sprite->GetPixel(x, y);

                if (pixel.a != 0)
                {
                    if (x <= minLeft)
                    {
                        minLeft = previousPixelX;
                    }

                    if (x >= maxRight)
                    {
                        maxRight = previousPixelX;
                    }
                }

                previousPixelX = x;
            }
        }

        computedBoundingBox = olc::vi2d(minLeft, maxRight);
    }
};

class Score
{
public:
    Score()
    {
    }

    void KillAnderson()
    {
        if (killedAnderson)
            return;

        killedAnderson = true;
        mScore += 10;
    }

    void KillAndersonMore()
    {
        if (killedAndersonMore)
            return;

        killedAndersonMore = true;
        mScore += 20;
    }

    void FindAnderson()
    {
        if (foundAnderson)
            return;

        foundAnderson = true;
        mScore += 5;
    }

    void HideTheBody()
    {
        if (hidTheBody)
            return;

        hidTheBody = true;
        mScore += 25;
    }

public:
    int mScore = 0;
    int mLives = 3;
    bool foundAnderson = false;
    bool killedAnderson = false;
    bool killedAndersonMore = false;
    bool hidTheBody = false;

private:
};

class Lia : public olc::PixelGameEngine
{

public:
    Lia()
    {
        sAppName = "Lia";
    }

private:
    Person *mLia;
    Person *mAnderson;
    Score *mScore;
    olc::Sprite *mLivesSprite;
    std::vector<olc::vi2d> randomVector;
    std::string dialog = "";

    float dialogTimer;
    bool showDialog;
    bool isDraggingAnderson;
    bool gameOver;
    bool gameStarted;

    void EnableLiaControls(float fElapsedTime)
    {
        if (GetKey(olc::Key::LEFT).bHeld)
        {
            mLia->mPosition.x -= mLia->mVelocity * fElapsedTime;
            mLia->mIsLeft = true;
            mLia->mAnimation->SetAnimate(true);
        }
        else if (GetKey(olc::Key::RIGHT).bHeld)
        {
            mLia->mPosition.x += mLia->mVelocity * fElapsedTime;
            mLia->mIsLeft = false;
            mLia->mAnimation->SetAnimate(true);
        }
        else if (GetKey(olc::Key::UP).bHeld)
        {
            mLia->mPosition.y -= mLia->mVelocity * fElapsedTime;
            mLia->mAnimation->SetAnimate(true);
        }
        else if (GetKey(olc::Key::DOWN).bHeld)
        {
            mLia->mPosition.y += mLia->mVelocity * fElapsedTime;
            mLia->mAnimation->SetAnimate(true);
        }
        else
        {
            mLia->mAnimation->SetFrame(1);
            mLia->mAnimation->SetAnimate(false);
        }
    }

    void NewDialog(std::string text)
    {
        dialog = text;
        showDialog = true;
    }

    void DrawDialog(float fElapsedTime)
    {
        if (!showDialog)
            return;

        dialogTimer += fElapsedTime;
        if (dialogTimer >= 3.0f)
        {
            dialogTimer = 0.0f;
            showDialog = false;
            dialog = "";
        }

        FillRect(olc::vi2d(0, 0), olc::vi2d(256, 16), olc::VERY_DARK_GREY);
        DrawString(olc::vi2d(0, 0), dialog, olc::WHITE, 1);
    }

    void DrawScore()
    {
        int height = ScreenHeight();

        // XP
        DrawString(olc::vi2d(70, height - 16), "XP: " + std::to_string(mScore->mScore), olc::WHITE, 1);

        // Lives
        for (int i = 0; i < mScore->mLives; i++)
        {
            DrawSprite(olc::vi2d(16 * i + 10, height - 20), mLivesSprite);
        }
    }

    void AndersonInteractionProtocol()
    {
        if (mAnderson->isDead)
        {
            if (GetKey(olc::Key::D).bHeld)
            {
                isDraggingAnderson = true;
            }
            else
            {
                isDraggingAnderson = false;

                // check if anderson is off screen
                if (mAnderson->mPosition.x < 0 || mAnderson->mPosition.x > ScreenWidth() ||
                    mAnderson->mPosition.y < 0 || mAnderson->mPosition.y > ScreenHeight())
                {
                    mScore->HideTheBody();
                    NewDialog("You hid the body!");
                }
            }

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                NewDialog("You killed Anderson even\n more ;-(");
                mScore->mLives--;
                mScore->KillAndersonMore();
            }
            return;
        }

        if (mScore->foundAnderson)
        {
            if (!showDialog)
            {
                NewDialog("You already found me!");
            }

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                NewDialog("You killed Anderson ;-(");
                mAnderson->Die("./gfx/and3.png");
                mScore->mLives--;
                mScore->KillAnderson();
            }

            return;
        }

        mScore->FindAnderson();
        NewDialog("You found Anderson!");
    }

public:
    bool OnUserCreate() override
    {
        gameStarted = false;
        gameOver = false;
        isDraggingAnderson = false;
        showDialog = false;
        dialogTimer = 0.0f;
        std::vector<std::string> liaSprites = {
            "./gfx/lia0.png",
            "./gfx/lia1.png",
            "./gfx/lia2.png",
        };
        randomVector = generateRandomVector(256, 0, 255);
        std::vector<std::string> andersonSprites = {
            "./gfx/and1.png",
            "./gfx/and2.png",
        };

        mLia = new Person(olc::vi2d(16, 16), liaSprites, 6.0f, olc::vf2d(0.0f, 0.0f));
        mScore = new Score();
        mAnderson = new Person(olc::vi2d(16, 16), andersonSprites, 1.5f, olc::vf2d(100.0f, 100.0f));
        mLivesSprite = new olc::Sprite("./gfx/lives.png");

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        if (!gameStarted)
        {
            Clear(olc::DARK_BLUE);
            if (GetKey(olc::Key::SPACE).bPressed)
            {
                gameStarted = true;
            }

            DrawString(olc::vi2d(0, 0), "Press SPACE to start", olc::WHITE, 1);
            DrawString(olc::vi2d(0, 10), "Use the arrow keys to move", olc::WHITE, 1);
            DrawString(olc::vi2d(0, 20), "Use D to drag Anderson", olc::WHITE, 1);
            DrawString(olc::vi2d(0, 30), "Use SPACE to interact w Anderson", olc::WHITE, 1);
            DrawString(olc::vi2d(0, 40), "WELCOME, LIA!", olc::WHITE, 1);

            return true;
        }

        Clear(olc::DARK_GREEN);

        for (int i = 0; i < randomVector.size(); i++)
        {
            Draw(randomVector[i], olc::Pixel(0, 100, 22));
        }

        SetPixelMode(olc::Pixel::MASK);
        EnableLiaControls(fElapsedTime);

        mAnderson->Draw(this, fElapsedTime);

        mLia->Draw(this, fElapsedTime);

        if (mLia->IsHit(mAnderson->GetPosition(), mAnderson->GetSize()))
        {
            AndersonInteractionProtocol();
        }

        if (isDraggingAnderson)
        {
            mAnderson->mPosition = mLia->mPosition;
        }

        DrawScore();
        DrawDialog(fElapsedTime);

        if (mScore->hidTheBody)
        {
            if (!showDialog)
                gameOver = true;
        }

        if (gameOver)
        {
            Clear(olc::BLACK);

            if (GetKey(olc::Key::R).bPressed)
            {
                OnUserCreate();
                gameOver = false;
            }

            int width = ScreenWidth() / 2;
            int height = ScreenHeight() / 2;

            DrawString(olc::vi2d(width - 75, height - 10), "Game Over", olc::WHITE, 2);
            DrawString(olc::vi2d(width - 75, height + 10), "Press R to restart", olc::WHITE, 1);
        }

        return true;
    }

    std::vector<olc::vi2d> generateRandomVector(int size, int min, int max)
    {
        std::vector<olc::vi2d> result;

        int width = ScreenWidth();
        int height = ScreenHeight();

        for (int i = 0; i < size; i++)
        {
            int x = rand() % (width - min + 1) + min;
            int y = rand() % (height - min + 1) + min;
            result.push_back({x, y});
        }
        return result;
    }
};

int main()
{
    srand(time(NULL));

    Lia game;

    if (game.Construct(256, 240, 4, 4))
        game.Start();

    return 0;
}