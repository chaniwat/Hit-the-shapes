#include "game_pawn.h"

GamePawn::GamePawn() : ColorID(glm::vec3(0.0f)), aliveTime(5.0f), Score(1), Position(0, 0), Size(1, 1), Color(1.0f), Rotation(0.0f), Sprite(), isDestroyed(GL_FALSE), special(GL_FALSE), specialtype(NONE)
{

}

GamePawn::GamePawn(glm::vec3 colorID, GLfloat alivetime, GLuint score, glm::vec2 pos, glm::vec2 size, Texture2D sprite, GLboolean isspecial, specialpawn type, glm::vec3 color) : ColorID(colorID), aliveTime(alivetime), Score(score), Position(pos), Size(size), Color(color), Rotation(0.0f), Sprite(sprite), isDestroyed(GL_FALSE), special(isspecial), specialtype(type)
{

}

GLvoid GamePawn::Draw(SpriteRender &renderer)
{
    renderer.Draw(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

GLvoid GamePawn::DrawColorID(SpriteRender &renderer)
{
    glDisable(GL_DITHER);
    renderer.Draw(this->Sprite, this->Position, this->Size, this->Rotation, this->ColorID);
    glEnable(GL_DITHER);
}