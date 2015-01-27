#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <assert.h>
#include "PointLight.h"
#include "ParallelLight.h"
#include <algorithm>
#include <math.h>
#include "Vertex.h"

#include <time.h>
#include <sstream>

map<GLuint, map<ShaderParamName, vector<vec4> > > tmpVao;

#define SPEC_SHININESS 3


void checkError()
{
	int a = glGetError();
	if (a != GL_NO_ERROR)
	{
		std::cout << "Throwing openGL error exception. erro no. " << a << endl;
		throw std::exception("An openGL error occured: " + a);
	}
}

Renderer::Renderer() :m_width(512), m_height(512), colorMethod(ColorMethod::FLAT), silhouette(false), toon(false)
{
	InitOpenGLRendering();
	initial_width = 512;
	initial_height = 512;
	CreateBuffers(512, 512);
	drawVertexNormals = false;
	drawFaceNorms = false;
}
Renderer::Renderer(int width, int height) : initial_width(width), initial_height(height), m_width(width), m_height(height), colorMethod(ColorMethod::FLAT), silhouette(false), toon(false)
{
	//InitOpenGLRendering();
	glViewport(0, 0, m_width, m_height);

	program = InitShader("vshader.glsl", "fshader.glsl");
	checkError();
	cout << "Renderer loaded program" << endl;

	fillShaderParams();
}

Renderer::~Renderer(void)
{
}


void Renderer::fillShaderParams() {
	cout << "Filling shader parameters" << endl;
	//glUseProgram(program);
	checkError();
	cout << "Using program: " << program << endl;
	
	shaderParams[ShaderParamName::V_POSITION] = ShaderParam(glGetAttribLocation(program, "vPosition"), 4);
	checkError();
	cout << "V_POSITION id: " << shaderParams[ShaderParamName::V_POSITION].id << endl;
	
	shaderParams[ShaderParamName::V_NORMAL] = ShaderParam(glGetAttribLocation(program, "vNormal"), 4);
	checkError();
	cout << "V_NORMAL id: " << shaderParams[ShaderParamName::V_NORMAL].id << endl;
	
	shaderParams[ShaderParamName::V_FACE_NORMAL] = ShaderParam(glGetAttribLocation(program, "vFaceNormal"), 4);
	checkError();
	cout << "V_FACE_NORMAL id: " << shaderParams[ShaderParamName::V_FACE_NORMAL].id << endl;

	shaderParams[ShaderParamName::V_AMBIENT] = ShaderParam(glGetAttribLocation(program, "vAmbient"), 3);
	checkError();
	cout << "V_AMBIENT id: " << shaderParams[ShaderParamName::V_AMBIENT].id << endl;

	shaderParams[ShaderParamName::V_DIFFUSE] = ShaderParam(glGetAttribLocation(program, "vDiffuse"), 3);
	checkError();
	cout << "V_DIFFUSE id: " << shaderParams[ShaderParamName::V_DIFFUSE].id << endl;

	shaderParams[ShaderParamName::V_SPECULAR] = ShaderParam(glGetAttribLocation(program, "vSpecular"), 3);
	checkError();
	cout << "V_SPECULAR id: " << shaderParams[ShaderParamName::V_SPECULAR].id << endl;

	shaderParams[ShaderParamName::V_FACE_MID] = ShaderParam(glGetAttribLocation(program, "vFaceMid"), 4);
	checkError();
	cout << "V_FACE_MID id: " << shaderParams[ShaderParamName::V_FACE_MID].id << endl;

	shaderParams[ShaderParamName::V_TEX_COORDS] = ShaderParam(glGetAttribLocation(program, "vTexCoords"), 2);
	checkError();
	cout << "V_TEX_COORDS id: " << shaderParams[ShaderParamName::V_TEX_COORDS].id << endl;
	
	shaderParams[ShaderParamName::U_MODELVIEW_MTX] = ShaderParam(glGetUniformLocation(program, "uModelviewMtx"), 16);
	checkError();
	cout << "U_MODELVIEW_MTX id: " << shaderParams[ShaderParamName::U_MODELVIEW_MTX].id << endl;
	
	shaderParams[ShaderParamName::U_NORM_MODELVIEW_MTX] = ShaderParam(glGetUniformLocation(program, "uNormModelviewMtx"), 16);
	checkError();
	cout << "U_NORM_MODELVIEW_MTX id: " << shaderParams[ShaderParamName::U_NORM_MODELVIEW_MTX].id << endl;
	
	shaderParams[ShaderParamName::U_PROJ_MTX] = ShaderParam(glGetUniformLocation(program, "uProjMtx"), 16);
	checkError();
	cout << "U_PROJ_MTX id: " << shaderParams[ShaderParamName::U_PROJ_MTX].id << endl;

	shaderParams[ShaderParamName::U_NUM_LIGHTS] = ShaderParam(glGetUniformLocation(program, "numLights"), 1);
	checkError();
	cout << "U_NUM_LIGHTS id: " << shaderParams[ShaderParamName::U_NUM_LIGHTS].id << endl;

	shaderParams[ShaderParamName::U_COLOR_METHOD] = ShaderParam(glGetUniformLocation(program, "uColorMethod"), 1);
	checkError();
	cout << "U_COLOR_METHOD id: " << shaderParams[ShaderParamName::U_COLOR_METHOD].id << endl;

	shaderParams[ShaderParamName::U_CONST_COLOR] = ShaderParam(glGetUniformLocation(program, "uConstColor"), 1);
	checkError();
	cout << "U_CONST_COLOR id: " << shaderParams[ShaderParamName::U_CONST_COLOR].id << endl;

	shaderParams[ShaderParamName::U_TEX_TYPE] = ShaderParam(glGetUniformLocation(program, "uTexType"), 1);
	checkError();
	cout << "U_TEX_TYPE id: " << shaderParams[ShaderParamName::U_TEX_TYPE].id << endl;

	shaderParams[ShaderParamName::U_TEX_MAP] = ShaderParam(glGetUniformLocation(program, "uTexMap"), 1);
	checkError();
	cout << "U_TEX_MAP id: " << shaderParams[ShaderParamName::U_TEX_MAP].id << endl;

	shaderParams[ShaderParamName::U_TOON] = ShaderParam(glGetUniformLocation(program, "uToon"), 1);
	checkError();
	cout << "U_TOON id: " << shaderParams[ShaderParamName::U_TOON].id << endl;

	shaderParams[ShaderParamName::U_SILHOUETTE] = ShaderParam(glGetUniformLocation(program, "uSilhouette"), 1);
	checkError();
	cout << "U_SILHOUETTE id: " << shaderParams[ShaderParamName::U_SILHOUETTE].id << endl;
	
	cout << "All shader parameters were filled" << endl;
}

void Renderer::CreateBuffers(int width, int height)
{
	m_width = width;
	m_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width*m_height];
	m_zbuffer = new float[m_width*m_height];
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, 256, i, 0)] = 1;	m_outBuffer[INDEX(m_width, 256, i, 1)] = 0;	m_outBuffer[INDEX(m_width, 256, i, 2)] = 0;

	}
	//horizontal line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, i, 256, 0)] = 1;	m_outBuffer[INDEX(m_width, i, 256, 1)] = 0;	m_outBuffer[INDEX(m_width, i, 256, 2)] = 1;

	}
}

/////////////////////////////////////////////////////
// Shmulik & Eyal stuff

void Renderer::setColorMethod(const ColorMethod& cm){
	colorMethod = cm;

	glUniform1i(shaderParams.at(ShaderParamName::U_COLOR_METHOD).id, (GLint) cm);
}

void Renderer::reshape(int width, int height){
	resizingMatrix = mat4((float)initial_width / width, 0, 0, 0,
		0, (float)initial_height / height, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	m_width = width;
	m_height = height;
	glViewport(0, 0, m_width, m_height);
	//CreateBuffers(width, height);
}

void Renderer::InitializeBuffer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//for (int x = 0; x < m_width; x++) {
	//	for (int y = 0; y < m_height; y++) {
	//		m_outBuffer[INDEX(m_width, x, y, 0)] = 0;
	//		m_outBuffer[INDEX(m_width, x, y, 1)] = 0;
	//		m_outBuffer[INDEX(m_width, x, y, 2)] = 0;
	//	}
	//}
}

bool Renderer::getDrawNormals() {
	return this->drawVertexNormals;
}
void Renderer::setDrawNormals(const bool drawNormals) {
	this->drawVertexNormals = drawNormals;
}

bool Renderer::getDrawFaceNormals() {
	return this->drawFaceNorms;
}

void Renderer::setDrawFaceNormals(const bool drawFaceNorms) {
	this->drawFaceNorms = drawFaceNorms;
}

void Renderer::fillColorVBO(GLuint vbo, const vector<Face>& faces, bool test){
	vector<vec3> buffer;
	for (const auto& face : faces){
		for (const auto& v : face.getVertices()){
			const Material& mat = v.getMaterial();
			buffer.push_back(mat.getAmbient());
			buffer.push_back(mat.getDiffuse());
			buffer.push_back(mat.getSpecular());
		}
	}
	assert(buffer.size() == faces.size() * 3 * 3);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkError();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * buffer.size(), buffer.data(), GL_STATIC_DRAW);
	checkError();
}

void Renderer::addModel(const vector<Face>& faces, GLuint& vao, GLuint& colorVbo) {
	cout << "Adding model to renderer with " << faces.size() << " faces" << endl;
	map<ShaderParamName, GLuint> vbos;

	// First we arrange everything in vectors
	vector<vec4> vertices;
	vector<vec4> normals;
	vector<vec4> faceNormals;
	vector<vec4> faceMid;
	vector<vec2> textures;
	for (auto& f : faces) {
		for (auto& v : f.getVertices()) {
			vertices.push_back(v.getCoords());
			normals.push_back(v.getNorm());
			faceNormals.push_back(f.getNorm());
			faceMid.push_back(f.getMidPoint());
			textures.push_back(v.getTexCoords());
		}
	}
	assert(vertices.size() == faces.size() * 3);
	assert(normals.size() == faces.size() * 3);
	assert(faceNormals.size() == faces.size() * 3);

	// Now we will create the vbos
	GLuint buffers[NUMBER_VBOS];
	glGenBuffers(NUMBER_VBOS, buffers);
	checkError();

	// Vertices vbo
	vbos[ShaderParamName::V_POSITION] = buffers[VBOIndex::VERTICES];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	checkError();

	// Normals vbo
	vbos[ShaderParamName::V_NORMAL] = buffers[VBOIndex::NORMALS];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * normals.size(), normals.data(), GL_STATIC_DRAW);
	checkError();

	// Face's normals vbo
	vbos[ShaderParamName::V_FACE_NORMAL] = buffers[VBOIndex::FACE_NORMALS];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_FACE_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * faceNormals.size(), faceNormals.data(), GL_STATIC_DRAW);
	checkError();

	// Face's mid points vbo
	vbos[ShaderParamName::V_FACE_MID] = buffers[VBOIndex::FACE_MID];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_FACE_MID]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * faceMid.size(), faceMid.data(), GL_STATIC_DRAW);

	// Textures vbo
	vbos[ShaderParamName::V_TEX_COORDS] = buffers[VBOIndex::TEXTURES];
	glBindBuffer(GL_ARRAY_BUFFER, vbos[ShaderParamName::V_TEX_COORDS]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * textures.size(), textures.data(), GL_STATIC_DRAW);

	// Materials vbo
	//vbos[ShaderParamName::V_MATERIAL] = buffers[VBOIndex::MATERIALS];
	colorVbo = buffers[VBOIndex::MATERIALS];
	fillColorVBO(colorVbo, faces);

	// Now we create the vao
	glGenVertexArrays(1, &vao);
	checkError();
	glBindVertexArray(vao);

	cout << "Binded vao " << vao << endl;

	for (auto it = vbos.begin(); it != vbos.end(); ++it) {
		ShaderParam& param = shaderParams.at(it->first);
		GLuint vbo = it->second;

		assert(param.id != -1 && vbo != -1);

		glEnableVertexAttribArray(param.id);
		checkError();
		cout << "Enabled attribute with id: " << param.id << endl;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		checkError();
		cout << "Binded vbo with id: " << vbo << endl;
		glVertexAttribPointer(param.id, param.size, GL_FLOAT, GL_FALSE, 0, 0);
		checkError();
	}

	ShaderParamName colorParams[3] = { ShaderParamName::V_AMBIENT, ShaderParamName::V_DIFFUSE, ShaderParamName::V_SPECULAR };
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	checkError();
	cout << "Binded Color vbo with id: " << colorVbo << endl;
	for (int i = 0; i < 3; i++){
		ShaderParam param = shaderParams[colorParams[i]];
		assert(param.size == 3);
		int stride = 3 * sizeof(vec3);
		int offset = i * sizeof(vec3);
		//int offset = 0;

		glEnableVertexAttribArray(param.id);
		checkError();
		cout << "Enabled color attribute with id: " << param.id << endl;
		glVertexAttribPointer(param.id, param.size, GL_FLOAT, GL_FALSE, stride, (GLvoid *) offset);
		checkError();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::setCamera(const mat4& viewMtx, const mat4& normViewMtx, const mat4& projMtx) {
	this->viewMtx = viewMtx;
	this->normViewMtx = normViewMtx;
	this->projMtx = projMtx;
}

void Renderer::setModelTransformations(GLuint vao, const mat4& modelMtx, const mat4& normModelMtx) const {
	glUseProgram(program);
	checkError();

	glUniformMatrix4fv(shaderParams.at(ShaderParamName::U_MODELVIEW_MTX).id, 1, GL_TRUE, viewMtx * modelMtx);
	checkError();
	glUniformMatrix4fv(shaderParams.at(ShaderParamName::U_NORM_MODELVIEW_MTX).id, 1, GL_TRUE, normViewMtx * normModelMtx);
	checkError();
	glUniformMatrix4fv(shaderParams.at(ShaderParamName::U_PROJ_MTX).id, 1, GL_TRUE, projMtx * resizingMatrix);
	checkError();

#ifdef DEBUG_PRINT
	cout << "Drawing vao " << vao << endl;
	cout << "Number of vertices: " << size << endl;
	//cout << "Vertices: ";
	//for (int i = 0; i < size; i++){
	//	cout << tmpVao[vao][ShaderParamName::V_POSITION][i] << ", " << endl;
	//}
	cout << "ModelView: " << viewMtx * modelMtx << endl;
	cout << "NormModelView: " << normViewMtx * normModelMtx << endl;
	cout << "ProjMtx: " << projMtx * resizingMatrix << endl;
#endif

	glBindVertexArray(vao);
	checkError();
}

void Renderer::setTexture(GLuint tex, TextureType texType) const{
	if (texType == TextureType::NONE){
		glUniform1i(shaderParams.at(ShaderParamName::U_TEX_TYPE).id, 0);
		checkError();
		return;
	}

	glActiveTexture(GL_TEXTURE0);
	checkError();

	glUniform1i(shaderParams.at(ShaderParamName::U_TEX_MAP).id, 0);
	checkError();

	glBindTexture(GL_TEXTURE_2D, tex);
	checkError();

	switch (texType){
	case TextureType::COLOR:{ 
		glUniform1i(shaderParams.at(ShaderParamName::U_TEX_TYPE).id, 1); 
		checkError(); 
		break; 
	}
	case TextureType::NORMAL: {
		glUniform1i(shaderParams.at(ShaderParamName::U_TEX_TYPE).id, 2); 
		checkError(); 
		break; 
	}
	default: throw exception("Tried setting unkown tex type");
	}
}

void Renderer::unsetTexture() const{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(shaderParams.at(ShaderParamName::U_TEX_TYPE).id, 0);
}

void Renderer::drawArrays(int size) const {
	glDrawArrays(GL_TRIANGLES, 0, size);
	checkError();
}

void Renderer::drawModel(GLuint vao, int size, const mat4& modelMtx, const mat4& normModelMtx, GLuint tex, TextureType texType) const {
	setModelTransformations(vao, modelMtx, normModelMtx);
	setTexture(tex, texType);
	setConstColor(false);

	if (silhouette) {
		glDepthMask(GL_FALSE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(3.0);

		glUniform1i(shaderParams.at(ShaderParamName::U_SILHOUETTE).id, 1);

		glDrawArrays(GL_TRIANGLES, 0, size);

		glUniform1i(shaderParams.at(ShaderParamName::U_SILHOUETTE).id, 0);

		glLineWidth(1.0);
		glDepthMask(GL_TRUE);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	drawArrays(size);

	unsetTexture();
}

void Renderer::drawActiveModel(GLuint vao, int size, const mat4& modelMtx, const mat4& normModelMtx) const {
	setModelTransformations(vao, modelMtx, normModelMtx);
	setConstColor(true);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	drawArrays(size);
}

void Renderer::setLights(const vector<RendererLight>& lights) {
	int size;
	if (lights.size() >= 10) {
		size = 10;
	}
	else {
		size = lights.size();
	}
	glUniform1i(shaderParams.at(ShaderParamName::U_NUM_LIGHTS).id, (GLint)size);
	checkError();

	int index = 0;
	for (auto light : lights) {
		stringstream lightsString;
		lightsString << "lights[" << index << "].";

		stringstream ambient, diffuse, specular, pos;
		ambient << lightsString.str() << "ambient";
		diffuse << lightsString.str() << "diffuse";
		specular << lightsString.str() << "specular";
		pos << lightsString.str() << "pos";
		
		vec4 direction;

		if (light.isPointLight) {
			const mat4& modelView = this->viewMtx * light.modelMtx;
			direction = modelView * vec4(0, 0, 0, 1);
		}
		else {
			const mat4& normModelView = this->normViewMtx * light.modelMtx;
			direction = normModelView * vec4(0, 0, 1, 0);
			direction.w = 0;

			direction = normalize(direction);
		}

		GLuint ambientId = glGetUniformLocation(program, ambient.str().c_str());
		glUniform3f(ambientId, light.ambient.x, light.ambient.y, light.ambient.z);
		checkError();

		GLuint diffuseId = glGetUniformLocation(program, diffuse.str().c_str());
		glUniform3f(diffuseId, light.diffuse.x, light.diffuse.y, light.diffuse.z);
		checkError();

		GLuint specularId = glGetUniformLocation(program, specular.str().c_str());
		glUniform3f(specularId, light.specular.x, light.specular.y, light.specular.z);
		checkError();

		GLuint posId = glGetUniformLocation(program, pos.str().c_str());
		glUniform4f(posId, direction.x, direction.y, direction.z, direction.w);
		checkError();

		index++;
		if (index >= 10) { return; }
	}
}

void Renderer::setConstColor(const boolean constColor) const {
	glUniform1i(shaderParams.at(ShaderParamName::U_CONST_COLOR).id, constColor);
	checkError();
}

GLuint Renderer::add2DTexture(GLubyte* texels, int width, int height) {
	GLuint tex;
	glGenTextures(1, &tex);
	checkError();
	glActiveTexture(GL_TEXTURE0);
	checkError();
	glBindTexture(GL_TEXTURE_2D, tex);
	checkError();

	// We are assigning the chosen image to the 'tex' data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);
	checkError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glGenerateMipmap(GL_TEXTURE_2D);
	checkError();
	
	glBindTexture(GL_TEXTURE_2D,0);
	checkError();
	return tex;
}

void Renderer::setSilhouette(const boolean silhouette) {
	this->silhouette = silhouette;
}

void Renderer::setToon(const boolean toon) {
	this->toon = toon;
	glUniform1i(shaderParams.at(ShaderParamName::U_TOON).id, toon);
	checkError();
}

boolean Renderer::getSilhouette() {
	return this->silhouette;
}

boolean Renderer::getToon() {
	return this->toon;
}

/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[] = {
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[] = {
		0, 0,
		1, 0,
		0, 1,
		0, 1,
		1, 0,
		1, 1 };
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	GLint  vPosition = glGetAttribLocation(program, "vPosition");

	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0);

	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid *) sizeof(vtc));
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
	a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{
	glutSwapBuffers();
	glFlush();
	//glutPostRedisplay();
	//int a = glGetError();
	//glActiveTexture(GL_TEXTURE0);
	//a = glGetError();
	//glBindTexture(GL_TEXTURE_2D, gScreenTex);
	//a = glGetError();
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//a = glGetError();

	//glBindVertexArray(gScreenVtc);
	//a = glGetError();
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//a = glGetError();
	//glutSwapBuffers();
	//a = glGetError();
}
