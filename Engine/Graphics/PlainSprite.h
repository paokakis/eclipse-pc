#ifndef PLAIN_TEXTURE_H
#define PLAIN_TEXTURE_H
#include "../../sources/glm/glm.hpp"
#include "GraphicsManager.hpp"
#include "../TimeManager.hpp"
#include "../Resource.hpp"
#include <string>
#include <map>

// forward declaration
class PlainSprite;

typedef struct plSpriteOpt
{
	// access to mpTexture
	friend class PlainSprite;
	plSpriteOpt() : imagePath(""), pos(), mpTexture(), active(true)
	{}
	plSpriteOpt(std::string pth, glm::vec3* pPos, bool bActive) : imagePath(pth), pos(pPos), mpTexture(), active(bActive), colorBuffer(1.0)
	{}
	std::string imagePath;
	glm::vec3* pos;
	glm::vec4 colorBuffer;
	glm::vec3 scale;
	float width;
	float height;
	bool active;
private:
	TextureProperties* mpTexture;
} sPlSpriteOpt_t;

typedef struct sAnimation {
	sAnimation() : imagePath(""), pTextureProperties(NULL), p_pos(NULL), speed(0), rot(0), scale(1), size(1) {}
	std::string imagePath;
	TextureProperties* pTextureProperties;
	glm::vec3* p_pos;
	float speed;
	float rot;
	glm::vec3 scale;
	glm::vec3 size;
} sAnimation_t;

typedef struct sAnimOptions
{
	// access to prevTime
	friend class PlainSprite;

	sAnimOptions() : vecAnimArray(), movePos(0), colorBuffer(1.0)
	{
		updateTime = 0.0;
		activeFrame = 0;
		loop = false;
		paused = true;
		stopped = false;
		prevTime = 0.0;
	}

	std::vector<sAnimation_t*> vecAnimArray;
	float updateTime; // in seconds
	int activeFrame;
	bool loop;
	glm::vec3 movePos;
	glm::vec4 colorBuffer;
	bool paused;
	bool stopped;
private:
	// time needs . don't set (it's set automatically)
	float prevTime;
} sAnimOptions_t;

class PlainSprite : public GraphicsComponent
{
public:
	PlainSprite(GraphicsManager&, TimeManager&);
	~PlainSprite();

	void initialize();
	void setActive(bool val)
	{
		active = val;
	}

	int32_t registerSprite(sPlSpriteOpt_t&);
	void removeSprite(int32_t sprId);
	int32_t registerAnimation(const sAnimOptions_t&);
	void removeAnimation(int32_t animId);

	status setSpriteProperties(int32_t key, sPlSpriteOpt_t opts);
	status setAnimationProperties(int32_t key, sAnimOptions_t opts);

	struct Vertex {
		GLfloat x, y, z, u, v;
	};
protected:
	status load();
	void update();
	void draw();

private:
	void prepSingleVrtx(Vertex pVertices[4], int current);
	void prepAnimVrtx(Vertex pVertices[4], int current);

	void loadPlainSpr();

	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;

	GLuint mShaderProgram;
	GLuint aPosition; GLuint aTexture;
	GLuint uProjection; GLuint uTexture;
	GLuint uColor; //GLfloat mColorBuffer[4]; // color data
	GLuint uScale;
	float mTimeNow;

	std::map<int32_t, sPlSpriteOpt_t*> mSingleFrameOpts;
	std::map<int32_t, sAnimOptions_t*> mAnimationOptsMap;
	std::vector<Vertex> mVertices;
	std::vector<GLushort> mIndexes;
	std::vector<Vertex> mAnimationVertices;
	std::vector<GLushort> mAnimationIndexes;

	int32_t mSpriteKey;
	int32_t mAnimationKey;

	bool active;
};

#endif // !PLAIN_TEXTURE_H