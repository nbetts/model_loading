/**
 * [Program description]
 */

// Statically link with GLEW.
#define GLEW_STATIC

// System headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "helpers.hpp"
#include "camera.cpp"
#include "model.cpp"
#include "shader.cpp"
#include "timer.hpp"

#define true  1
#define false 0
#define DEFAULT_WINDOW_WIDTH  1200
#define DEFAULT_WINDOW_HEIGHT 675

GLvoid initialiseAll();
GLvoid keyboard(GLFWwindow* window, GLint key, GLint scancode,
                GLint action, GLint mode);
GLvoid mouseMovement(GLFWwindow* window, GLdouble x, GLdouble y);
GLvoid mouseScroll(GLFWwindow* window, GLdouble x, GLdouble y);
GLvoid initialiseEnvironment();
GLvoid initialiseCamera();
GLvoid initialiseModel();
GLvoid moveCamera();
GLvoid runMainLoop();
GLvoid initialiseGraphics(GLint argc, GLchar* argv[]);
GLvoid terminateGraphics();
GLint main(GLint argc, GLchar* argv[]);