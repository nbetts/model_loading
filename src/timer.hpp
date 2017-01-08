/**
 * [Program description]
 */

#ifndef TIMER_HEADER
#define TIMER_HEADER

class Timer
{
  public:
    GLfloat currentTime;
    GLfloat lastTime;
    GLfloat deltaTime;   

    Timer();
    GLvoid update(GLfloat time);
};

Timer::Timer()
{
  currentTime = 0.0f;
  lastTime = 0.0f;
  deltaTime = 0.0f;
}

GLvoid Timer::update(GLfloat time)
{
  lastTime = currentTime;
  currentTime = time;
  deltaTime = currentTime - lastTime;
}
  
#endif
