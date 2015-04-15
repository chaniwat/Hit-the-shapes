#include "game.h"
#include "resourcemanager.h"

enum Cursortype { ARROW, POINTER };

GLvoid Makewindow();
GLFWwindow* Getwindow();

GLvoid SetWindowCursor(Cursortype type);