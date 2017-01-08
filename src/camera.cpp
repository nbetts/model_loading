/**
 * [Program description]
 */

#include "camera.hpp"

/**
 * Constructor to create and set the attributes of the camera.
 */
Camera::Camera(glm::vec3 desiredPosition, glm::vec3 desiredUp,
               GLfloat desiredYaw, GLfloat desiredPitch,
               GLfloat desiredMovementSpeed, GLfloat desiredTurnSensitivity,
               GLfloat desiredFov, GLfloat desiredAspectRatio,
               GLfloat desiredNear, GLfloat desiredFar,
               GLfloat desiredZoomMultiplier)
{
  view = glm::mat4();
  projection = glm::mat4();
  position = desiredPosition;
  up = desiredUp;
  yaw = desiredYaw;
  setPitch(desiredPitch);
  movementSpeed = desiredMovementSpeed;
  turnSensitivity = desiredTurnSensitivity;
  aspectRatio = desiredAspectRatio;
  near = desiredNear;
  far = desiredFar;
  setFov(desiredFov);
  zoomMultiplier = desiredZoomMultiplier;

  updateOrientation(0.0f, 0.0f);
}

GLvoid Camera::updatePerspective()
{
  projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

GLvoid Camera::updateLookAtMatrix()
{
  view = glm::lookAt(position, position + front, up);
}

GLvoid Camera::updatePosition(Direction direction, GLfloat distance)
{
  switch(direction) {
    case Camera::FORWARD:
      position += front * distance;
      break;
    case Camera::BACKWARD:
      position -= front * distance;
      break;
    case Camera::LEFT:
      position -= glm::normalize(glm::cross(front, up)) * distance;
      break;
    case Camera::RIGHT:
      position += glm::normalize(glm::cross(front, up)) * distance;
      break;
  }

  updateLookAtMatrix();
}

GLvoid Camera::updateOrientation(GLfloat deltaYaw, GLfloat deltaPitch)
{
  yaw += deltaYaw * (fov / zoomMultiplier) * turnSensitivity;
  setPitch(pitch + (deltaPitch * (fov / zoomMultiplier) * turnSensitivity));

  front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  front.y = sin(glm::radians(pitch));
  front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  front = glm::normalize(front);

  updateLookAtMatrix();
}

GLvoid Camera::setPitch(GLfloat desiredPitch)
{
  pitch = desiredPitch;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  } else if (pitch < -89.0f) {
    pitch = -89.0f;
  }
}

GLvoid Camera::setFov(GLfloat desiredFov)
{
  fov = desiredFov;

  if (fov <= 0.1f) {
    fov = 0.1f;
  } else if (fov >= 180.0f) {
    fov = 180.0f;
  }

  updatePerspective();
}

GLvoid Camera::updateFov(GLfloat deltaFov)
{
  setFov(fov + (deltaFov * (fov / zoomMultiplier)));
}

GLvoid Camera::print()
{
  printf("Camera\np: %.3f %.3f %.3f\nyaw: %.3f, pitch: %.3f, fov: %.3f\naspectRatio: %.3f, near: %.3f, far: %.3f, zoomMultiplier: %.3f\n",
         position.x, position.y, position.z, yaw, pitch, fov,
         aspectRatio, near, far, zoomMultiplier);
}