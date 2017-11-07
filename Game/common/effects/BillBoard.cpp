#include "BillBoard.h"
#include "../../../Engine/Tools/Log.hpp"
#include "../../../Engine/Tools/Helper.h"
#include "../Engine/Graphics/Configuration/Folders.h"

static const char* VERTEX_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"in vec4 aPosition;\n"
"uniform mat4 uProjection;\n"
"uniform float uSize;\n"
"uniform vec3 uCenterPos;\n"
"void main()\n"
"{\n"
" gl_Position = uProjection * (aPosition + vec4(uCenterPos, 0.0));\n"
" gl_PointSize = uSize;\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"out vec4 color;\n"
"uniform sampler2D s_texture;\n"
"uniform float LifeLevel;\n"
"void main()\n"
"{\n"
" vec2 texCoords;\n"
" if (gl_PointCoord.y > 0.45 && gl_PointCoord.y < 0.55) {\n"
"	texCoords = gl_PointCoord;"
"   color = texture2D(s_texture, texCoords);\n"
"   if (texCoords.x < LifeLevel && texCoords.y > 0.47 && texCoords.y < 0.53 && texCoords.x > 0.04 && texCoords.x < 0.96)\n"
"	if (LifeLevel >= 0.5)\n"
"		color = vec4(0.2, 0.8, 0.2, 1.0);\n"
"	else if (LifeLevel > 0.2 && LifeLevel < 0.5)\n"
"		color = vec4(1.0, 1.0, 0.2, 1.0);\n"
"	else if (LifeLevel > 0.0 && LifeLevel <= 0.2)\n"
"		color = vec4(0.8, 0.2, 0.2, 1.0);\n"
" }\n"
" else { discard; }\n"
"}";

BillBoard::BillBoard(GraphicsManager& pGraphMan, float size, glm::vec3& pos) :
	mGraphicsManager(pGraphMan), mTextureResource(),
	u_time(0), u_centerPosition(0), u_lifetime(0), u_color(0), s_texture(0), uProjection(0),
	mShaderProgram(0), mCenterPos(pos), uSize(0), mSize(size), mNumParticles(1)
{
	Log::info("Explosion effect constructor");

	mGraphicsManager.registerComponent(this, 6);
}

BillBoard::~BillBoard()
{
	Log::info("Explosion effect destructor");
	mTextureProperties.clear();
}

status BillBoard::load()
{
	Log::info("Loading explosion.");

	TextureProperties* textureProperties;
	Vertex* vertexBuffer = new Vertex[mNumParticles];
	for (int32_t i = 0; i < mNumParticles; ++i)
	{
		vertexBuffer[i].x = 0;
		vertexBuffer[i].y = 0;
		vertexBuffer[i].z = 20;
	}
	// Loads the vertex buffer into OpenGL.
	mVertexBuffer = mGraphicsManager.loadVertexBuffer((uint8_t*)vertexBuffer, mNumParticles * sizeof(Vertex));
	delete[] vertexBuffer;
	if (mVertexBuffer == 0)
		goto ERROR;

	// Loads the texture.
	textureProperties = mGraphicsManager.loadTextureDiffuse(BILLBOARD_TEXTURE);
	if (textureProperties == NULL)
		goto ERROR;
	mTextureProperties.push_back(textureProperties);

	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uSize = glGetUniformLocation(mShaderProgram, "uSize");
	u_centerPosition = glGetUniformLocation(mShaderProgram, "uCenterPos");
	s_texture = glGetUniformLocation(mShaderProgram, "s_texture");
	LifeLevelID = glGetUniformLocation(mShaderProgram, "LifeLevel");

	Log::info("Explosion loaded successfuly.");

	mGraphicsManager.glErrorCheck();

	return STATUS_OK;
ERROR:
	Log::error("Error loading explosion");

	return STATUS_KO;
}

void BillBoard::update()
{

}

void BillBoard::draw()
{
	// Blend particles
	/*The result of this is that the alpha produced in the fragment shader is modulated
	with the fragment color. This value is then added into whatever values
	are stored in the destination of the fragment. The result is to get an
	additive blend effect for the particle system.*/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SPRITE);
	// Use the program object and pass the data
	glUseProgram(mShaderProgram);
	// Selects the vertex buffer and indicates how data is stored.
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition,	// Attribute Index 
		3,								// number of elements per vertex, here (x,y,z)
		GL_FLOAT,						// the type of each element
		GL_FALSE,						// take our values as-is
		3 * sizeof(GLfloat),			// no space between values
		(GLvoid*)0);					// use the vertex buffer object

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
	glUniform1f(uSize, (mSize * (350.f / (glm::distance(mGraphicsManager.getCamPosition(), mCenterPos)))));
	glUniform3fv(u_centerPosition, 1, &mCenterPos.x);
	
	glUniform1f(LifeLevelID, LifeLevel);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureProperties[0]->texture);
	glEnable(GL_TEXTURE_2D);
	// Set the sampler texture unit to 0
	glUniform1i(s_texture, 0);

	// Draw the particles
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, mNumParticles);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// Set everything back to default
	glUseProgram(0);
	glDisableVertexAttribArray(aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE);
}