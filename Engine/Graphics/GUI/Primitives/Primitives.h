#pragma once
#include "../Engine/Graphics/GraphicsManager.hpp"

class Primitives : public GraphicsComponent
{
public:
	Primitives(GraphicsManager& pGraphMan, GLfloat obj[6]);

protected:
	status load();
	void update() {}
	void draw();

private:
	GraphicsManager& mGraphicsManager;
	GLfloat primObject[6];

	GLuint vbo;
	GLuint vao;
	GLuint mShaderProgram;
	GLint posAttrib; GLint colAttrib; GLint sidesAttrib;
};