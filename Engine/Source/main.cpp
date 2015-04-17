//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "main.h"

// Game declaration
Game Hittheshapes(1024, 768);

// Window declaration
GLFWwindow* window;
GLFWcursor* cursor_arrow;
GLFWcursor* cursor_pointer;
GLvoid BuildWindow();

// GLFW functions decleration
GLvoid key_callback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mode);
GLvoid mouse_callback(GLFWwindow* window, GLint button, GLint action, GLint mods);

int main(int argc, char **argv)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    BuildWindow();
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    ALCcontext *context;
    ALCdevice *device;

    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    alGetError();
    alutInit(&argc, argv);

    cursor_arrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    cursor_pointer = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    SetWindowCursor(ARROW);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    // Configure OpenGL
    glViewport(0, 0, Hittheshapes.windowWidth, Hittheshapes.windowHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // DeltaTime
    GLfloat currentFrame;
    GLfloat DeltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    Hittheshapes.init();

    while (!glfwWindowShouldClose(window))
    {
        // Calculate DeltaTime
        currentFrame = glfwGetTime();
        DeltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Clear color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Process Input Events
        glfwPollEvents();
        Hittheshapes.ProcessInput();

        // Update Game
        Hittheshapes.Update(DeltaTime);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Hittheshapes.Render(DeltaTime);

        // Swap to current buffer
        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();
    alutExit();
    context = alcGetCurrentContext();
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    glfwTerminate();
    return 0;
}

GLvoid BuildWindow()
{
    window = glfwCreateWindow(Hittheshapes.windowWidth, Hittheshapes.windowHeight, "Hit the shapes!", nullptr, nullptr);
}

GLFWwindow* GetWindow()
{
    return window;
}

GLvoid key_callback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mode)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Hittheshapes.Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
        {
            Hittheshapes.Keys[key] = GL_FALSE;
            Hittheshapes.Keysprocessed[key] = GL_FALSE;
        }
    }
}

GLvoid mouse_callback(GLFWwindow* window, GLint button, GLint action, GLint mods)
{
    if (button >= 0 && button <= 8)
    {
        if (action == GLFW_PRESS)
            Hittheshapes.Keys[button] = GL_TRUE;
        else if (action == GLFW_RELEASE)
        {
            Hittheshapes.Keys[button] = GL_FALSE;
            Hittheshapes.Keysprocessed[button] = GL_FALSE;
        }
    }
}

GLvoid SetWindowCursor(GLFWCursorType type)
{
    if (type == ARROW) glfwSetCursor(window, cursor_arrow);
    else if (type == POINTER) glfwSetCursor(window, cursor_pointer);
}