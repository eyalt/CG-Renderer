#pragma once
#include <vector>
#include "mat.h"
#include "Face.h"
#include "Renderer.h"

using std::vector;

enum Axes {X, Y, Z, ALL};

class Model {
private:
	GLuint vao;
	GLuint colorVbo;
protected:
	Renderer* renderer;
	GLuint* textureImg;
	int textureWidth, textureHeight;
	Model() : renderer(NULL), textureWidth(512), textureHeight(512), textureImg(new GLuint[3*textureWidth*textureWidth]) {
		for (int x = 0; x < 512; x++) {
			for (int y = 0; y < 512; y++) {
				textureImg[INDEX(textureWidth, x, y, 0)] = 0;
				textureImg[INDEX(textureWidth, x, y, 1)] = 0;
				textureImg[INDEX(textureWidth, x, y, 2)] = 0;
			}
		}
	}

	vector<Face> faces;

	mat4 spinScaleMtx; // spinning and scaling are done in model spcae
	mat4 spinScaleInvMtx;
	mat4 rotateTranslateMtx; // moves the spinned and scaled object in the world
	mat4 rotateTranslateInvMtx;
	
	//mat3 _normal_transform;

	const mat4 genRotationMatrix(const GLfloat deg, const Axes axis) const;
	const mat4 genTranslationMatrix(const GLfloat tx, const GLfloat ty, const GLfloat tz) const;
	const mat4 genScaleMatrix(const GLfloat sx, const GLfloat sy, const GLfloat sz) const;

	bool isActive;

public:
	virtual ~Model() {}
	const mat4 getModelMatrix() const;
	const mat4 getModelNormalMatrix() const;
	const vector<Face>& getFaces() const;

	virtual void reset();
	virtual void setMaterial(const Material& mat);
	virtual void setRandomMaterial();

	void spin(const GLfloat deg, const Axes axis);
	virtual void scale(const GLfloat sx, const GLfloat sy, const GLfloat sz);
	void translate(const GLfloat tx, const GLfloat ty, const GLfloat tz);
	void rotate(const GLfloat deg, const Axes axis);

	void setActive(const bool isActive);
	bool getActive() const;

	void setRenderer(Renderer* renderer);
	void setTexture(string textureFilename);
	void draw() const;
};