#ifndef OBJ_RECTANGLE
#define OBJ_RECTANGLE

#include <GL\glew.h>
#include <GLM\glm.hpp>

#include "texture.h"
#include "spriterender.h"

class OBJRectangle
{
public:
    // Pawn State
    glm::vec2 Position, Size;
    glm::vec3 Color, ColorID;
    GLfloat Hue, Rotation;
    // Texture
    GLboolean isTextured = GL_FALSE;
    Texture2D Sprite;
    // Constructor
    OBJRectangle();
    OBJRectangle(glm::vec3 colorID, glm::vec2 pos, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f), GLfloat hue = 0.0f);
    OBJRectangle(glm::vec3 colorID, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), GLfloat hue = 0.0f);
    // Draw sprite
    virtual GLvoid Draw(SpriteRender &renderer);
    // Draw color ID
    virtual GLvoid DrawColorID(SpriteRender &renderer);
};

#endif OBJ_RECTANGLE