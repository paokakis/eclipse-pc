#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H
#include "..\..\GraphicsManager.hpp"
#include "..\..\Text2D.h"
#include "../Engine/Graphics/PlainSprite.h"

class LoadingScreen
{
public:
	LoadingScreen(GraphicsManager& graphMan, TimeManager& timeMan, PlainSprite& plainSprite);
	~LoadingScreen();

	void initialize();

	status screenLoad();
	void screenUpdate();
	void screenDraw();
	bool screenDrawFinished();

	// 0 - 100
	void setProgress(float progress)
	{
		mProgress = progress;
	}

private:
	struct Vertex {
		GLfloat x, y, z;
	};

	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	PlainSprite& mPlainSprite;

	Text2D mText2d;

	std::vector<GLushort> mIndexes;
	std::vector<Vertex> mVertices;

	GLuint mShaderProgram, aPosition, uProjection, uColor;

	glm::vec4 mColor;
	glm::vec4 mTextColor;
	glm::vec3 mPos;
	glm::vec3 mTextPos;
	std::string loadingString;
	int32_t mSpriteId;

	std::vector<glm::vec3> mBarPositionsVec;
	std::vector<sPlSpriteOpt_t*> pSpriteOptsVec;
	std::vector<int32_t> spriteIdVec;

	float mTimeout;
	float mProgress;
	float mLoadingSize = 200.f;
	bool mNextScreen;

private:
	void prepVrtx(Vertex pVertices[4]);
};


#endif