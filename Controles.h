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
    void mouse_callback(GLFWwindow*, double, double);
    void processInput(GLFWwindow *, float);
    void vistas(int);
    glm::mat4 process_vista();
};
User::User() {
    glm::vec3 centro(250.0f, 235.0f, 68.0f); 
    cameraPos = centro + glm::vec3(0.0f, 0.0f, 600.0f);
    cameraFront = glm::normalize(centro - cameraPos);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    X = 400, Y = 300;
    yaw = -90.0f, pitch = 0.0f;
    fov = 45.0f;
    firstMouse = true;
}
void User::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        this->X = xpos;
        this->Y = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - this->X;
    float yoffset = this->Y - ypos;
    this->X = xpos;
    this->Y = ypos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    this->yaw   += xoffset;
    this->pitch += yoffset;
    if(this->pitch > 89.0f)
        this->pitch = 89.0f;
    if(this->pitch < -89.0f)
        this->pitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->cameraFront = glm::normalize(front);
}
void User::processInput(GLFWwindow *window, float deltaTime) {
    float speed = 100.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        this->cameraPos += speed * this->cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        this->cameraPos -= speed * this->cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        this->cameraPos -= glm::normalize(glm::cross(this->cameraFront, this->cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        this->cameraPos += glm::normalize(glm::cross(this->cameraFront, this->cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        vistas(1);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        vistas(2);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        vistas(3);
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        vistas(4);
}
void User::vistas(int vista) {
    glm::vec3 centro(250.0f, 235.0f, 68.0f); 
    switch (vista) {
        case 1: //Vista superior
            cameraPos = centro + glm::vec3(0, 600, 0);
            cameraFront = glm::normalize(centro - cameraPos);
            break;
        case 2: //Vista frontal
            cameraPos = centro + glm::vec3(0, 0, 600);
            cameraFront = glm::normalize(centro - cameraPos);
            break;
        case 3: //Vista lateral derecha
            cameraPos = centro + glm::vec3(600, 0, 0);
            cameraFront = glm::normalize(centro - cameraPos);
            break;
        case 4: //Vista isométrica
            cameraPos = centro + glm::vec3(400, 400, 400);
            cameraFront = glm::normalize(centro - cameraPos);
            break;
    }
    cameraUp = glm::vec3(0, 1, 0);
    glm::vec3 dir = glm::normalize(cameraFront);
    yaw = glm::degrees(atan2(dir.z, dir.x));
    pitch = glm::degrees(asin(dir.y));
    firstMouse = false;
}
glm::mat4 User::process_vista(){
    return glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
}
#endif