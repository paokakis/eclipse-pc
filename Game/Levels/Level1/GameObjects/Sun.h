#ifndef SUN_H
#define SUN_H
#include "../../Engine/Graphics/Object3D.h"
#include "../Engine/Graphics/Configuration/Folders.h"

class PlanetSun : public Object3D
{
public:
	PlanetSun(GraphicsManager&, TimeManager& timeMan, glm::vec3& pos, glm::vec3& scale,
		const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE,
		int id, const std::string& textureNormalPath = "", bool enBillBoard = false, texturesPath_t* texturesPath = 0);
	~PlanetSun() {}

protected:
	status load();
	void update();
	void draw();

private:
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;

	std::vector<std::string> mTexturePaths;
	std::vector<TextureProperties*> mTextures;

	int mID = 1010;

	// standard glsl handles
	GLuint mShaderProgram;
	GLuint aTexture; GLuint uMVP; GLuint uScale;
	GLuint uTexture; GLuint uTexture2; GLuint uTexture3; GLuint uLightPos; GLuint uColor; GLuint uModel; GLuint uProjection; GLuint uPos; GLuint uLightColor; GLuint viewPos;
	GLuint aTexCoords; GLuint aPosition; GLuint aNormal; GLuint uNumTextures;
};

#endif
