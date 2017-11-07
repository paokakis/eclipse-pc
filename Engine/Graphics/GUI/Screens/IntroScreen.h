#ifndef INTRO_SCREEN_H
#define INTRO_SCREEN_H
#include "..\..\GraphicsManager.hpp"
#include "..\..\..\Engine\TimeManager.hpp"
#include <vector>
#include "..\..\Text2D.h"

class IntroScreen
{
public:
	IntroScreen(GraphicsManager& graphMan, TimeManager& timeMan);
	~IntroScreen();

	void initialize();
	
	status screenLoad();
	void screenUpdate();
	void screenDraw();
	bool screenDrawFinished();

private:
	struct Vertex {
		GLfloat x, y, z;
	};
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	Text2D mText2d;

	void prepVrtx(Vertex pVertices[4]);

	std::vector<GLushort> mIndexes;
	std::vector<Vertex> mVertices;

	GLuint mShaderProgram, aPosition, uProjection, uColor;

	glm::vec4 mColor;
	glm::vec4 mTextColor;
	glm::vec3 mPos;
	glm::vec3 mTextPos;

	float mTime;
	float mFadeInTime, mStayTime, mFadeOutTime;
};

#endif