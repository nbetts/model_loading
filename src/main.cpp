/**
 * [Program description]
 */

#include "main.hpp"

// System info
GLFWwindow* window;
GLint frameWidth, frameHeight;
GLfloat aspectRatio;

// environment info
const GLchar* profile;
std::map<std::string, GLfloat> env;

// keyboard info
GLuint keyPressed[512];

// mouse info
GLfloat lastMouseX = DEFAULT_WINDOW_WIDTH / 2.0f;
GLfloat lastMouseY = DEFAULT_WINDOW_HEIGHT / 2.0f;
GLuint initialiseMouseMovement = true;

// time info
Timer timer;

// camera info
Camera camera;

// scene info
glm::vec3 backgroundColour(0.0f);
glm::vec3 lightPosition(0.0f);
GLuint isPointLightingEnabled;
GLuint isCullingEnabled;
GLuint areFacesEnabled;
GLuint areNormalsEnabled;
GLuint isWireframeEnabled;
GLuint isOutlineEnabled;
glm::vec4 wireframeColour;
glm::vec4 outlineColour;
GLfloat shineValue = 1.0f;
GLfloat normalLength;
GLfloat outlineSize;

Shader simpleShader, normalShader, outlineShader;
Model featureModel, lightModel;
std::string featureModelPath, lightModelPath;

/**
 * Listen for keyboard events.
 */
GLvoid keyboard(GLFWwindow* window, GLint key, GLint scancode,
                GLint action, GLint mode)
{
  // Store the action state of the given key.
  keyPressed[key] = action;

  if (action != GLFW_PRESS) {
    return;
  }

  switch(key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
    case GLFW_KEY_1:
      initialiseCamera();
      break;
    case GLFW_KEY_F:
      areFacesEnabled = !areFacesEnabled;
      env["areFacesEnabled"] = !env["areFacesEnabled"];
      break;
    case GLFW_KEY_C:
      isCullingEnabled = !isCullingEnabled;
      env["isCullingEnabled"] = isCullingEnabled;
      break;
    case GLFW_KEY_N:
      areNormalsEnabled = !areNormalsEnabled;
      env["areNormalsEnabled"] = areNormalsEnabled;
      break;
    case GLFW_KEY_X:
      isWireframeEnabled = !isWireframeEnabled;
      env["isWireframeEnabled"] = isWireframeEnabled;
      break;
    case GLFW_KEY_V:
      isOutlineEnabled = !isOutlineEnabled;
      env["isOutlineEnabled"] = isOutlineEnabled;
      break;
    case GLFW_KEY_P:
      isPointLightingEnabled = !isPointLightingEnabled;
      env["isPointLightingEnabled"] = isPointLightingEnabled;
      break;
    case GLFW_KEY_Z:
      shineValue = -shineValue;
      break;
  }
}

/**
 * Listen for mouse events.
 */
GLvoid mouseMovement(GLFWwindow* window, GLdouble x, GLdouble y)
{
  // Set the last x and y values for the mouse's initial movement capture.
  if (initialiseMouseMovement) {
    lastMouseX = x;
    lastMouseY = y;
    initialiseMouseMovement = false;
  }

  GLfloat deltaX = x - lastMouseX;
  GLfloat deltaY = lastMouseY - y;
  lastMouseX = x;
  lastMouseY = y;
  camera.updateOrientation(deltaX, deltaY);
}

/**
 * Listen for mouse scroll events.
 */
GLvoid mouseScroll(GLFWwindow* window, GLdouble x, GLdouble y)
{
  camera.updateFov(y);
}

/**
 * Initialise the environment properties, camera and fractal.
 */
GLvoid initialiseEnvironment()
{
  env = readProfile(profile);

  backgroundColour.r = env["backgroundColourRed"];
  backgroundColour.g = env["backgroundColourGreen"];
  backgroundColour.b = env["backgroundColourBlue"];
  lightPosition.x = env["lightPositionX"];
  lightPosition.y = env["lightPositionY"];
  lightPosition.z = env["lightPositionZ"];
  isPointLightingEnabled = env["isPointLightingEnabled"];

  areFacesEnabled = env["areFacesEnabled"];
  areNormalsEnabled = env["areNormalsEnabled"];
  isWireframeEnabled = env["isWireframeEnabled"];
  isOutlineEnabled = env["isOutlineEnabled"];
  isCullingEnabled = env["isCullingEnabled"];
  normalLength = env["normalLength"];
  outlineSize = env["outlineSize"] / 100.0f;

  wireframeColour = glm::vec4(env["wireframeColourRed"],
                             env["wireframeColourGreen"],
                             env["wireframeColourBlue"],
                             env["wireframeColourAlpha"]);
  outlineColour = glm::vec4(env["outlineColourRed"],
                            env["outlineColourGreen"],
                            env["outlineColourBlue"],
                            env["outlineColourAlpha"]);
}

/**
 * Initialise the camera attributes.
 */
GLvoid initialiseCamera()
{
  camera = Camera(glm::vec3(-0.326f, 1.617f, 0.837f),
                  glm::vec3(0.0f, 1.0f, 0.0f),
                  290.0f, -30.0f,
                  env["cameraMovementSpeed"],
                  env["cameraTurnSensitivity"],
                  env["cameraFov"],
                  aspectRatio, 0.01f, 100.0f, 40.0f);
}

/**
 * Initialise the feature model.
 */
GLvoid initialiseModel()
{
  simpleShader = Shader("src/shaders/model.vert",
                        "src/shaders/model.frag",
                        "src/shaders/model.geom");
  normalShader = Shader("src/shaders/normal.vert",
                        "src/shaders/normal.frag",
                        "src/shaders/normal.geom");
  outlineShader = Shader("src/shaders/outline.vert",
                         "src/shaders/outline.frag",
                         "src/shaders/outline.geom");
  simpleShader.load();
  normalShader.load();
  outlineShader.load();

  featureModel = Model(featureModelPath);
  lightModel = Model(lightModelPath);

  featureModel.load();
  lightModel.load();

  featureModel.normalize(-1.0f, 1.0f);
}

/**
 * Update any camera atrributes before rendering the scene.
 */
GLvoid moveCamera()
{
  GLfloat distance = camera.movementSpeed * timer.deltaTime;

  if (keyPressed[GLFW_KEY_W]) {
    camera.updatePosition(Camera::FORWARD, distance);
  }
  if (keyPressed[GLFW_KEY_S]) {
    camera.updatePosition(Camera::BACKWARD, distance);
  }
  if (keyPressed[GLFW_KEY_A]) {
    camera.updatePosition(Camera::LEFT, distance);
  }
  if (keyPressed[GLFW_KEY_D]) {
    camera.updatePosition(Camera::RIGHT, distance);
  }
}

GLvoid drawModel()
{
  using namespace glm;

  mat4 model;

  GLuint modelLoc, viewLoc, projectionLoc, viewPosLoc;
  GLuint facesLoc, wireframeLoc, wireframeColourLoc, normalLengthLoc;
  GLuint matShineLoc, outlineSizeLoc, outlineColourLoc;
  GLuint lightPositionLoc, lightAmbientLoc, lightDiffuseLoc, lightSpecularLoc;

  // Draw the feature model.
  simpleShader.use();

  viewPosLoc  = glGetUniformLocation(simpleShader.id, "viewPosition");
  modelLoc      = glGetUniformLocation(simpleShader.id, "model");
  viewLoc       = glGetUniformLocation(simpleShader.id, "view");
  projectionLoc = glGetUniformLocation(simpleShader.id, "projection");

  glUniform4f(viewPosLoc, camera.position.x,
              camera.position.y, camera.position.z, 1.0f);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera.view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(camera.projection));

  // Material uniforms
  matShineLoc    = glGetUniformLocation(simpleShader.id, "material.shininess"); 
  glUniform1f(matShineLoc, shineValue);

  // Light uniforms
  lightPositionLoc = glGetUniformLocation(simpleShader.id, "light.position");
  lightAmbientLoc  = glGetUniformLocation(simpleShader.id, "light.ambient");
  lightDiffuseLoc  = glGetUniformLocation(simpleShader.id, "light.diffuse");
  lightSpecularLoc = glGetUniformLocation(simpleShader.id, "light.specular");

  glUniform1f(glGetUniformLocation(simpleShader.id, "light.constant"),  1.0f);
  glUniform1f(glGetUniformLocation(simpleShader.id, "light.linear"),    0.09f);
  glUniform1f(glGetUniformLocation(simpleShader.id, "light.quadratic"), 0.032f);

  glUniform4f(lightPositionLoc, lightPosition.x,
              lightPosition.y, lightPosition.z, isPointLightingEnabled);
  glUniform3f(lightAmbientLoc,  0.3f, 0.3f, 0.3f);
  glUniform3f(lightDiffuseLoc, 1.0f, 1.0f, 1.0f);
  glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

  facesLoc = glGetUniformLocation(simpleShader.id, "areFacesEnabled");
  wireframeLoc = glGetUniformLocation(simpleShader.id, "isWireframeEnabled");
  wireframeColourLoc = glGetUniformLocation(simpleShader.id, "wireframeColour");
  glUniform1f(facesLoc, areFacesEnabled);
  glUniform1f(wireframeLoc, isWireframeEnabled);
  glUniform4f(wireframeColourLoc, wireframeColour.r, wireframeColour.g,
                                  wireframeColour.b, wireframeColour.a);
  
  if (isOutlineEnabled) {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
  }
  
  featureModel.draw(simpleShader, isCullingEnabled);

  if (areNormalsEnabled) {
    normalShader.use();

    modelLoc      = glGetUniformLocation(normalShader.id, "model");
    viewLoc       = glGetUniformLocation(normalShader.id, "view");
    projectionLoc = glGetUniformLocation(normalShader.id, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera.view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,value_ptr(camera.projection));

    normalLengthLoc = glGetUniformLocation(normalShader.id, "normalLength");
    glUniform1f(normalLengthLoc, normalLength);
    
    featureModel.draw(normalShader, isCullingEnabled);
  }

  if (isOutlineEnabled) {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    outlineShader.use();

    modelLoc      = glGetUniformLocation(outlineShader.id, "model");
    viewLoc       = glGetUniformLocation(outlineShader.id, "view");
    projectionLoc = glGetUniformLocation(outlineShader.id, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera.view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,value_ptr(camera.projection));

    outlineSizeLoc = glGetUniformLocation(outlineShader.id, "outlineSize");
    glUniform1f(outlineSizeLoc, outlineSize);

    outlineColourLoc = glGetUniformLocation(outlineShader.id, "outlineColour");
    glUniform4f(outlineColourLoc, outlineColour.r, outlineColour.g,
                                  outlineColour.b, outlineColour.a);
    
    featureModel.draw(outlineShader, isCullingEnabled);

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
  }

  // Draw the light model.
  simpleShader.use();

  model = mat4();
  model = translate(model, vec3(lightPosition.x,
                    lightPosition.y, lightPosition.z));
  model = scale(model, vec3(0.1f));

  modelLoc = glGetUniformLocation(simpleShader.id, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

  lightModel.draw(simpleShader, isCullingEnabled);
}

/**
 * Run the close event loop. This is where elements are drawn and window
 * events are polled.
 */
GLvoid runMainLoop()
{
  using namespace glm;

  while(!glfwWindowShouldClose(window))
  {
    // Update the timer.
    timer.update(glfwGetTime());

    // Listen for events from the window.
    glfwPollEvents();

    // Update the camera attributes.
    moveCamera();

    if (keyPressed[GLFW_KEY_U]) {
    lightPosition.x += 0.01f;
    }
    if (keyPressed[GLFW_KEY_J]) {
      lightPosition.x -= 0.01f;
    }
    if (keyPressed[GLFW_KEY_O]) {
      lightPosition.z += 0.01f;
    }
    if (keyPressed[GLFW_KEY_L]) {
      lightPosition.z -= 0.01f;
    }
    if (keyPressed[GLFW_KEY_I]) {
      lightPosition.y += 0.01f;
    }
    if (keyPressed[GLFW_KEY_K]) {
      lightPosition.y -= 0.01f;
    }

    // Clear the screen.
    glClearColor(backgroundColour.r, backgroundColour.g,
                 backgroundColour.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Draw functions.
    drawModel();

    glfwSwapBuffers(window);
  }
}

/**
 * Initialise the graphics libraries and window.
 */
GLvoid initialiseGraphics(GLint argc, GLchar* argv[])
{
  GLint majorVersion, minorVersion, revision;
  GLuint isFullscreen = env["isFullScreenEnabled"];

  // Initialise GLFW.
  glfwInit();

  // Initialise the window and force the use of modern OpenGL (v. >= 3.0).
  glfwGetVersion(&majorVersion, &minorVersion, &revision);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLuint width = DEFAULT_WINDOW_WIDTH;
  GLuint height = DEFAULT_WINDOW_HEIGHT;
  GLFWmonitor* monitor = nullptr;

  if (isFullscreen) {
    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
    width = videoMode->width;
    height = videoMode->height;
  }
  window = glfwCreateWindow(width, height, "Model Loading", monitor, nullptr);

  glfwMakeContextCurrent(window);

  // Enable keyboard and mouse input.
  glfwSetKeyCallback(window, keyboard);
  glfwSetCursorPosCallback(window, mouseMovement);
  glfwSetScrollCallback(window, mouseScroll);

  // Initialise GLEW.
  glewExperimental = GL_TRUE;
  glewInit();

  // Define the viewport dimensions.
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
  aspectRatio = (GLfloat)frameWidth / (GLfloat)frameHeight;
  glViewport(0, 0, frameWidth, frameHeight);

  // Set extra options.
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * Terminate the application gracefully by deleting objects and properly
 * closing the window.
 */
GLvoid terminateGraphics()
{
  simpleShader.unload();
  normalShader.unload();
  outlineShader.unload();

  featureModel.unload();
  lightModel.unload();

  glfwTerminate();
}

/**
 * Main method.
 */
GLint main(GLint argc, GLchar* argv[])
{
  // srand(time(nullptr));

  // Read in the profile.
  // profile = (argc >= 2) ? argv[1] : "profile.txt";
  profile = "profile.txt";

  if (argc < 3) {
    printf("To run, provide a feature model path and light model path, e.g:\n./build.sh -x models/nanosuit/nanosuit.obj models/icosphere/icosphere.obj\n");
    return -1;
  } else {
    featureModelPath = std::string(argv[1]);
    lightModelPath = std::string(argv[2]);
  }

  // Initialise the envorinment properties.
  initialiseEnvironment();

  // Initialise the graphics environment.
  initialiseGraphics(argc, argv);

  // Initialise the camera.
  initialiseCamera();
  initialiseModel();

  // Run the graphics loop.
  runMainLoop();

  // Close the application gracefully.
  terminateGraphics();

  return 0;
}
