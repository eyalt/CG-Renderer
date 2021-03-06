#pragma once

#include "Model.h"

enum class CameraLookAtError {
	EYE_AT_TARGET, INVALID_UP, OK
};

class Camera : public Model {

	mat4 projection;
	float left, right, bottom, top, zNear, zFar;
public:
	Camera();

	CameraLookAtError LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho();
	void Frustum();
	void setFrustrumBoundries(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
	void Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);

	void scale(const GLfloat sx, const GLfloat sy, const GLfloat sz) {}

	void reset();
	
	const mat4 getViewMatrix() const;
	const mat4 getViewNormalMatrix() const;
	const mat4 getProjectionMatrix() const;
};

bool isCameraType(const Model* p);