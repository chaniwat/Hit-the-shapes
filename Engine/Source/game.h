#ifndef GAME
#define GAME

#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ui_button.h"
#include "game_pawn.h"
#include "obj_rectangle.h"

enum GameLevel { MENU_LV, THEME_LV, MODE_LV, PLAY_LV, SCORE_LV };
enum GameMode { TIME_ATTACK, ENDLESS };
enum PlayState { PLAY, PAUSE, END };

class Game
{
    public:
        // Game State
        GameLevel Currentlevel;
        GLuint Currenttheme;
        GameMode Currentmode;
        PlayState CurrentPlayState;
        GLuint Score;
        GLuint HighscoreTIME, HighscoreEND;
        GLint Lives;
        GLfloat Time;
        GLboolean SlowMode = GL_FALSE;
        GLfloat SlowTime = 0.0f;
        GLboolean MultiMode = GL_FALSE;
        GLfloat MultiTime = 0.0f;
        GLboolean DestroyMode = GL_FALSE;
        GLfloat RSCID_red = 1.0f, RSCID_green = 0.0f, RSCID_blue = 0.0f; // RSCID - Random Spawn ColorID - Used for click on object checking
        std::vector<UIButton> Buttons;
        std::vector<GamePawn> Pawn;
        std::vector<OBJRectangle> RectanglePawn;
        // Draw level
        GLvoid DrawCurrentLevel(GLfloat dt);
        // Pawn in game
        GLvoid SpawnPawn(GLfloat dt);
        // Game State
        GLboolean Keys[1024], Keysprocessed[1024];
        // Game Configure
        GLuint windowWidth, windowHeight;
        // Constructor/Deconstructor
        Game(GLuint width, GLuint height);
        ~Game();
        // Initialize game state (shaders/textures/sounds/levels)
        GLvoid init();
        // Initialize game theme
        GLvoid LoadGameTheme(GLchar *PathToGameTheme);
        // GameLoop
        GLvoid ProcessInput();
        GLvoid Update(GLfloat dt);
        GLvoid Render(GLfloat dt);
        // Reset
        GLvoid ResetGame();
        // Change Level
        GLvoid ChangeLevel(GameLevel level);
        // Render loading
        GLvoid RenderLoading();
        // Restart Play
        GLvoid RestartPlay();
        // Load highscore
        GLvoid LoadHighscore();
        // Save highscore
        GLvoid SaveHighscore();
    private:
        const GLfloat TimeSpawnBase = 1.75f;
        GLfloat NextTimeSpawn = 0.0f;
        GLfloat PlayTimer = 0.0f;
        GLvoid ResetColorID();
};

#endif GAME