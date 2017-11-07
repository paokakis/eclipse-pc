#include "IntroScreen.h"
#include "..\..\Engine\tools\Log.hpp"

static const char* VERTEX_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"attribute vec3 aPosition;\n"
"uniform mat4 uProjection;\n"
"void main() {\n"
" gl_Position = uProjection * vec4(aPosition, 1.0 );\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"uniform vec4 uColor;\n"
"void main() {\n"
" gl_FragColor = uColor;\n"
"}\n";

static const float TOTAL_TIME = 6.f;

IntroScreen::IntroScreen(GraphicsManager& graphMan, TimeManager& timeMan) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mText2d(graphMan),
	mTime(0), mFadeInTime(TOTAL_TIME / 3), mStayTime(TOTAL_TIME / 3), mFadeOutTime(TOTAL_TIME / 3)
{
	Log::info("IntroScreen constructor");
}

IntroScreen::~IntroScreen()
{
	Log::info("IntroScreen destructor");

	mIndexes.clear();
	mVertices.clear();
}

status IntroScreen::screenLoad()
{
	Log::info("IntroScreen load");

	mColor.r = 0.0f;
	mColor.g = 0.0f;
	mColor.b = 0.0f;
	mColor.a = 1.0f;

	mTextColor.r = 0.0f;
	mTextColor.g = 0.0f;
	mTextColor.b = 0.0f;
	mTextColor.a = 1.0f;

	mTextPos.x = mGraphicsManager.getRenderWidth() / 2;
	mTextPos.y = mGraphicsManager.getRenderHeight() / 2;
	mTextPos.z = mGraphicsManager.getDepth() / 2;

	mIndexes.push_back(0); mIndexes.push_back(1);
	mIndexes.push_back(2); mIndexes.push_back(2);
	mIndexes.push_back(1); mIndexes.push_back(3);

	for (int j = 0; j < 4; ++j)
	{
		mVertices.push_back(Vertex());
	}

	prepVrtx(&mVertices[0]);

	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uColor = glGetUniformLocation(mShaderProgram, "uColor");

	return STATUS_OK;

ERROR:
	Log::error("IntroScreen load failed");

	mIndexes.clear();
	mVertices.clear();

	return STATUS_KO;
}

void IntroScreen::screenDraw()
{
	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &mGraphicsManager.getOrthoMVP()[0][0]);
	
	glEnable(GL_BLEND);
	// pass color data
	glUniform4fv(uColor, 1, &mColor.r);
	// pass xyz vertex data
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, // Attribute Index
		3,						// Size in bytes (x y and z)
		GL_FLOAT,				// Data type
		GL_FALSE,				// Normalized
		sizeof(Vertex),			// Stride
		&(mVertices[0].x));		// Location

	glDrawElements(GL_TRIANGLES,
		// Number of indexes
		6,
		// Indexes data type
		GL_UNSIGNED_SHORT,
		// First index
		&mIndexes[0]);

	glUseProgram(0);
	glDisableVertexAttribArray(aPosition);
	glDisable(GL_BLEND);

	mText2d.RenderText(std::string(GAME_TITLE), mTextPos, 1, mTextColor);
}

void IntroScreen::screenUpdate()
{
	mTime += mTimeManager.elapsed();
	if (mTime > 0.0)
	{
		if (mTime < mFadeInTime)
		{
			mTextColor.r += mTime / (mFadeInTime + mStayTime + mFadeOutTime + 100.f);
		}
		else if (mTime >= mFadeInTime && mTime < mFadeInTime + mStayTime)
		{ 
			// Do nothing 
		}
		else
		{
			mTextColor.r -= mTime / (mFadeInTime + mStayTime + mFadeOutTime + 100.f);
		}		
	}
	else
	{
		// do nothing
	}
}

bool IntroScreen::screenDrawFinished()
{
	return (mTime > TOTAL_TIME ? true : false);
}

void IntroScreen::prepVrtx(Vertex pVertices[4])
{
	// Draws selected frame.
	GLfloat posX1 = 0.0;
	GLfloat posY1 = 0.0;
	GLfloat posX2 = mGraphicsManager.getRenderWidth();
	GLfloat posY2 = mGraphicsManager.getRenderHeight();
	GLfloat posZ1 = 0.f;
	GLfloat posZ2 = 0.f;

	pVertices[0].x = posX1; pVertices[0].y = posY1; pVertices[0].z = posZ1;
	pVertices[1].x = posX1; pVertices[1].y = posY2; pVertices[1].z = posZ2;
	pVertices[2].x = posX2; pVertices[2].y = posY1; pVertices[2].z = posZ1;
	pVertices[3].x = posX2; pVertices[3].y = posY2; pVertices[3].z = posZ2;
}

void IntroScreen::initialize()
{
	mText2d.initialize(70);
}