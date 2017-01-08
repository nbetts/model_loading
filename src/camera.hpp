/**
 * [Program description]
 */

#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <glm/glm.hpp>

class Camera
{
  public:
    typedef enum {
      FORWARD,
      BACKWARD,
      LEFT,
      RIGHT
    } Direction;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    glm::mat4 view;
    glm::mat4 projection;

    GLfloat yaw;
    GLfloat pitch;
    GLfloat movementSpeed;
    GLfloat turnSensitivity;
    GLfloat fov;
    GLfloat aspectRatio;
    GLfloat near;
    GLfloat far;
    GLfloat zoomMultiplier;

    Camera(glm::vec3 desiredPosition = glm::vec3(0.0f),
           glm::vec3 desiredUp = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat desiredYaw = 0.0f, GLfloat desiredPitch = 0.0f,
           GLfloat desiredMovementSpeed = 1.0f,
           GLfloat desiredTurnSensitivity = 1.0f,
           GLfloat desiredFov = 45.0f, GLfloat desiredAspectRatio = 1.0f,
           GLfloat desiredNear = 0.1f, GLfloat desiredFar = 1000.0f,
           GLfloat desiredZoomMultiplier = 40.0f);
    GLvoid updatePerspective();
    GLvoid updateLookAtMatrix();
    GLvoid updatePosition(Direction direction, GLfloat deltaTime);
    GLvoid updateOrientation(GLfloat deltaYaw, GLfloat deltaPitch);
    GLvoid setPitch(GLfloat desiredPitch);
    GLvoid setFov(GLfloat desiredFov);
    GLvoid updateFov(GLfloat deltaFov);
    GLvoid print();
};

#endif
