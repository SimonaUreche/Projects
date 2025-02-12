#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, glm::vec3 cameraFrontDirection, glm::vec3 cameraRightDirection) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = glm::normalize(cameraUp);
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)); //necesare pt. calculul miscarilor
        //TODO - Update the rest of camera parameters

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD) {
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
        }
        else if (direction == MOVE_BACKWARD) {
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
        }
        else if (direction == MOVE_LEFT) {
            cameraPosition -= glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
            cameraTarget -= cameraRightDirection * speed;
        }
        else if (direction == MOVE_RIGHT) {
            cameraPosition += glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
            cameraTarget += cameraRightDirection * speed;
        } 
        else if (direction == MOVE_UP) {
            cameraPosition += cameraUpDirection * speed;
            cameraTarget += cameraUpDirection * speed;
        }
        else if (direction == MOVE_DOWN) {
            cameraPosition -= cameraUpDirection * speed;
            cameraTarget -= cameraUpDirection * speed;
        }
    }

    void Camera::setPosition(const glm::vec3& newPos) {
        cameraPosition = newPos;
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition); 
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }

    // Funcție pentru a seta targetul camerei
    void Camera::setTarget(const glm::vec3& newTarget) {
        cameraTarget = newTarget;
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition); 
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }
    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        // Cumulează unghiurile de rotație pentru a păstra continuitatea rotațiilor
        pitch += pitch;
        yaw += yaw;

        // Limitează pitch-ul pentru a evita gimbal lock (de exemplu, -89° la 89°)
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        // Calculează direcția camerei folosind yaw și pitch
        glm::vec3 vec;
        vec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        vec.y = sin(glm::radians(pitch));
        vec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(vec);

    }
}