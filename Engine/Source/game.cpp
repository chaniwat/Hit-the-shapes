#include "main.h"
#include "game.h"
#include "resourcemanager.h"
#include "spriterender.h"
#include "textrender.h"
//#include "sound.h"

SpriteRender *SpriteRenderer;
SpriteRender *ColorIDRenderer;
TextRender *TextRenderer;

static GLchar pathlist[256][128];
static GLint themecount = 0;
static GLchar temptheme[256][256];
static GLint themepage = 1;
static GLint thememaxpage = 1;

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
    ResourceManager::LoadTexture("../Images/milkyway-galaxy-bg.jpg", GL_FALSE, "background");
    // UI Button loaded
    ResourceManager::LoadTexture("../Images/Button/btn_play.png", GL_TRUE, "ui_btn_play");
    ResourceManager::LoadTexture("../Images/Button/btn_timeatk.png", GL_TRUE, "ui_btn_timeatk");
    ResourceManager::LoadTexture("../Images/Button/btn_endless.png", GL_TRUE, "ui_btn_endless");
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
    if (this->Currentlevel == PLAY_LV)
    {
        if (this->Currentmode == TIME_ATTACK) 
        {
            if (this->CurrentPlayState == PLAY && this->Time > 0)
            {
                this->Time -= dt;
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
            else if (this->Lives < 0)
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
                itr.aliveTime -= dt;
                if (itr.aliveTime <= 0) 
                {
                    if (this->Currentmode == ENDLESS) this->Lives -= 1;
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
    if (this->Keys[GLFW_MOUSE_BUTTON_LEFT] && !this->Keysprocessed[GLFW_MOUSE_BUTTON_LEFT])
    {
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

                // Read Color pixel at cursor
                GLint viewport[4];
                GLubyte pixel[3];
                GLdouble xpos, ypos;
                glGetIntegerv(GL_VIEWPORT, viewport);
                glfwGetCursorPos(Getwindow(), &xpos, &ypos);
                glReadPixels(xpos, viewport[3] - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixel);
                std::cout << "X: " << xpos << " Y: " << ypos << std::endl;
                std::cout << "R: " << (int)pixel[0] << " G: " << (int)pixel[1] << " B: " << (int)pixel[2] << std::endl;

                // Check what object is cursor over and do something
                for (GamePawn &itr : this->Pawn)
                {
                    if (itr.ColorID.r * 255.0f == pixel[0] && itr.ColorID.g * 255.0f == pixel[1] && itr.ColorID.b * 255.0f == pixel[2])
                    {
                        // Destroy pawn and add score
                        std::cout << "You get: " << itr.Score << " scores for clicking." << std::endl;
                        this->Score += itr.Score;
                        itr.isDestroyed = GL_TRUE;
                    }
                }
            }
        }
        else
        {
            // Render the color ID
            for (UIButton &itr : this->Buttons)
            {
                itr.DrawColorID(*ColorIDRenderer);
            }

            // Read Color pixel at cursor
            GLint viewport[4];
            GLubyte pixel[3];
            GLdouble xpos, ypos;
            glGetIntegerv(GL_VIEWPORT, viewport);
            glfwGetCursorPos(Getwindow(), &xpos, &ypos);
            glReadPixels(xpos, viewport[3] - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixel);
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
                            this->ChangeLevel(THEME_LV);
                        }
                    }
                    else if (this->Currentlevel == THEME_LV)
                    {
                        if (itr.ColorID.r * 255.0f >= 1.0f && itr.ColorID.g * 255.0f == 0.0f && itr.ColorID.b * 255.0f == 0.0f)
                        {
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
        if (this->Currentlevel == MENU_LV) SpriteRenderer->Draw(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->windowWidth, this->windowHeight), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        else SpriteRenderer->Draw(ResourceManager::GetTexture("theme_background"), glm::vec2(0, 0), glm::vec2(this->windowWidth, this->windowHeight), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        for (UIButton &itr : this->Buttons)
        {
            itr.Draw(*SpriteRenderer);
        }
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
    if (this->Currentmode == TIME_ATTACK)
    {
        this->PlayTimer += dt;

        // Random spawn by time
        if (this->PlayTimer > this->NextTimeSpawn)
        {
            for (GLfloat i = 0.0f; i < PlayTimer / 5; i++)
            {
                GLfloat x_pos = std::rand() % (this->windowWidth - 100 + 1);
                GLfloat y_pos = std::rand() % (this->windowHeight - 100 + 1);
                GLfloat percentofrarespawn = std::rand() % 10000;
                if (percentofrarespawn >= 0 && percentofrarespawn <= 6000)
                {
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(100, 100), ResourceManager::GetTexture("theme_pawn1")));
                }
                else if (percentofrarespawn >= 6001 && percentofrarespawn <= 8500)
                {
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 2.5, 3, glm::vec2(x_pos, y_pos), glm::vec2(80, 80), ResourceManager::GetTexture("theme_pawn2")));
                }
                else if (percentofrarespawn >= 8501 && percentofrarespawn <= 9999)
                {
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 1.75, 5, glm::vec2(x_pos, y_pos), glm::vec2(60, 60), ResourceManager::GetTexture("theme_pawn3")));
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
                GLfloat x_pos = std::rand() % (this->windowWidth - 100 + 1);
                GLfloat y_pos = std::rand() % (this->windowHeight - 100 + 1);
                GLfloat percentofrarespawn = std::rand() % 10000;
                if (percentofrarespawn >= 0 && percentofrarespawn <= 6000)
                {
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(100, 100), ResourceManager::GetTexture("theme_pawn1")));
                }
                else if (percentofrarespawn >= 6001 && percentofrarespawn <= 8500)
                {
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(100, 100), ResourceManager::GetTexture("theme_pawn2")));
                }
                else if (percentofrarespawn >= 8501 && percentofrarespawn <= 9999)
                {
                    this->Pawn.push_back(GamePawn(glm::vec3(this->RSCID_red / 255.0f, this->RSCID_green / 255.0f, this->RSCID_blue / 255.0f), 3, 1, glm::vec2(x_pos, y_pos), glm::vec2(100, 100), ResourceManager::GetTexture("theme_pawn3")));
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

        this->Buttons.push_back(UIButton(glm::vec3(1.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2(0, 0), glm::vec2(250, 88), ResourceManager::GetTexture("ui_btn_play")));
    }
    else if (level == THEME_LV)
    {
        this->Currentlevel = THEME_LV;

        strcpy(pathlist[0], "");
        themecount = 0;
        
        // Open files
        std::ifstream themelistsfile("../Theme/themes.txt");
        // Read file's buffer contents into streams
        std::stringstream themelistsfileStream;
        themelistsfileStream << themelistsfile.rdbuf();
        // close file handlers
        themelistsfile.close();
        // Convert stream into string
        std::string themelistsA;
        themelistsA = themelistsfileStream.str();
        const GLchar *themelistB = themelistsA.c_str();

        GLchar themename[128] = "";
        for (int i = 0; i < strlen(themelistB); i++)
        {
            if (themelistB[i] != '\n' && themelistB[i] != '\0')
            {
                GLchar t[2] = "";
                sprintf(t, "%c", themelistB[i]);
                strcat(themename, t);
            }
            if (themelistB[i] == '\n' || themelistB[i] == '\0' || i == strlen(themelistB) - 1)
            {
                strcat(themename, "/");

                strcpy(pathlist[themecount + 1], "../Theme/");
                strcat(pathlist[themecount + 1], themename);

                GLchar pathtopreview[256] = "";
                strcpy(pathtopreview, pathlist[themecount + 1]);
                strcat(pathtopreview, "preview.png");

                strcpy(temptheme[themecount], "");
                sprintf(temptheme[themecount], "Theme%03d", themecount);

                ResourceManager::LoadTexture(pathtopreview, GL_TRUE, temptheme[themecount]);
                this->Buttons.push_back(UIButton(glm::vec3(this->RSCID_red++ / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f), glm::vec2(300 * ((themecount % 9) % 3), (225 * (((themecount % 9) / 3) % 3))), glm::vec2(300, 225), ResourceManager::GetTexture(temptheme[themecount])));

                themecount++;
                strcpy(themename, "");
            }
        }

        themecount--;
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
    this->Time = 30.0f;
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

    // Theme loaded
    ResourceManager::LoadTexture(pathtobackground, GL_FALSE, "theme_background");
    ResourceManager::LoadTexture(pathtopawn1, GL_TRUE, "theme_pawn1");
    ResourceManager::LoadTexture(pathtopawn2, GL_TRUE, "theme_pawn2");
    ResourceManager::LoadTexture(pathtopawn3, GL_TRUE, "theme_pawn3");
}

GLvoid de_allocatethemepreview()
{
    for (int i = 0; i < themecount; i++)
    {
        ResourceManager::DeleteTexture(temptheme[i]);
    }
}