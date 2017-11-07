#include "LoadingScreen.h"

#include "..\..\Engine\tools\Log.hpp"
#include "../Engine/Graphics/Configuration/Folders.h"
#include <thread>

static const char* VERTEX_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"attribute vec3 aPosition;\n"
"uniform mat4 uProjection;\n"
"void main() {\n"
" gl_Position = uProjection * vec4(aPosition, 1.0 );\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"uniform vec4 uColor;\n"
"void main() {\n"
" gl_FragColor = uColor;\n"
"}\n";

#define CUBE_ID			(100)

static glm::vec2 clickPos(0.1);
static glm::vec3 cubeRot(0.1);
static bool firstClick = true;
static std::string lString = "Loading...";

static const float TOTAL_TIME = 10.f;

LoadingScreen::LoadingScreen(GraphicsManager& graphMan, TimeManager& timeMan, PlainSprite& plainSprite) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mPlainSprite(plainSprite),
	mNextScreen(false),	mTimeout(0), mText2d(graphMan), loadingString(lString), mProgress(0.f)
{
	Log::info("LoadingScreen constructor");
}

LoadingScreen::~LoadingScreen()
{
	Log::info("LoadingScreen destructor");
}

void LoadingScreen::initialize()
{
	mText2d.initialize(70);

	//float boxWidth = 34;
	//float boxHeight = 35;
	//float boxFill = 32;
	//float edgeBoxWidth = 33;
	//float edgeBoxHeight = 26;

	//mBarPositionsVec.push_back(glm::vec3(mGraphicsManager.getRenderWidth() / 2 - boxWidth, mGraphicsManager.getRenderHeight() / 3 + boxHeight / 2, 10));
	//mBarPositionsVec.push_back(glm::vec3(mBarPositionsVec[0].x, mBarPositionsVec[0].y - boxHeight, 10));
	//mBarPositionsVec.push_back(glm::vec3(mBarPositionsVec[0].x + boxWidth + boxFill, mBarPositionsVec[0].y, 10));
	//mBarPositionsVec.push_back(glm::vec3(mBarPositionsVec[2].x, mBarPositionsVec[2].y - boxHeight, 10));
	//mBarPositionsVec.push_back(glm::vec3(mBarPositionsVec[0].x + boxWidth, mBarPositionsVec[2].y, 10));
	//mBarPositionsVec.push_back(glm::vec3(mBarPositionsVec[0].x + boxWidth, mBarPositionsVec[0].y - boxWidth * 2 + edgeBoxHeight, 10));
	//mBarPositionsVec.push_back(glm::vec3(mBarPositionsVec[0].x + boxWidth, mBarPositionsVec[0].y + boxWidth - edgeBoxHeight, 10));

	//sPlSpriteOpt_t* spriteOpts1 = new sPlSpriteOpt_t();
	//spriteOpts1->imagePath = GUI_BB_TOP_LEFT;
	//spriteOpts1->active = true;
	//spriteOpts1->colorBuffer = glm::vec4(1.f);
	//spriteOpts1->pos = &mBarPositionsVec[0];
	//spriteOpts1->scale = glm::vec3(1.f);

	//sPlSpriteOpt_t* spriteOpts2 = new sPlSpriteOpt_t();
	//spriteOpts2->imagePath = GUI_BB_BOT_LEFT;
	//spriteOpts2->active = true;
	//spriteOpts2->colorBuffer = glm::vec4(1.f);
	//spriteOpts2->pos = &mBarPositionsVec[1];
	//spriteOpts2->scale = glm::vec3(1.f);
	
	//sPlSpriteOpt_t* spriteOpts3 = new sPlSpriteOpt_t();
	//spriteOpts3->imagePath = GUI_BB_TOP_RIGHT;
	//spriteOpts3->active = true;
	//spriteOpts3->colorBuffer = glm::vec4(1.f);
	//spriteOpts3->pos = &mBarPositionsVec[2];
	//spriteOpts3->scale = glm::vec3(1.f);

	//sPlSpriteOpt_t* spriteOpts4 = new sPlSpriteOpt_t();
	//spriteOpts4->imagePath = GUI_BB_BOT_RIGHT;
	//spriteOpts4->active = true;
	//spriteOpts4->colorBuffer = glm::vec4(1.f);
	//spriteOpts4->pos = &mBarPositionsVec[3];
	//spriteOpts4->scale = glm::vec3(1.f);
	
	//sPlSpriteOpt_t* spriteOpts5 = new sPlSpriteOpt_t();
	//spriteOpts5->imagePath = GUI_BB_FILL;
	//spriteOpts5->active = true;
	//spriteOpts5->colorBuffer = glm::vec4(1.f);
	//spriteOpts5->pos = &mBarPositionsVec[4];
	//spriteOpts5->scale = glm::vec3(1.f);

	//sPlSpriteOpt_t* spriteOpts6 = new sPlSpriteOpt_t();
	//spriteOpts6->imagePath = GUI_BB_BOT;
	//spriteOpts6->active = true;
	//spriteOpts6->colorBuffer = glm::vec4(1.f);
	//spriteOpts6->pos = &mBarPositionsVec[5];
	//spriteOpts6->scale = glm::vec3(1.f);

	//sPlSpriteOpt_t* spriteOpts7 = new sPlSpriteOpt_t();
	//spriteOpts7->imagePath = GUI_BB_TOP;
	//spriteOpts7->active = true;
	//spriteOpts7->colorBuffer = glm::vec4(1.f);
	//spriteOpts7->pos = &mBarPositionsVec[6];
	//spriteOpts7->scale = glm::vec3(1.f);

	//pSpriteOptsVec.push_back(spriteOpts1);
	//pSpriteOptsVec.push_back(spriteOpts2);
	//pSpriteOptsVec.push_back(spriteOpts3);
	//pSpriteOptsVec.push_back(spriteOpts4);
	//pSpriteOptsVec.push_back(spriteOpts5);
	//pSpriteOptsVec.push_back(spriteOpts6);
	//pSpriteOptsVec.push_back(spriteOpts7);

	//for (int i = 0; i < pSpriteOptsVec.size(); ++i)
	//{
	//	spriteIdVec.push_back(mPlainSprite.registerSprite(*pSpriteOptsVec[i]));
	//}
}

status LoadingScreen::screenLoad()
{
	Log::info("LoadingScreen load");

	mColor.r = 0.0f;
	mColor.g = 0.0f;
	mColor.b = 1.0f;
	mColor.a = 1.0f;

	mTextColor.r = 0.7f;
	mTextColor.g = 0.7f;
	mTextColor.b = 0.7f;
	mTextColor.a = 1.0f;

	mTextPos.x = mGraphicsManager.getRenderWidth() / 2;
	mTextPos.y = mGraphicsManager.getRenderHeight() / 1.5;
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
	Log::error("LoadingScreen load failed");

	return STATUS_KO;
}

void LoadingScreen::screenDraw()
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
		&mVertices[0].x);		// Location

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

	mText2d.RenderText(loadingString, mTextPos, 1, mTextColor);
}

void LoadingScreen::screenUpdate()
{
	mVertices[2].x = (mGraphicsManager.getRenderWidth() / 2 - mLoadingSize) + (mProgress * 4);
	mVertices[3].x = (mGraphicsManager.getRenderWidth() / 2 - mLoadingSize) + (mProgress * 4);
}

bool LoadingScreen::screenDrawFinished()
{
	if (mNextScreen)
	{
		//clean up
		for (int i = 0; i < pSpriteOptsVec.size(); ++i)
		{
			mPlainSprite.removeSprite(spriteIdVec[i]);
			delete pSpriteOptsVec[i];
		}
		pSpriteOptsVec.clear();
		spriteIdVec.clear();
	}
	return mNextScreen;
}

void LoadingScreen::prepVrtx(Vertex pVertices[4])
{
	// Draws selected frame.
	GLfloat posX1 = mGraphicsManager.getRenderWidth() / 2 - mLoadingSize;
	GLfloat posY1 = mGraphicsManager.getRenderHeight() / 2 - 5;
	GLfloat posX2 = mGraphicsManager.getRenderWidth() / 2 - mLoadingSize;
	GLfloat posY2 = mGraphicsManager.getRenderHeight() / 2 + 5;
	GLfloat posZ1 = 0.f;
	GLfloat posZ2 = 0.f;

	pVertices[0].x = posX1; pVertices[0].y = posY1; pVertices[0].z = posZ1;
	pVertices[1].x = posX1; pVertices[1].y = posY2; pVertices[1].z = posZ2;
	pVertices[2].x = posX2; pVertices[2].y = posY1; pVertices[2].z = posZ1;
	pVertices[3].x = posX2; pVertices[3].y = posY2; pVertices[3].z = posZ2;
}