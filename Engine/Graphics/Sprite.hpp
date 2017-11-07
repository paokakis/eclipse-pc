#ifndef _PACKT_GRAPHICSSPRITE_HPP_
#define _PACKT_GRAPHICSSPRITE_HPP_
#include "GraphicsManager.hpp"
#include "../Resource.hpp"
#include "../Tools/Types.hpp"
#include "../../sources/glm/glm.hpp"

class SpriteBatch;

class Sprite {
	friend class SpriteBatch;
public:
	Sprite(GraphicsManager& pGraphicsManager, const std::string pTextureResource, int32_t pHeight, int32_t pWidth, glm::vec3&);
	void setAnimation(int32_t pStartFrame, int32_t pFrameCount, float pSpeed, bool pLoop);
	bool animationEnded() { return mAnimFrame > (mAnimFrameCount - 1); }
	glm::vec3& location;

	void initialize();

	int32_t getTextureWidth() { return mSpriteWidth; }
	int32_t getTextureHeight() { return mSpriteHeight; }
protected:
	struct Vertex {
		GLfloat x, y, z, u, v;
	};
	status load(GraphicsManager& pGraphicsManager);
	void draw(Vertex pVertex[4], float pTimeStep);
private:
	const std::string mTextureResource;
	GLuint mTexture;
	// Frame.
	int32_t mSheetHeight, mSheetWidth;
	int32_t mSpriteHeight, mSpriteWidth;
	int32_t mFrameXCount, mFrameYCount, mFrameCount;
	// Animation.
	int32_t mAnimStartFrame, mAnimFrameCount;
	float mAnimSpeed, mAnimFrame;
	bool mAnimLoop;
	bool active;
};

#endif