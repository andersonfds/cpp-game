#define OLC_PGE_APPLICATION
#include <cassert>
#include <olcPixelGameEngine.h>
#include <player.h>
#include <terrain.h>

class LiaGame : public olc::PixelGameEngine
{
public:
    LiaGame()
    {
        sAppName = "Lia";
    }

private:
    olc::Sprite *livesSprite;
    bool isDraggingShovel;
    bool isDraggingAnderson;
    bool didGameStart;
    bool isGameOver;
    bool renderShovel;

    // Scores
    bool didFindAnderson;
    bool didBuryAnderson;

    bool isDialogOpen;
    bool finishedDialog;
    float dialogTimeout;

public:
    character::player *player;
    character::player *anderson;
    character::character *shovel;
    terrain::terrain *terrain;

public:
    bool OnUserCreate() override
    {
        player = new character::player("Lia");
        player->EnableControls();
        player->SetPosition({10.0f, 10.0f});

        anderson = new character::player("Anderson", andersonDefinition);
        anderson->SetPosition({100.0f, 100.0f});
        anderson->SetState("idle", 1, true);
        anderson->coins = rand() % 10 + 1;

        shovel = new character::player("Shovel", shovelDefinition);
        shovel->SetPosition({200.0f, 200.0f});
        shovel->SetState("idle", 1, true);

        livesSprite = new olc::Sprite("./gfx/lives.png");
        terrain = new terrain::terrain();
        terrain->Create(ScreenWidth(), ScreenHeight(), 100, 1);

        didGameStart = false;
        isGameOver = false;
        isDraggingShovel = false;
        isDialogOpen = false;
        finishedDialog = false;
        dialogTimeout = 0.0f;
        renderShovel = false;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        if (isGameOver)
        {
            DrawGameOverScreen();
            return true;
        }

        if (!didGameStart)
        {
            DrawStartScreen();
            return true;
        }

        terrain->Update(this, fElapsedTime);
        anderson->Draw(this, fElapsedTime);
        player->Draw(this, fElapsedTime);

        DrawShovel();
        InteractWithAnderson();
        InteractWithShovel();
        DrawUI();

        return true;
    }

private:
    void InteractWithAnderson()
    {
        bool collidesWithHole = terrain->CollidesWithHole(anderson->box);
        bool collidesWithShovel = shovel->box->Intersects(anderson->box);

        if (collidesWithHole)
        {
            KillAnderson();
        }

        if (player->box->Intersects(anderson->box))
        {
            if (anderson->IsDead())
            {
                if (anderson->coins > 0)
                {
                    ShowDialog("Holy shit! You killed him!!\nPress SPACE to steal his coins!", 0.0f);
                }
                else if (!collidesWithHole)
                {
                    if (!terrain->HasHoles())
                    {
                        ShowDialog("You need to bury him. Find a shovel", 0.0f);
                        renderShovel = true;
                    }
                    else if (isDraggingAnderson)
                        ShowDialog("Go to the hole", 0.0f);
                    else
                        ShowDialog("Press SPACE to drag the body", 0.0f);
                }
                else
                {
                    if (isDraggingAnderson)
                        ShowDialog("Press SPACE to drop the body", 0.0f);
                    else
                    {
                        if (!collidesWithShovel)
                        {
                            ShowDialog("Go get the shovel", 0.0f);
                        }
                        else if (isDraggingShovel)
                        {
                            ShowDialog("Press SPACE to bury the body", 0.0f);
                            didBuryAnderson = true;
                        }
                    }
                }
            }
            else
            {
                ShowDialog("You found Anderson!!\nPress SPACE to interact", 0.0f);
                FindAnderson();
            }

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                if (anderson->IsDead())
                {
                    if (anderson->coins > 0)
                    {
                        StealAnderson();
                    }
                    else
                    {
                        isDraggingAnderson = !isDraggingAnderson;
                    }
                }
                else
                {
                    KillAnderson();
                }
            }
        }

        if (isDraggingAnderson)
        {
            DragAndersonBody();
        }
    }

    void InteractWithShovel()
    {
        if (!renderShovel)
        {
            return;
        }

        bool collidesWithHole = terrain->CollidesWithHole(shovel->box);
        bool collidesWithAnderson = anderson->box->Intersects(shovel->box);

        if (player->box->Intersects(shovel->box))
        {
            if (GetKey(olc::Key::SPACE).bPressed)
            {
                if (collidesWithHole)
                {
                    isDraggingShovel = true;
                    ShowDialog("Can't drop the shovel here", 0.0f);
                }
                else
                {
                    isDraggingShovel = !isDraggingShovel;
                }
            }

            if (GetKey(olc::Key::R).bPressed)
            {
                AddHoleAtPlayerPosition();
            }

            if (!terrain->HasHoles())
            {
                if (isDraggingShovel)
                    ShowDialog("Press R to dig a hole", 0.0f);
                else
                    ShowDialog("Press SPACE to pick up the shovel", 0.0f);
            }
            else if (terrain->CollidesWithHole(shovel->box))
            {
                if (collidesWithAnderson)
                {
                    if (didBuryAnderson)
                        isGameOver = true;
                    else
                        ShowDialog("Press SPACE to bury the body", 0.0f);
                }
                else
                {
                    ShowDialog("Go to Anderson", 0.0f);
                }
            }
            else
            {
                if (!collidesWithAnderson)
                {
                    if (isDraggingShovel)
                        ShowDialog("Go to Anderson", 0.0f);
                    else
                        ShowDialog("Press SPACE to pick up the shovel", 0.0f);
                }
            }
        }
    }

private:
    void KillAnderson()
    {
        if (anderson->IsDead())
        {
            return;
        }
        player->XP += 10;
        anderson->SetState("dead");
        anderson->Kill();
    }

    void FindAnderson()
    {
        if (didFindAnderson)
        {
            return;
        }

        didFindAnderson = true;
        player->XP += 10;
    }

    void ShowDialog(std::string text, float fElapsedTime)
    {
        bool timeoutReached = dialogTimeout >= 3.0f;

        if (timeoutReached)
        {
            isDialogOpen = false;
            return;
        }

        dialogTimeout += fElapsedTime;
        isDialogOpen = true;
        DrawDialog(text);
    }

    void ResetDialog()
    {
        dialogTimeout = 0.0f;
        isDialogOpen = false;
    }

    void DrawDialog(std::string text)
    {
        if (!isDialogOpen)
        {
            return;
        }

        int padding = 10;
        int width = ScreenWidth() - padding * 2;
        int height = 20 + padding * 2;

        FillRect(olc::vi2d(padding, padding), olc::vi2d(width, height), olc::VERY_DARK_GREEN);
        DrawString(olc::vi2d(padding + 5, padding + 5), text, olc::WHITE, 1);
    }

    void AddHoleAtPlayerPosition()
    {
        olc::vf2d pos = *player->GetPosition();
        pos.x += 8;
        pos.y += 8;
        terrain->AddHole(pos);
    }

    void StealAnderson()
    {
        if (!anderson->IsDead() || anderson->coins <= 0)
        {
            return;
        }

        player->XP += 10;
        player->coins += anderson->coins;
        anderson->coins = 0;
    }

    void DragAndersonBody()
    {
        if (!anderson->IsDead())
        {
            return;
        }

        anderson->SetPosition(*player->GetPosition());
    }

    void DrawShovel()
    {
        if (!renderShovel)
        {
            return;
        }

        olc::vf2d *pos = shovel->GetPosition();
        SetPixelMode(olc::Pixel::MASK);
        bool flip = player->isFacingLeft | !isDraggingShovel;
        DrawSprite(*pos, shovel->GetSprite(), 1, flip);

        if (isDraggingShovel)
        {
            shovel->SetPosition(*player->GetPosition());
        }
    }

    void DrawUI()
    {
        int y = ScreenHeight() - 20;
        int spriteWidth = livesSprite->width;

        // Lives
        for (int i = 0; i < player->GetLives(); i++)
        {
            SetPixelMode(olc::Pixel::MASK);
            DrawSprite(olc::vi2d(spriteWidth * i + 10, y), livesSprite);
            SetPixelMode(olc::Pixel::NORMAL);
        }

        // XP
        DrawString(olc::vi2d(70, y + 4), "XP: " + std::to_string(player->XP), olc::WHITE, 1);

        // Draw Coins
        int x = 130, radius = 5, coins = player->coins;
        for (int i = 0; i < coins * 2; i++)
        {
            bool isOdd = i % 2 != 0;
            olc::Pixel color = isOdd ? olc::YELLOW : olc::DARK_YELLOW;
            FillCircle(olc::vi2d(x + i * 2, y + radius), radius, color);
        }
    }

    void DrawStartScreen()
    {
        Clear(olc::VERY_DARK_BLUE);
        if (GetKey(olc::Key::SPACE).bPressed)
        {
            didGameStart = true;
        }

        SetPixelMode(olc::Pixel::MASK);
        DrawString(olc::vi2d(10, 10), "Welcome, " + player->GetName() + "! XD", olc::WHITE, 1);

        DrawSprite(olc::vi2d(10, 60), anderson->GetSprite(), 3);
        DrawSprite(olc::vi2d(50, 90), player->GetSprite(), 4, true);

        // Start game
        DrawString(olc::vi2d(10, ScreenHeight() - 20), "Press SPACE to start", olc::WHITE, 1);
        DrawString(olc::vi2d(ScreenWidth() - 200, ScreenHeight() - 20), "Developed by Anderson!", olc::DARK_GREY, 1);
    }

    void DrawGameOverScreen()
    {
        Clear(olc::BLACK);

        if (GetKey(olc::Key::R).bPressed)
        {
            OnUserCreate();
            isGameOver = false;
        }

        int width = ScreenWidth() / 2;
        int height = ScreenHeight() / 2;

        DrawString(olc::vi2d(width - 75, height - 10), "Game Over", olc::WHITE, 2);
        DrawString(olc::vi2d(width - 100, height + 10), "You burried Anderson ^-^", olc::WHITE, 1);
        DrawString(olc::vi2d(width - 80, height + 30), "Thx for playing s2", olc::VERY_DARK_GREY, 1);
        DrawString(olc::vi2d(width - 80, ScreenHeight() - 30), "Press R to restart", olc::VERY_DARK_GREY, 1);
    }
};

int main()
{
    srand(time(NULL));

    LiaGame game;

    if (game.Construct(400, 300, 3, 3))
        game.Start();

    return 0;
}