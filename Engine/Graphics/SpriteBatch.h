#ifndef GRAPHICSSPRITEBATCH_H
#define GRAPHICSSPRITEBATCH_H

#include "GraphicsManager.hpp"
#include "Sprite.hpp"
#include "../TimeManager.hpp"
#include "../Tools/Types.hpp"
#include "../../sources/glm/glm.hpp"
#include <vector>

class SpriteBatch : public GraphicsComponent {
public:
	SpriteBatch(TimeManager& pTimeManager, GraphicsManager& pGraphicsManager);
	~SpriteBatch();
	Sprite* registerSprite(const std::string pTextureResource, int32_t pHeight, int32_t pWidth, float scale, glm::vec3&);

	void initialize();
	void setActive(bool val)
	{
		active = val;
	}

protected:
	status load();
	void update() {}
	void draw();

private:
	TimeManager& mTimeManager;
	GraphicsManager& mGraphicsManager;
	std::vector<Sprite*> mSprites;
	std::vector<Sprite::Vertex> mVertices;
	std::vector<GLushort> mIndexes;

	typedef std::vector<Sprite*>::iterator spriteVectorIterator;

	GLuint mShaderProgram;
	GLuint aPosition; GLuint aTexture;
	GLuint uProjection; GLuint uTexture;
	GLuint uTexSize;

	std::vector<float> mTexSize;

	bool active;
};

#endif