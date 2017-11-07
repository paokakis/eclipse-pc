#include "Primates.h"
#include "../../../tools/Log.hpp"

static const char* VERTEX_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"attribute vec4 aPosition;\n"
"uniform mat4 uProjection;\n"
"void main() {\n"
" gl_Position = uProjection * aPosition;\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"uniform vec3 uColor;\n"
"void main() {"
"  gl_FragColor = vec4(uColor, 1.0);"
"}";


Primates::Primates(GraphicsManager& graphMan) : mGraphicsManager(graphMan)
{
	Log::info("Primates constructor");
	memset(lineCoords, 0.f, sizeof(lineCoords) / sizeof(lineCoords[0]));

	mGraphicsManager.registerComponent(this, 101);
}

status Primates::load()
{
	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0)
		goto _ERROR_;

	lineVBO = mGraphicsManager.loadVertexBuffer(lineCoords, sizeof(lineCoords));
	if (lineVBO == 0) return STATUS_KO;

	// Specify the layout of the vertex data
	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uColor = glGetUniformLocation(mShaderProgram, "uColor");

    mGraphicsManager.glErrorCheck();

	return STATUS_OK;

_ERROR_:
	Log::error("Error loading Primates");

	return STATUS_KO;
}

void Primates::draw()
{
	if (!shapes.empty())
	{
		// draw
		glUseProgram(mShaderProgram);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glEnableVertexAttribArray(aPosition);
		for (size_t iter = 0; iter < shapes.size(); ++iter)
		{
			shape_properties_t shape = shapes.back();
			shapes.pop_back();
			
			switch (shape.shapeType)
			{
			case SHP_POINT:
				break;
			case SHP_LINE:
				lineCoords[0] = shape.begin.x;
				lineCoords[1] = shape.begin.y;
				lineCoords[2] = shape.begin.z;
				lineCoords[3] = shape.end.x;
				lineCoords[4] = shape.end.y;
				lineCoords[5] = shape.end.z;
				// used for updating the data array and drawing.
				glBufferData(GL_ARRAY_BUFFER, sizeof(lineCoords), lineCoords, GL_DYNAMIC_DRAW);

				glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
				glUniform3fv(uColor, 1, &shape.color.r);
				glVertexAttribPointer(aPosition, // Attribute Index 
					3, // Number of components
					GL_FLOAT, // Data type
					GL_FALSE, // Normalized
					3 * sizeof(GLfloat), // Stride
					(GLvoid*)0); // First vertex
				// Draw the triangle !
				glDrawArrays(GL_LINES, 0, 2);
				break;
			default:
				break;
			}
			// end draw
		}
		// bring everything back to normal
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(aPosition);
		glUseProgram(0);
	}
}

void Primates::addLine(glm::vec3& from, glm::vec3& to, glm::vec3& color)
{
	shapes.push_back(shape_properties_t(from, to, color, SHP_LINE));
}