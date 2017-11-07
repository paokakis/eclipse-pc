#include "Explosion.h"
#include "../../../Engine/Tools/Log.hpp"
#include "../../../Engine/Tools/Helper.h"
#include "../Engine/Graphics/Configuration/Folders.h"

static const char* VERTEX_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"attribute vec4 aPosition;\n"
"uniform mat4 uProjection;\n"
"uniform float u_time;\n"
"uniform float uSize;\n"
"uniform vec3 uCenterPos;\n"
"uniform float u_lifetime;\n"
"varying float v_lifetime;\n"
"void main()\n"
"{\n"
" if (u_time <= u_lifetime) { gl_Position = uProjection * (aPosition + vec4(uCenterPos, 0.0)); }\n"
" else { gl_Position = vec4(-2000.0, -2000.0, 0.0, 0.0); }\n"
" v_lifetime = 1.0 - (u_time / u_lifetime);\n"
" v_lifetime = clamp(v_lifetime, 0.0, 1.0);\n"
" gl_PointSize = (v_lifetime * v_lifetime) * uSize;\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"uniform vec4 u_color;\n"
"uniform sampler2D s_texture;\n"
"varying float v_lifetime;\n"
"void main()\n"
"{\n"
" vec4 texColor;\n"
" texColor = texture2D(s_texture, gl_PointCoord);\n"
" if(texColor.a < 0.5){ discard; }\n"
" gl_FragColor = u_color * texColor;\n"
" gl_FragColor.a *= v_lifetime;\n"
"}";

static const int DIV_FACTOR = 5;

Explosion::Explosion(GraphicsManager& pGraphMan, TimeManager& pTimeMan, float size) :
	mGraphicsManager(pGraphMan), mTimeManager(pTimeMan), mTextureResource(),
	u_time(0), u_centerPosition(0), u_lifetime(0), u_color(0), s_texture(0), uProjection(0),
	mTime(0), mLifeTime(0.0), mShaderProgram(0), mStart(false), mCenterPos(200.0),
	uSize(0), mSize(size), mCurrentTex(0), mUpdateTimeTex(0.5), mTimeTex(0.0), mNumParticles(size / DIV_FACTOR)
{
	Log::info("Explosion effect constructor");
	mTextureResource.push_back(EXPLOSION_PATH8);
	mTextureResource.push_back(EXPLOSION_CLOUD);
#ifdef HQ
	mTextureResource.push_back(EXPLOSION_PATH2);
	mTextureResource.push_back(EXPLOSION_CLOUD);
	mTextureResource.push_back(EXPLOSION_CLOUD);
	mTextureResource.push_back(EXPLOSION_PATH2);
	mTextureResource.push_back(EXPLOSION_PATH3);
	mTextureResource.push_back(EXPLOSION_PATH4);
	mTextureResource.push_back(EXPLOSION_PATH5);
	mTextureResource.push_back(EXPLOSION_PATH6);
	mTextureResource.push_back(EXPLOSION_PATH7);
	mTextureResource.push_back(EXPLOSION_PATH0);
#endif

	mGraphicsManager.registerComponent(this, 5);
}

Explosion::~Explosion()
{
	Log::info("Explosion effect destructor");
	mTextureProperties.clear();
}

status Explosion::load()
{
	Log::info("Loading explosion.");

	TextureProperties* textureProperties;
	Vertex* vertexBuffer = new Vertex[mNumParticles];
	for (int32_t i = 0; i < mNumParticles; ++i)
	{
		if (i < mNumParticles / 4)
		{
			vertexBuffer[i].x = RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].y = RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].z = RANDOM(mSize / DIV_FACTOR);
		}
		else if ((i >= mNumParticles / 4) && (i < mNumParticles / 2))
		{
			vertexBuffer[i].x = RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].y = -RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].z = RANDOM(mSize / DIV_FACTOR);
		}
		else if ((i >= mNumParticles / 2) && (i < (mNumParticles - (mNumParticles / 4))))
		{
			vertexBuffer[i].x = -RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].y = RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].z = RANDOM(mSize / DIV_FACTOR);
		}
		else
		{
			vertexBuffer[i].x = -RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].y = -RANDOM(mSize / DIV_FACTOR);
			vertexBuffer[i].z = RANDOM(mSize / DIV_FACTOR);
		}
	}
	// Loads the vertex buffer into OpenGL.
	mVertexBuffer = mGraphicsManager.loadVertexBuffer((uint8_t*)vertexBuffer, mNumParticles * sizeof(Vertex));
	delete[] vertexBuffer;
	if (mVertexBuffer == 0)
		goto ERROR;
	for (int i = 0; i < mTextureResource.size(); ++i)
	{
		// Loads the texture.
		textureProperties = mGraphicsManager.loadTextureDiffuse(mTextureResource[i]);
		if (textureProperties == NULL)
			goto ERROR;
		mTextureProperties.push_back(textureProperties);
	}

	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	u_time = glGetUniformLocation(mShaderProgram, "u_time");
	uSize = glGetUniformLocation(mShaderProgram, "uSize");
	u_centerPosition = glGetUniformLocation(mShaderProgram, "uCenterPos");
	u_lifetime = glGetUniformLocation(mShaderProgram, "u_lifetime");
	u_color = glGetUniformLocation(mShaderProgram, "u_color");
	s_texture = glGetUniformLocation(mShaderProgram, "s_texture");

	mGraphicsManager.glErrorCheck();

	Log::info("Explosion loaded successfuly.");

	return STATUS_OK;
ERROR:
	Log::error("Error loading explosion");

	return STATUS_KO;
}

void Explosion::update()
{
	mTime += mTimeManager.elapsed();
	if (mStart)
	{
		if (mTime >= mUpdateTimeTex)
		{
			mCurrentTex++;
			if (mCurrentTex >= mTextureResource.size() - 1) mCurrentTex = 0;
		}
		if (mTime >= mLifeTime)
		{
			mStart = false;
			mCurrentTex = 0;
		}
	}
}

void Explosion::draw()
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
	if (glGetError() != GL_NO_ERROR)
	{
		Log::error("Explosion error binding buffer");
	}
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition,	// Attribute Index 
		3,								// number of elements per vertex, here (x,y,z)
		GL_FLOAT,						// the type of each element
		GL_FALSE,						// take our values as-is
		3 * sizeof(GLfloat),			// no space between values
		(GLvoid*)0);					// use the vertex buffer object

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
	glUniform1f(u_time, mTime);
	glUniform1f(uSize, (mSize * (350.f / (glm::distance(mGraphicsManager.getCamPosition(), mCenterPos)))));
	glUniform3fv(u_centerPosition, 1, &mCenterPos.x);
	glUniform1f(u_lifetime, mLifeTime);
	glUniform4fv(u_color, 1, &mColor.r);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureProperties[mCurrentTex]->texture);
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

void Explosion::start(glm::vec3 centrePos, float lifeTime)
{
	mCenterPos.x = centrePos.x;
	mCenterPos.y = centrePos.y;
	mCenterPos.z = centrePos.z;
	mLifeTime = lifeTime;
	mUpdateTimeTex = mLifeTime / mTextureResource.size();

	// Random color
	mColor.r = (float)RANDOM(0.5f) + 5.f;
	mColor.g = (float)RANDOM(0.5f) + 5.f;
	mColor.b = (float)RANDOM(0.5f) + 1.f;
	mColor.a = 1.f;

	mStart = true;
	mTime = 0.0;
}

bool Explosion::finishedAnimating()
{
	return !mStart;
}