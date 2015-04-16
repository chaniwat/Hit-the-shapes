#ifndef SPRITE_RENDER
#define SPRITE_RENDER

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

class SpriteRender
{
    private:
        GLuint VAO, VBO, EBO;
        // Initializes and configures
        GLvoid initRenderData();
    public:
        // Render State
        Shader shader;
        // Constructor/Deconstructor
        SpriteRender(Shader &shader);
        ~SpriteRender();
        // Renders a sprite
        GLvoid Draw(Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f), GLfloat alpha = 1.0f, GLfloat hue = 0.0f);
};

#endif SPRITE_RENDER