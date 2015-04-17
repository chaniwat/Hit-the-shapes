#include "obj_rectangle.h"

OBJRectangle::OBJRectangle() : ColorID(glm::vec3(0.0f)), Position(0, 0), Size(1, 1), Color(1.0f), Rotation(0.0f), Sprite(), Alpha(1.0f), Hue(0.0f)
{

}

OBJRectangle::OBJRectangle(glm::vec3 colorID, glm::vec2 pos, glm::vec2 size, glm::vec3 color, GLfloat alpha, GLfloat hue) : ColorID(colorID), Position(pos), Size(size), Color(color), Rotation(0.0f), Sprite(), Alpha(Alpha), Hue(hue)
{

}

OBJRectangle::OBJRectangle(glm::vec3 colorID, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, GLfloat alpha, GLfloat hue) : ColorID(colorID), Position(pos), Size(size), Color(color), Rotation(0.0f), Sprite(sprite), Alpha(alpha), Hue(hue), isTextured(GL_TRUE)
{

}

GLvoid OBJRectangle::Draw(SpriteRender &renderer)
{
    if (this->isTextured) renderer.shader.SetInteger("isTextured", 1);
    else renderer.shader.SetInteger("isTextured", 0);
    renderer.Draw(this->Sprite, this->Position, this->Size, this->Rotation, this->Color, this->Alpha, this->Hue);
}

GLvoid OBJRectangle::DrawColorID(SpriteRender &renderer)
{
    glDisable(GL_DITHER);
    renderer.Draw(this->Sprite, this->Position, this->Size, this->Rotation, this->ColorID);
    glEnable(GL_DITHER);
}