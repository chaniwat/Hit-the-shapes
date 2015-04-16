#include "main.h"
#include "game.h"
#include "resourcemanager.h"
#include "spriterender.h"
#include "textrender.h"
#include "dirent.h"
//#include "sound.h"

#include <mxml.h>

SpriteRender *SpriteRenderer;
SpriteRender *ColorIDRenderer;
TextRender *TextRenderer;

static GLchar pathlist[256][128];
static GLint themecount = 0;
static GLchar temptheme[256][256];
static GLint themepage = 1;
static GLint thememaxpage = 1;

static GLfloat waitopeningtime = 3;
static GLfloat alpha = 1.0f;

static GLfloat circle1_angle = 0.0f;
static GLfloat circle2_angle = 0.0f;
static GLfloat circle3_angle = 0.0f;

static GLfloat forst_frame_alpha = 0.0f;

static GLint pawnthemesize[4][2];

GLvoid de_allocatethemepreview();

Game::Game(GLuint width, GLuint heigth) : windowWidth(width), windowHeight(heigth) ,Currentlevel(MENU_LV), Currenttheme(0), Score(0)
{

}

Game::~Game()
{
    delete SpriteRenderer;
    delete ColorIDRenderer;
    delete TextRenderer;
}

GLvoid Game::init()
{
    // Load shaders
    ResourceManager::LoadShader("../Shader/sprite.vert", "../Shader/sprite.frag", "sprite");
    ResourceManager::LoadShader("../Shader/sprite.vert", "../Shader/colorid.frag", "colorid");
    // Load textures
    // UI BG
    ResourceManager::LoadTexture("../Images/home.jpg", GL_FALSE, "openingbg");
    ResourceManager::LoadTexture("../Images/menubg.jpg", GL_FALSE, "background");
    // Menu Level
    ResourceManager::LoadTexture("../Images/Button/btn_play.png", GL_TRUE, "ui_btn_play");
    ResourceManager::LoadTexture("../Images/Button/btn_quit.png", GL_TRUE, "ui_btn_quit");
    ResourceManager::LoadTexture("../Images/Button/btn_highscore.png", GL_TRUE, "ui_btn_highscore");
    ResourceManager::LoadTexture("../Images/illu_menu.png", GL_TRUE, "ui_illu_menu");
    ResourceManager::LoadTexture("../Images/circle_1.png", GL_TRUE, "ui_circle_1");
    ResourceManager::LoadTexture("../Images/circle_2.png", GL_TRUE, "ui_circle_2");
    ResourceManager::LoadTexture("../Images/circle_3.png", GL_TRUE, "ui_circle_3");
    // Other level
    ResourceManager::LoadTexture("../Images/Button/btn_timeatk.png", GL_TRUE, "ui_btn_timeatk");
    ResourceManager::LoadTexture("../Images/Button/btn_endless.png", GL_TRUE, "ui_btn_endless");
    // Play level
    ResourceManager::LoadTexture("../Images/frost-frame.png", GL_TRUE, "ui_forst_frame");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->windowWidth), static_cast<GLfloat>(this->windowHeight), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("colorid").Use().SetMatrix4("projection", projection);
    // Set render-specific controls
    SpriteRenderer = new SpriteRender(ResourceManager::GetShader("sprite"));
    ColorIDRenderer = new SpriteRender(ResourceManager::GetShader("colorid"));
    TextRenderer = new TextRender(this->windowWidth, this->windowHeight);
    TextRenderer->Load("../Font/supermarket.ttf", 128);
    // Initialize level
    this->ChangeLevel(MENU_LV);
}

GLvoid Game::Update(GLfloat dt)
{
    if (this->Currentlevel != PLAY_LV)
    {
        if (waitopeningtime > 0)
        {
            waitopeningtime -= dt;
            if (waitopeningtime < 1)
            {
                alpha -= dt;
            }
        }

        circle1_angle += (115.0f / 360.0f) * dt;
        if (circle1_angle > 360.0f) circle1_angle = fmod(circle1_angle, 360.0f);
        circle2_angle += (175.0f / 360.0f) * dt;
        if (circle1_angle > 360.0f) circle1_angle = fmod(circle1_angle, 360.0f);
        circle3_angle += (236.0f / 360.0f) * dt;
        if (circle1_angle > 360.0f) circle1_angle = fmod(circle1_angle, 360.0f);
    }
    else 
    {
        if (this->SlowMode && this->SlowTime > 0)
        {
            this->SlowTime -= dt;
            if (forst_frame_alpha < 1 && this->SlowTime > 0.5f)
            {
                forst_frame_alpha += dt * 2;
                if (forst_frame_alpha > 1) forst_frame_alpha = 1.0;
            }
            else if (forst_frame_alpha > 0)
            {
                forst_frame_alpha -= dt * 2;
                if (forst_frame_alpha < 0) forst_frame_alpha = 0.0;
            }
        }
        else if (this->SlowMode && this->SlowTime <= 0)
        {
            this->SlowMode = GL_FALSE;
            this->SlowTime = 0;

            std::cout << "Slow mode expired!" << std::endl;
        }

        if (this->MultiMode && this->MultiTime > 0)
        {
            this->MultiTime -= dt;
            /*if (forst_frame_alpha < 1 && this->MultiTime > 0.5f)
            {
                forst_frame_alpha += dt * 2;
                if (forst_frame_alpha > 1) forst_frame_alpha = 1.0;
            }
            else if (forst_frame_alpha > 0)
            {
                forst_frame_alpha -= dt * 2;
                if (forst_frame_alpha < 0) forst_frame_alpha = 0.0;
            }*/
        }
        else if (this->MultiMode && this->MultiTime <= 0)
        {
            this->MultiMode = GL_FALSE;
            this->MultiTime = 0;

            std::cout << "Multiplier mode expired!" << std::endl;
        }

        if (this->DestroyMode)
        {
            for (GamePawn &itr : this->Pawn)
            {
                // Destroy pawn and add score
                if (itr.isDestroyed) continue;
                if (this->MultiMode)
                {
                    std::cout << "You get: " << itr.Score * 2 << " scores for destroy powerup." << std::endl;
                    this->Score += itr.Score * 2;
                }
                else
                {
                    std::cout << "You get: " << itr.Score << " scores for destroy powerup." << std::endl;
                    this->Score += itr.Score;
                }
                itr.isDestroyed = GL_TRUE;
            }
            this->DestroyMode = GL_FALSE;
        }

        if (this->Currentmode == TIME_ATTACK) 
        {
            if (this->CurrentPlayState == PLAY && this->Time > 0)
            {
                if (this->SlowMode) this->Time -= 0;
                else this->Time -= dt;
                this->SpawnPawn(dt);
            }
            else if (this->Time < 0)
            {
                this->Time = 0;
                this->CurrentPlayState = END;
            }
        }
        else if (this->Currentmode == ENDLESS)
        {
            if (this->CurrentPlayState == PLAY && this->Lives > 0)
            {
                this->Time += dt;
                this->SpawnPawn(dt);
            }
            else if (this->Lives <= 0)
            {
                this->Lives = 0;
                this->CurrentPlayState = END;
            }
        }
        if (this->CurrentPlayState == PLAY)
        {
            // Reduce life time
            for (GamePawn &itr : this->Pawn)
            {
                if (itr.isDestroyed) continue;
                if (this->SlowMode) itr.aliveTime -= 0;
                else itr.aliveTime -= dt;
                if (itr.aliveTime <= 0) 
                {
                    if (this->Currentmode == ENDLESS) if (!itr.special) this->Lives -= 1;
                    itr.isDestroyed = GL_TRUE;
                }
            }
        }   
    }
}

GLvoid Game::Render(GLfloat dt)
{
    DrawCurrentLevel(dt);
}

GLvoid Game::ProcessInput()
{
    GLint viewport[4];
    GLubyte pixel[3];
    GLdouble xpos, ypos;

    /*
        Change cursor events
    */
    if (this->Currentlevel == PLAY_LV)
    {
        if (this->CurrentPlayState == PLAY)
        {
            // Render the color ID
            for (GamePawn &itr : this->Pawn)
            {
                // Skip the pawn that already destroyed
                if (itr.isDestroyed) continue;
                // Render pawn color to color ID
                itr.DrawColorID(*ColorIDRenderer);
            }
        }

        // Read Color pixel at cursor
        glGetIntegerv(GL_VIEWPORT, viewport);
        glfwGetCursorPos(Getwindow(), &xpos, &ypos);
        glReadPixels(xpos, viewport[3] - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixel);

        // Check if over on any clickable, change cursor to pointer
        for (GamePawn &itr : this->Pawn)
        {
            if (itr.ColorID.r * 255.0f == pixel[0] && itr.ColorID.g * 255.0f == pixel[1] && itr.ColorID.b * 255.0f == pixel[2])
            {
                SetWindowCursor(POINTER);
                break;
            }
            else
            {
                SetWindowCursor(ARROW);
            }
        }
    }
    else if (waitopeningtime <= 0)
    {
        // Render the color ID
        for (UIButton &itr : this->Buttons)
        {
            if (this->Currentlevel == THEME_LV && itr.ColorID.r * 255.0f >= (9.0f * (themepage - 1)) + 1 && itr.ColorID.r * 255.0f <= (9.0f * themepage) && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
            {
                itr.DrawColorID(*ColorIDRenderer);
            }
            if (this->Currentlevel == THEME_LV && itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 1.0f && itr.ColorID.b * 255.0f == 0.0f && themepage != 1)
            {
                itr.DrawColorID(*ColorIDRenderer);
            }
            if (this->Currentlevel == THEME_LV && itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 2.0f && itr.ColorID.b * 255.0f == 0.0f && themepage != thememaxpage)
            {
                itr.DrawColorID(*ColorIDRenderer);
            }
            else if(this->Currentlevel != THEME_LV) itr.DrawColorID(*ColorIDRenderer);
        }

        // Read Color pixel at cursor
        glGetIntegerv(GL_VIEWPORT, viewport);
        glfwGetCursorPos(Getwindow(), &xpos, &ypos);
        glReadPixels(xpos, viewport[3] - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixel);

        // Check if over on any clickable, change cursor to pointer
        for (UIButton &itr : this->Buttons)
        {
            if (itr.ColorID.r * 255.0f == pixel[0] && itr.ColorID.g * 255.0f == pixel[1] && itr.ColorID.b * 255.0f == pixel[2])
            {
                SetWindowCursor(POINTER);
                break;
            }
            else
            {
                SetWindowCursor(ARROW);
            }
        }
    }

    if (this->Keys[GLFW_MOUSE_BUTTON_LEFT] && !this->Keysprocessed[GLFW_MOUSE_BUTTON_LEFT])
    {
        if (this->Currentlevel == PLAY_LV)
        {
            std::cout << "X: " << xpos << " Y: " << ypos << std::endl;
            std::cout << "R: " << (int)pixel[0] << " G: " << (int)pixel[1] << " B: " << (int)pixel[2] << std::endl;

            if (this->CurrentPlayState == PLAY)
            {
                // Check what object is cursor over and do something
                for (GamePawn &itr : this->Pawn)
                {
                    if (itr.ColorID.r * 255.0f == pixel[0] && itr.ColorID.g * 255.0f == pixel[1] && itr.ColorID.b * 255.0f == pixel[2] && !itr.special)
                    {
                        // Destroy pawn and add score
                        if (this->MultiMode)
                        {
                            std::cout << "You get: " << itr.Score * 2 << " scores for clicking." << std::endl;
                            this->Score += itr.Score * 2;
                        }
                        else
                        {
                            std::cout << "You get: " << itr.Score << " scores for clicking." << std::endl;
                            this->Score += itr.Score;
                        }
                        itr.isDestroyed = GL_TRUE;
                    }
                    else if (itr.ColorID.r * 255.0f == pixel[0] && itr.ColorID.g * 255.0f == pixel[1] && itr.ColorID.b * 255.0f == pixel[2] && itr.special)
                    {
                        // Destroy pawn and add score
                        if (itr.specialtype == SLOW) 
                        {
                            this->SlowMode = GL_TRUE;
                            std::cout << "Slow mode engage for 5 seconds!" << std::endl;
                            this->SlowTime = 5;
                        }
                        else if (itr.specialtype == MULTIPLIER)
                        {
                            this->MultiMode = GL_TRUE;
                            std::cout << "Multiplier mode engage for 5 seconds!" << std::endl;
                            this->MultiTime = 5;
                        }
                        else if (itr.specialtype == DESTROY)
                        {
                            this->DestroyMode = GL_TRUE;
                            std::cout << "Destroy all." << std::endl;
                        }
                        itr.isDestroyed = GL_TRUE;
                    }
                }
            }
        }
        else if (waitopeningtime <= 0)
        {
            std::cout << "X: " << xpos << " Y: " << ypos << std::endl;
            std::cout << "R: " << (int)pixel[0] << " G: " << (int)pixel[1] << " B: " << (int)pixel[2] << std::endl;

            // Check what object is cursor over and do something
            for (UIButton &itr : this->Buttons)
            {
                if (itr.ColorID.r * 255.0f == pixel[0] && itr.ColorID.g * 255.0f == pixel[1] && itr.ColorID.b * 255.0f == pixel[2])
                {
                    // set isClicked to true to used to process in future
                    itr.isClicked = GL_TRUE;
                }
            }

            for (UIButton &itr : this->Buttons)
            {
                if (itr.isClicked)
                {
                    itr.isClicked = GL_FALSE;
                    if (this->Currentlevel == MENU_LV)
                    {
                        if (itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            this->RenderLoading("Loading Themes...", 0.60f);
                            this->ChangeLevel(THEME_LV);
                        }
                        else if (itr.ColorID.r * 255.0f == 2.0f && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            glfwSetWindowShouldClose(Getwindow(), GL_TRUE);
                        }
                    }
                    else if (this->Currentlevel == THEME_LV)
                    {
                        if (itr.ColorID.r * 255.0f >= 1.0f && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            this->RenderLoading("Loading Selected Themes...", 0.60f);
                            this->ChangeLevel(MODE_LV);
                            int i = itr.ColorID.r * 255.0f;
                            this->LoadGameTheme(pathlist[i]);
                            de_allocatethemepreview();
                        }
                        else if (itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 1.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            themepage--;
                        }
                        else if (itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 2.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            themepage++;
                        }
                    }
                    else if (this->Currentlevel == MODE_LV)
                    {
                        if (itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            this->ChangeLevel(PLAY_LV);
                            this->Currentmode = TIME_ATTACK;
                            this->Time = 30.0f;
                        }
                        else if (itr.ColorID.r * 255.0f == 2.0f && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
                            this->ChangeLevel(PLAY_LV);
                            this->Currentmode = ENDLESS;
                            this->Lives = 15;
                        }
                    }
                    break;
                }
            }
        }

        // Set that this key already processed
        this->Keysprocessed[GLFW_MOUSE_BUTTON_LEFT] = GL_TRUE;
    }

    if (this->Keys[GLFW_KEY_ESCAPE] && !this->Keysprocessed[GLFW_KEY_ESCAPE])
    {
        if (this->Currentlevel == PLAY_LV)
        {
            if (this->CurrentPlayState != PAUSE && this->CurrentPlayState != END)
            {
                this->CurrentPlayState = PAUSE;
            }
            else if (this->CurrentPlayState == PAUSE)
            {
                this->CurrentPlayState = PLAY;
            }
        }
        this->Keysprocessed[GLFW_KEY_ESCAPE] = GL_TRUE;
    }

    if (this->Currentlevel == PLAY_LV)
    {
        if (this->CurrentPlayState == END)
        {
            GLint anykeypress;
            for (anykeypress = 10; anykeypress < 1024; anykeypress++)
            {
                if (this->Keys[anykeypress])
                {
                    this->ResetGame();
                    this->ChangeLevel(MENU_LV);
                    this->Keysprocessed[anykeypress] = GL_TRUE;
                    break;
                }
            }
        }
    }
}

GLvoid Game::DrawCurrentLevel(GLfloat dt)
{
    if (this->Currentlevel == MENU_LV || this->Currentlevel == MODE_LV)
    {
        if (this->Currentlevel == MENU_LV)
        {
            SpriteRenderer->Draw(ResourceManager::GetTexture("background"), glm::vec2(0, -30), glm::vec2(this->windowWidth, this->windowHeight + 60), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            SpriteRenderer->Draw(ResourceManager::GetTexture("ui_illu_menu"), glm::vec2((this->windowWidth / 2) - 224, (this->windowHeight / 2) - 257), glm::vec2(500, 520), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            SpriteRenderer->Draw(ResourceManager::GetTexture("ui_circle_1"), glm::vec2((this->windowWidth / 2) - 167, (this->windowHeight / 2) - 157), glm::vec2(334, 318), circle1_angle, glm::vec3(1.0f, 1.0f, 1.0f));
            SpriteRenderer->Draw(ResourceManager::GetTexture("ui_circle_2"), glm::vec2((this->windowWidth / 2) - 212, (this->windowHeight / 2) - 197), glm::vec2(414, 394), circle2_angle, glm::vec3(1.0f, 1.0f, 1.0f));
            SpriteRenderer->Draw(ResourceManager::GetTexture("ui_circle_3"), glm::vec2((this->windowWidth / 2) - 260, (this->windowHeight / 2) - 245), glm::vec2(520, 494), circle3_angle, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        else SpriteRenderer->Draw(ResourceManager::GetTexture("theme_background"), glm::vec2(0, 0), glm::vec2(this->windowWidth, this->windowHeight), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        for (UIButton &itr : this->Buttons)
        {
            itr.Draw(*SpriteRenderer);
        }

        if (waitopeningtime > 0) SpriteRenderer->Draw(ResourceManager::GetTexture("openingbg"), glm::vec2(0, -30), glm::vec2(this->windowWidth, this->windowHeight + 60), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), alpha);
    }
    else if (this->Currentlevel == THEME_LV)
    {
        SpriteRenderer->Draw(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->windowWidth, this->windowHeight), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        for (UIButton &itr : this->Buttons)
        {
            if (itr.ColorID.r * 255.0f >= (9.0f * (themepage - 1)) + 1 && itr.ColorID.r * 255.0f <= (9.0f * themepage) && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
            {
                itr.Draw(*SpriteRenderer);
            }
            if (itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 1.0f && itr.ColorID.b * 255.0f == 0.0f && themepage != 1)
            {
                itr.Draw(*SpriteRenderer);
            }
            if (itr.ColorID.r * 255.0f == 1.0f && itr.ColorID.g * 255.0f == 2.0f && itr.ColorID.b * 255.0f == 0.0f && themepage != thememaxpage)
            {
                itr.Draw(*SpriteRenderer);
            }
        }
    }
    else
    {
        SpriteRenderer->Draw(ResourceManager::GetTexture("theme_background"), glm::vec2(0, 0), glm::vec2(this->windowWidth, this->windowHeight), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        for (GamePawn &itr : this->Pawn)
        {
            if (itr.isDestroyed) continue;
            itr.Draw(*SpriteRenderer);
        }

        if (this->SlowMode) SpriteRenderer->Draw(ResourceManager::GetTexture("ui_forst_frame"), glm::vec2(-30, -30), glm::vec2(this->windowWidth+60, this->windowHeight+60), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), forst_frame_alpha);

        static GLchar buffertext[32];
        sprintf(buffertext, "Score: %d", this->Score);
        TextRenderer->RenderText(buffertext, 0.0f, 0.0f, 0.40f, glm::vec3(255.0f / 255.0f, 28.0f / 255.0f, 202.0f / 255.0f));

        sprintf(buffertext, "Time: %.2f", this->Time);
        TextRenderer->RenderText(buffertext, 0.0f, 50.0f, 0.40f, glm::vec3(255.0f / 255.0f, 28.0f / 255.0f, 202.0f / 255.0f));

        if (this->Currentmode == ENDLESS)
        {
            sprintf(buffertext, "Lives: %d", this->Lives);
            TextRenderer->RenderText(buffertext, 0.0f, 100.0f, 0.40f, glm::vec3(255.0f / 255.0f, 28.0f / 255.0f, 202.0f / 255.0f));
        }

        if (this->CurrentPlayState == PAUSE)
        {
            TextRenderer->RenderText("GAME PAUSE!, Press ESC to continue", (this->windowWidth / 2) - 342.0f, (this->windowHeight / 2) - 30.0f, 0.40f, glm::vec3(255.0f / 255.0f, 28.0f / 255.0f, 202.0f / 255.0f));
        }

        if (this->CurrentPlayState == END)
        {
            TextRenderer->RenderText("GAME OVER... Press any key to start new game.", (this->windowWidth / 2) - 312.0f, (this->windowHeight / 2) - 30.0f, 0.40f, glm::vec3(255.0f / 255.0f, 28.0f / 255.0f, 202.0f / 255.0f));
        }
    }
}

GLvoid Game::SpawnPawn(GLfloat dt)
{
    std::srand(std::time(0));

    if (this->Currentmode == TIME_ATTACK)
    {
        this->PlayTimer += dt;

        // Random spawn by time
        if (this->PlayTimer > this->NextTimeSpawn)
        {
            
            for (GLfloat i = 0.0f; i < PlayTimer / 5; i++)
            {
                
                GLfloat factorsize;
                GLfloat x_pos = std::rand() % (this->windowWidth - 100 + 1);
                GLfloat y_pos = std::rand() % (this->windowHeight - 100 + 1);
                GLfloat percentofrarespawn = std::rand() % 10000;
                if (percentofrarespawn >= 0 && percentofrarespawn <= 6000)
                {
                    if (pawnthemesize[0][0] > pawnthemesize[0][1]) factorsize = pawnthemesize[0][0] / 100;
                    else factorsize = pawnthemesize[0][1] / 100;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[0][0] / factorsize, pawnthemesize[0][1] / factorsize), ResourceManager::GetTexture("theme_pawn1")));
                }
                else if (percentofrarespawn >= 6001 && percentofrarespawn <= 8500)
                {
                    if (pawnthemesize[1][0] > pawnthemesize[1][1]) factorsize = pawnthemesize[1][0] / 80;
                    else factorsize = pawnthemesize[1][1] / 80;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 2.5, 3, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[1][0] / factorsize, pawnthemesize[1][1] / factorsize), ResourceManager::GetTexture("theme_pawn2")));
                }
                else if (percentofrarespawn >= 8501 && percentofrarespawn <= 9550)
                {
                    if (pawnthemesize[2][0] > pawnthemesize[2][1]) factorsize = pawnthemesize[2][0] / 60;
                    else factorsize = pawnthemesize[2][1] / 60;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 1.75, 5, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[2][0] / factorsize, pawnthemesize[2][1] / factorsize), ResourceManager::GetTexture("theme_pawn3")));
                }
                else if (percentofrarespawn >= 9551 && percentofrarespawn <= 9999)
                {
                    GLfloat typerand = (std::rand() % 3) + 1;
                    specialpawn type;
                    if (typerand == 1) type = SLOW;
                    else if (typerand == 2) type = MULTIPLIER;
                    else if (typerand == 3) type = DESTROY;
                    if (pawnthemesize[3][0] > pawnthemesize[3][1]) factorsize = pawnthemesize[3][0] / 100;
                    else factorsize = pawnthemesize[3][1] / 100;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 2.35, 0, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[3][0] / factorsize, pawnthemesize[3][1] / factorsize), ResourceManager::GetTexture("theme_slow"), GL_TRUE, type));
                }
                if (this->RSCID_red != 255)
                {
                    this->RSCID_red++;
                }
                else if (this->RSCID_green != 255)
                {
                    this->RSCID_red = 1.0f;
                    this->RSCID_green++;
                }
                else
                {
                    this->RSCID_red = 1.0f;
                    this->RSCID_green = 0.0f;
                    this->RSCID_blue++;
                }
            }
            this->NextTimeSpawn += this->TimeSpawnBase;
        }
    }
    else if (this->Currentmode == ENDLESS)
    {
        this->PlayTimer += dt;

        // Random spawn by time
        if (this->PlayTimer > this->NextTimeSpawn)
        {
            for (GLfloat i = 0.0f; i < PlayTimer / 10; i++)
            {
                GLfloat factorsize;
                GLfloat x_pos = std::rand() % (this->windowWidth - 100 + 1);
                GLfloat y_pos = std::rand() % (this->windowHeight - 100 + 1);
                GLfloat percentofrarespawn = std::rand() % 10000;
                if (percentofrarespawn >= 0 && percentofrarespawn <= 6000)
                {
                    if (pawnthemesize[0][0] > pawnthemesize[0][1]) factorsize = pawnthemesize[0][0] / 100;
                    else factorsize = pawnthemesize[0][1] / 100;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[0][0] / factorsize, pawnthemesize[0][1] / factorsize), ResourceManager::GetTexture("theme_pawn1")));
                }
                else if (percentofrarespawn >= 6001 && percentofrarespawn <= 8500)
                {
                    if (pawnthemesize[1][0] > pawnthemesize[1][1]) factorsize = pawnthemesize[1][0] / 100;
                    else factorsize = pawnthemesize[1][1] / 100;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[1][0] / factorsize, pawnthemesize[1][1] / factorsize), ResourceManager::GetTexture("theme_pawn2")));
                }
                else if (percentofrarespawn >= 8501 && percentofrarespawn <= 9550)
                {
                    if (pawnthemesize[2][0] > pawnthemesize[2][1]) factorsize = pawnthemesize[2][0] / 100;
                    else factorsize = pawnthemesize[2][1] / 100;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[2][0] / factorsize, pawnthemesize[2][1] / factorsize), ResourceManager::GetTexture("theme_pawn3")));
                }
                else if (percentofrarespawn >= 9551 && percentofrarespawn <= 9999)
                {
                    GLfloat typerand = (std::rand() % 3) + 1;
                    specialpawn type;
                    if (typerand == 1) type = SLOW;
                    else if (typerand == 2) type = MULTIPLIER;
                    else if (typerand == 3) type = DESTROY;
                    if (pawnthemesize[3][0] > pawnthemesize[3][1]) factorsize = pawnthemesize[3][0] / 100;
                    else factorsize = pawnthemesize[3][1] / 100;
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 2.35, 0, glm::vec2(x_pos, y_pos), glm::vec2(pawnthemesize[3][0] / factorsize, pawnthemesize[3][1] / factorsize), ResourceManager::GetTexture("theme_slow"), GL_TRUE, type));
                }
                if (this->RSCID_red != 255)
                {
                    this->RSCID_red++;
                }
                else if (this->RSCID_green != 255)
                {
                    this->RSCID_red = 1.0f;
                    this->RSCID_green++;
                }
                else
                {
                    this->RSCID_red = 1.0f;
                    this->RSCID_green = 0.0f;
                    this->RSCID_blue++;
                }
            }
            this->NextTimeSpawn += this->TimeSpawnBase;
        }
    }
}

GLvoid Game::ChangeLevel(GameLevel level)
{
    this->Buttons.clear();
    this->ResetColorID();
    
    if (level == MENU_LV)
    {
        this->Currentlevel = MENU_LV;

        this->Buttons.push_back(UIButton(glm::vec3(1.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2((this->windowWidth / 2) - (267.0 / 2.0), (this->windowHeight / 2) - 124.0), glm::vec2(267, 124), ResourceManager::GetTexture("ui_btn_play")));
        this->Buttons.push_back(UIButton(glm::vec3(2.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2((this->windowWidth / 2) - 167.0, (this->windowHeight / 2)), glm::vec2(167, 158), ResourceManager::GetTexture("ui_btn_quit")));
        this->Buttons.push_back(UIButton(glm::vec3(3.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2((this->windowWidth / 2), (this->windowHeight / 2)), glm::vec2(208, 199), ResourceManager::GetTexture("ui_btn_highscore")));
    }
    else if (level == THEME_LV)
    {
        this->Currentlevel = THEME_LV;

        strcpy(pathlist[0], "");
        themecount = 0;

        int dflag = 0;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir("../Theme/")) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL) {
                if (dflag < 2)
                {
                    dflag++;
                    continue;
                }
                std::cout << ent->d_name << std::endl;
                
                strcpy(pathlist[themecount + 1], "../Theme/");
                strcat(pathlist[themecount + 1], ent->d_name);
                strcat(pathlist[themecount + 1], "/");

                GLchar pathtopreview[256] = "";
                strcpy(pathtopreview, pathlist[themecount + 1]);
                strcat(pathtopreview, "preview.png");

                strcpy(temptheme[themecount], "");
                sprintf(temptheme[themecount], "Theme%03d", themecount);

                ResourceManager::LoadTexture(pathtopreview, GL_TRUE, temptheme[themecount]);
                this->Buttons.push_back(UIButton(glm::vec3(this->RSCID_red++ / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2((300 * ((themecount % 9) % 3)) + 58, (225 * (((themecount % 9) / 3) % 3) + 32)), glm::vec2(300, 225), ResourceManager::GetTexture(temptheme[themecount])));

                themecount++;
            }
            closedir(dir);
        }
        else {
            /* could not open directory */
            perror("");
        }

        thememaxpage = ceil(themecount / 9.0f);

        ResourceManager::LoadTexture("../Images/Button/prevpagetheme.png", GL_TRUE, "prevpagetheme");
        ResourceManager::LoadTexture("../Images/Button/nextpagetheme.png", GL_TRUE, "nextpagetheme");
        this->Buttons.push_back(UIButton(glm::vec3(1.0f / 255.0f, 1.0f / 255.0f, 0.0f / 255.0f), glm::vec2(0, this->windowHeight - 80), glm::vec2(180, 80), ResourceManager::GetTexture("prevpagetheme")));
        this->Buttons.push_back(UIButton(glm::vec3(1.0f / 255.0f, 2.0f / 255.0f, 0.0f / 255.0f), glm::vec2(this->windowWidth - 180, this->windowHeight - 80), glm::vec2(180, 80), ResourceManager::GetTexture("nextpagetheme")));
    }
    else if (level == MODE_LV)
    {
        this->Currentlevel = MODE_LV;

        this->Buttons.push_back(UIButton(glm::vec3(1.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2(0, 0), glm::vec2(250, 88), ResourceManager::GetTexture("ui_btn_timeatk")));
        this->Buttons.push_back(UIButton(glm::vec3(2.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2(0, 98), glm::vec2(250, 88), ResourceManager::GetTexture("ui_btn_endless")));
    }
    else if (level == PLAY_LV)
    {
        this->Currentlevel = PLAY_LV;
        this->CurrentPlayState = PLAY;
    }
}

GLvoid Game::ResetGame()
{
    this->Score = 0;
    this->Time = 0;
    this->Lives = 0;
    this->Pawn.clear();
    this->CurrentPlayState = PLAY;
    this->PlayTimer = 0.0f;
    this->NextTimeSpawn = 0.0f;
    this->ResetColorID();
    ResourceManager::DeleteTexture("theme_background");
    ResourceManager::DeleteTexture("theme_pawn1");
    ResourceManager::DeleteTexture("theme_pawn2");
    ResourceManager::DeleteTexture("theme_pawn3");
}

GLvoid Game::ResetColorID()
{
    this->RSCID_red = 1.0f;
    this->RSCID_green = 0.0f;
    this->RSCID_blue = 0.0f;
}

GLvoid Game::LoadGameTheme(GLchar *PathToGameTheme)
{
    std::cout << PathToGameTheme << std::endl;

    GLchar pathtoxml[256] = "";
    strcpy(pathtoxml, PathToGameTheme);
    strcat(pathtoxml, "main.xml");

    std::cout << pathtoxml << std::endl;

    GLchar pathtobackground[256] = "";
    strcpy(pathtobackground, PathToGameTheme);
    strcat(pathtobackground, "bg/bg.png");

    std::cout << pathtobackground << std::endl;

    GLchar pathtopawn1[256] = "";
    strcpy(pathtopawn1, PathToGameTheme);
    strcat(pathtopawn1, "pawn/1.png");

    GLchar pathtopawn2[256] = "";
    strcpy(pathtopawn2, PathToGameTheme);
    strcat(pathtopawn2, "pawn/2.png");

    GLchar pathtopawn3[256] = "";
    strcpy(pathtopawn3, PathToGameTheme);
    strcat(pathtopawn3, "pawn/3.png");

    GLchar pathtoslow[256] = "";
    strcpy(pathtoslow, PathToGameTheme);
    strcat(pathtoslow, "powerup/slow.png");

    // Get XML information
    FILE *fp;
    mxml_node_t *tree;

    fp = fopen(pathtoxml, "r");
    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    fclose(fp);

    mxml_node_t *classnode;

    //int i = 0;

    for (classnode = mxmlFindElement(tree, tree, "class", NULL, NULL, MXML_DESCEND); classnode != NULL; classnode = mxmlFindElement(classnode, tree, "class", NULL, NULL, MXML_DESCEND))
    {
        printf("Class: %s\n", classnode->child->value.text.string);
        //i++;
    }

    mxmlDelete(tree);

    // Theme loaded
    ResourceManager::LoadTexture(pathtobackground, GL_FALSE, "theme_background");
    ResourceManager::LoadTexture(pathtopawn1, GL_TRUE, "theme_pawn1");
    pawnthemesize[0][0] = ResourceManager::GetSizeTexture("theme_pawn1", WIDTH);
    pawnthemesize[0][1] = ResourceManager::GetSizeTexture("theme_pawn1", HEIGHT);
    ResourceManager::LoadTexture(pathtopawn2, GL_TRUE, "theme_pawn2");
    pawnthemesize[1][0] = ResourceManager::GetSizeTexture("theme_pawn2", WIDTH);
    pawnthemesize[1][1] = ResourceManager::GetSizeTexture("theme_pawn2", HEIGHT);
    ResourceManager::LoadTexture(pathtopawn3, GL_TRUE, "theme_pawn3");
    pawnthemesize[2][0] = ResourceManager::GetSizeTexture("theme_pawn3", WIDTH);
    pawnthemesize[2][1] = ResourceManager::GetSizeTexture("theme_pawn3", HEIGHT);
    ResourceManager::LoadTexture(pathtoslow, GL_TRUE, "theme_slow");
    pawnthemesize[3][0] = ResourceManager::GetSizeTexture("theme_slow", WIDTH);
    pawnthemesize[3][1] = ResourceManager::GetSizeTexture("theme_slow", HEIGHT);
}

GLvoid Game::RenderLoading(std::string text, GLfloat size)
{
    this->Buttons.clear();
    this->ResetColorID();
    SpriteRenderer->Draw(ResourceManager::GetTexture("background"), glm::vec2(0, -30), glm::vec2(this->windowWidth, this->windowHeight + 60), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    TextRenderer->RenderText(text, this->windowWidth / 2, this->windowHeight / 2, size, glm::vec3(255.0f / 255.0f, 28.0f / 255.0f, 202.0f / 255.0f));
    glfwSwapBuffers(Getwindow());
}

GLvoid de_allocatethemepreview()
{
    for (int i = 0; i < themecount; i++)
    {
        ResourceManager::DeleteTexture(temptheme[i]);
    }
}