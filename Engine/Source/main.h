#include "game.h"
#include "resourcemanager.h"

enum GLFWCursorType { ARROW, POINTER };

// Window access function
GLFWwindow* GetWindow();

// Set window cursor
GLvoid SetWindowCursor(GLFWCursorType type);