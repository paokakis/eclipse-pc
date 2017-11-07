#ifndef BACKGROUND_H
#define BACKGROUND_H
#include "../../../Engine/Graphics/GraphicsManager.hpp"
#include "../../../Engine/Graphics/PlainSprite.h"
#include <vector>

class BackGround {
public:
	BackGround(GraphicsManager& graphMan, TimeManager& timeMan, PlainSprite& pPlainSpr);
	~BackGround();

	void initialize();
	void update();
private:
	GraphicsManager& mGraphicsManager; TimeManager& mTimeManager;
	PlainSprite& mSprite;

	glm::vec3 planetPos;
	glm::vec3 nebula1Pos;
	glm::vec3 nebula2Pos;
	glm::vec3 nebula3Pos;
	glm::vec3 earthPos;

	sPlSpriteOpt_t opts;
	sAnimOptions_t animOpts; int32_t earthAmimId;
};

#endif