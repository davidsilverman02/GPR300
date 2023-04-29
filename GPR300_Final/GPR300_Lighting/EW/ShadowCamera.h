#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ShadowCamera {
public:
	ShadowCamera(float aspectRatio) : mAspectRatio(aspectRatio) {
	}
	//GETTERS
	glm::vec3 getForward();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
	//SETTERS
	inline void setForward(const glm::vec3 forward) { mForward = forward; }
	inline void setDistance(const float dis) { distance = dis; }
	inline void setNearPlane(const float nearPlane) { mNearPlane = nearPlane; }
	inline void setFarPlane(const float farPlane) { mFarPlane = farPlane; }
	inline void setOrthoSize(const float orthoSize) { mOrthoSize = orthoSize; }
	inline void setAspectRatio(const float aspectRatio) { mAspectRatio = aspectRatio; }
private:
	glm::vec3 mForward;
	float mNearPlane = 0.001f;
	float mFarPlane = 1000.0f;
	float mOrthoSize = 7.5f;
	float mAspectRatio = 1.7777f;
	float distance = 5;
	glm::vec3 genPos() { return -mForward * distance; }
};