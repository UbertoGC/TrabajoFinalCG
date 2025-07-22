#ifndef __CONTROLES_H__
#define __CONTROLES_H__
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class User{
private:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float X;
    float Y;
    float yaw;
    float pitch;
    float fov;
    bool firstMouse;
public:
    User();
    glm::mat4 process_vista();
};
User::User() {
    glm::vec3 centro(0.0f, 0.0f, 0.0f);
    cameraPos = centro + glm::vec3(0.0f, 0.0f, 600.0f);
    cameraFront = glm::normalize(centro - cameraPos);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    X = 400, Y = 300;
    yaw = -90.0f, pitch = 0.0f;
    fov = 45.0f;
    firstMouse = true;
}
glm::mat4 User::process_vista(){
    return glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
}
#endif