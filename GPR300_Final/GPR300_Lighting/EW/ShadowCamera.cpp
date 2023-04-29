#include "ShadowCamera.h"

glm::vec3 ShadowCamera::getForward() {
	return mForward;
}

glm::mat4 ShadowCamera::getProjectionMatrix() {
	float width = mOrthoSize * mAspectRatio;
	float right = width * 0.5f;
	float left = -right;
	float top = mOrthoSize * 0.5f;
	float bottom = -top;
	return glm::ortho(left, right, bottom, top, mNearPlane, mFarPlane);
}

glm::mat4 ShadowCamera::getViewMatrix() {
	return glm::lookAt(genPos(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//set forward as position
}