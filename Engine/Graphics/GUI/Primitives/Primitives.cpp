#include "Primitives.h"
#include "../Engine/tools/Log.hpp"

GLfloat points[] = {
	//Coordinates        Color        Sides
	-0.45f,  0.45f, 1.0f, 0.0f, 0.0f,  4.0f,
	 0.45f,  0.45f, 0.0f, 1.0f, 0.0f,  8.0f,
	 0.45f, -0.45f, 0.0f, 0.0f, 1.0f, 16.0f,
	-0.45f, -0.45f, 1.0f, 1.0f, 0.0f, 32.0f
};

// Vertex shader
static const GLchar* VERTEX_SHADER = R"glsl(
    #version 150 core

    in vec2 pos;
    in vec3 color;
    in float sides;

    out vec3 vColor;
    out float vSides;

    void main()
    {
        gl_Position = vec4(pos, 0.0, 1.0);
        vColor = color;
        vSides = sides;
    }
)glsl";

// Geometry shader
static const GLchar* GEOMETRY_SHADER = R"glsl(
    #version 150 core

    layout(points) in;
    layout(line_strip, max_vertices = 64) out;

    in vec3 vColor[];
    in float vSides[];
    out vec3 fColor;

    const float PI = 3.1415926;

    void main()
    {
        fColor = vColor[0];

        // Safe, GLfloats can represent small integers exactly
        for (int i = 0; i <= vSides[0]; i++) {
            // Angle between each side in radians
            float ang = PI * 2.0 / vSides[0] * i;

            // Offset from center of point (0.3 to accomodate for aspect ratio)
            vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
            gl_Position = gl_in[0].gl_Position + offset;

            EmitVertex();
        }

        EndPrimitive();
    }
)glsl";

// Fragment shader
static const GLchar* FRAGMENT_SHADER = R"glsl(
    #version 150 core
    in vec3 fColor;
    out vec4 outColor;

    void main()
    {
        outColor = vec4(fColor, 1.0);
    }
)glsl";

Primitives::Primitives(GraphicsManager& pGraphMan, GLfloat obj[6]) : mGraphicsManager(pGraphMan)
{
	Log::info("Primitives constructor");
	for (int i = 0; i < 6; ++i)
	{
		primObject[i] = obj[i];
	}
	mGraphicsManager.registerComponent(this, 100);
}

status Primitives::load()
{
	Log::info("Primitives load");

	mShaderProgram = mGraphicsManager.loadAllShaders(VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER);
	if (mShaderProgram == 0) goto ERROR;
	
	vbo = mGraphicsManager.loadVertexBuffer(primObject, sizeof(primObject));
	if (vbo == 0) goto ERROR;

	vao = mGraphicsManager.loadVertexArray();
	if (vao == 0) goto ERROR;

	// Specify the layout of the vertex data
	posAttrib = glGetAttribLocation(mShaderProgram, "pos");
	colAttrib = glGetAttribLocation(mShaderProgram, "color");
	sidesAttrib = glGetAttribLocation(mShaderProgram, "sides");

	mGraphicsManager.glErrorCheck();

	return STATUS_OK;
ERROR:
	Log::error("Error loading Primitives");
	return STATUS_KO;
}

void Primitives::draw()
{
	glUseProgram(mShaderProgram);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// used for updating the data array and drawing.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(sidesAttrib);
	glVertexAttribPointer(sidesAttrib, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	glDrawArrays(GL_POINTS, 0, 4);

	// bring everything back to normal
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisableVertexAttribArray(posAttrib);
	glDisableVertexAttribArray(colAttrib);
	glDisableVertexAttribArray(sidesAttrib);
	glUseProgram(0);

	//points[0] += 0.01;
}