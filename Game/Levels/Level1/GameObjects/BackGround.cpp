#include "BackGround.h"
#include "../../../Engine/Tools/Log.hpp"
#include "../../../Engine/Tools/Helper.h"

#define RAND(pMax) (float(pMax) * float(rand()) / float(RAND_MAX))

BackGround::BackGround(GraphicsManager& graphMan, TimeManager& timeMan, PlainSprite& pPlainSpr) :
	mSprite(pPlainSpr),
	mGraphicsManager(graphMan), mTimeManager(timeMan),
	planetPos(1), nebula1Pos(1), nebula2Pos(1), nebula3Pos(1), earthPos(1)
{
	Log::info("Background constructor");

	//std::string PATH1 = ("F:/vs_workspace/game_development_workspace/game_1/twitch/Assets/droidblaster/backGround/Sun.png");
	//std::string PATH2 = ("F:/vs_workspace/game_development_workspace/game_1/twitch/Assets/droidblaster/backGround/Nebula1.png");
	//std::string PATH3 = ("F:/vs_workspace/game_development_workspace/game_1/twitch/Assets/droidblaster/backGround/Nebula2.png");
	//std::string PATH4 = ("F:/vs_workspace/game_development_workspace/game_1/twitch/Assets/droidblaster/backGround/Nebula3.png");

	planetPos = glm::vec3(600 / 2, 1524, 200);
	nebula1Pos = glm::vec3(300 / 2, 800, 150);
	nebula2Pos = glm::vec3(800 / 2, 600, 250);
	nebula3Pos = glm::vec3(200 / 2, 300, 50);
	earthPos = glm::vec3(400, 800, 90);

	//opts.imagePath = PATH4;
	//opts.pos = &nebula3Pos;
	//mSprite.registerSprite(opts);

	/*for (int i = 119; i >= 1; --i)
	{
		sAnimation_t* anim = new sAnimation_t;

		std::string pth = std::string("F:/vs_workspace/game_development_workspace/game_1/twitch/Assets/droidblaster/backGround/planet/").append(itostr(i)).append(".png");

		anim->imagePath = pth;
		anim->p_pos = &earthPos;
		anim->rot = 0;
		anim->scale = glm::vec3(1);
		anim->speed = 0;

		animOpts.vecAnimArray.push_back(anim);
	}

	animOpts.activeFrame = 0;
	animOpts.loop = true;
	animOpts.paused = false;
	animOpts.stopped = false;
	animOpts.updateTime = 0.30f;
	animOpts.movePos = glm::vec3(0);

	earthAmimId = mSprite.registerAnimation(animOpts);*/

	//opts.imagePath = PATH2;
	//opts.pos = &nebula1Pos;
	//mSprite.registerSprite(opts);

	//opts.imagePath = PATH1;
	//opts.pos = &planetPos;
	//mSprite.registerSprite(opts);

	//opts.imagePath = PATH3;
	//opts.pos = &nebula2Pos;
	//mSprite.registerSprite(opts);	
}

BackGround::~BackGround()
{
	Log::info("Background destructor");
}

void BackGround::initialize()
{
	Log::info("Background initialize");
}

void BackGround::update()
{
	planetPos.y -= (mGraphicsManager.getDepth() / planetPos.z) * mTimeManager.elapsed() * 10;
	nebula1Pos.y -= (mGraphicsManager.getDepth() / nebula1Pos.z) * mTimeManager.elapsed() * 10;
	nebula2Pos.y -= (mGraphicsManager.getDepth() / nebula2Pos.z) * mTimeManager.elapsed() * 10;
	nebula3Pos.y -= (mGraphicsManager.getDepth() / nebula3Pos.z) * mTimeManager.elapsed() * 10;

	if (planetPos.y < -4000.f)
	{
		planetPos.x = RAND(mGraphicsManager.getRenderWidth() + 1000.f);
		planetPos.y = mGraphicsManager.getRenderHeight() + 3000.f;
	}
	if (nebula1Pos.y < -2000.f)
	{ 
		nebula1Pos.x = RAND(mGraphicsManager.getRenderWidth() + 1500.f);
		nebula1Pos.y = mGraphicsManager.getRenderHeight() + 4000.f;
	}
	if (nebula2Pos.y < -1500.f)
	{
		nebula2Pos.x = RAND(mGraphicsManager.getRenderWidth() + 500.f);
		nebula2Pos.y = mGraphicsManager.getRenderHeight() + 1500.f;
	}
	if (nebula3Pos.y < -1500.f)
	{
		nebula3Pos.x = RAND(mGraphicsManager.getRenderWidth() + 500.f);
		nebula3Pos.y = mGraphicsManager.getRenderHeight() + 1500.f;
	}
}