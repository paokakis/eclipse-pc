#ifndef STARFIELD_HPP
#define STARFIELD_HPP

#include "../../../Engine/Graphics/GraphicsManager.hpp"
#include "../../../Engine/TimeManager.hpp"
#include "../../../Engine/Tools/Types.hpp"

class StarField : public GraphicsComponent 
{
public:
	StarField(TimeManager& pTimeManager,
		GraphicsManager& pGraphicsManager, int32_t pStarCount,
		const char* pTextureResource);
	status load();
	void update();
	void draw();
private:
	struct Vertex {
		GLfloat x, y, z;
	};
	TimeManager& mTimeManager;
	GraphicsManager& mGraphicsManager;
	int32_t mStarCount;
	const char* mTextureResource;
	GLuint mVertexBuffer; GLuint mTexture; GLuint mShaderProgram;
	GLuint aPosition; GLuint uProjection;
	GLuint uTime; GLuint uHeight; GLuint uTexture;
};

#endif