#include "MathsOps.h"
#include <random>

glm::vec2 MathsOps::TransformCoord(const glm::vec2& coord, float angleInDegrees, float scale) {
    // Convert angle from degrees to radians
    float angleInRadians = glm::radians(angleInDegrees);

    // Create rotation matrix
    glm::mat2 rotationMatrix = glm::mat2(
        cos(angleInRadians), -sin(angleInRadians),
        sin(angleInRadians), cos(angleInRadians)
    );

    // Apply transformation (rotation + scaling)
    return rotationMatrix * (coord * scale);
}

float MathsOps::RndFloat(float min, float max) {
    return min + (rand() / (static_cast<float>(RAND_MAX) + 1.0f)) * (max - min);

}

float MathsOps::Deg2Rad(float deg) {

	return deg * 3.14159265358979323846f / 180.0f;

	//return rad * 180.0f / 3.14159265358979323846f;

}