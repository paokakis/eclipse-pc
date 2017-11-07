#ifndef GAME_CONFIGURATION_H
#define GAME_CONFIGURATION_H

typedef enum {
	EFFECT_MSSA,
	EFFECT_SHADOW_MAPPING,
	EFFECT_BLOOM,
	EFFECT_HDR,
} Effects_en;

class GameConfiguration
{
public:
	static GameConfiguration* getInstance() {
		if (pInstance)
		{
			return pInstance;
		}
		else
		{
			pInstance = new GameConfiguration();

			return pInstance;
		}
	}

	void setEffects(Effects_en effects)
	{
		switch (effects)
		{
		case EFFECT_MSSA:
			mEffects.MSAA = true;
			break;
		case EFFECT_SHADOW_MAPPING:
			mEffects.SHADOW_MAPPING = true;
			break;
		case EFFECT_BLOOM:
			mEffects.BLOOM = true;
			break;
		case EFFECT_HDR:
			mEffects.HDR = true;
			break;
		default:
			break;
		}
	}
private:
	explicit GameConfiguration() {}
	~GameConfiguration() {}

	static GameConfiguration* pInstance;

	typedef struct {
		bool MSAA;
		bool SHADOW_MAPPING;
		bool BLOOM;
		bool HDR;
	} Effects_t;

	Effects_t mEffects;
};

#endif