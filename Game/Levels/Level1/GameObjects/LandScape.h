#ifndef LAND_SCAPE_H
#define LAND_SCAPE_H
#include "../Engine/Graphics/GraphicsManager.hpp"
#include "../Engine/physics/BulletPhysicsManager.h"
#include "../Engine/physics/BulletMoveableBody.h"
#include "../Engine/Graphics/Object3D.h"

class LandScape : public GraphicsComponent
{
public:
	LandScape(TimeManager& timeMan, GraphicsManager& graphMan, BphysicsManager& physMan, glm::vec3& scale, std::string heightMapPath, std::string texturePath);
	~LandScape();

	void initialize();
	void update();

	status load();
	void draw();

	void ReleaseHeightmap();

	void setPosition(const glm::vec3& pos) 
	{
		mPos = pos; 
	}
	void setRotation(const glm::vec4& rot)
	{
		mRot.x = rot.x;
		mRot.y = rot.y;
		mRot.z = rot.z;
		mAngle = rot.w;
	}

private:
	GraphicsManager& mGraphicsManager;
	BphysicsManager& mPhysicsManager;
	std::string mHeightMapPath;
	status retStatus = STATUS_KO;

	glm::mat4 mProjection;
	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	glm::mat4 mModel;

	glm::vec3 mPos;
	glm::vec3 mRot;
	glm::vec3 mScale;
	glm::vec3 mLightPos;
	glm::vec4 mColor;
	glm::vec4 mLightColor;

	float mRotSpeed;
	float mAngle;
	int numOfTextures = 1;

	GLuint m_VBO, m_VAO, mShaderProgram; 
	GLuint aTexCoords; GLuint aPosition; GLuint aNormal;
	GLuint aTexture; GLuint uMVP; GLuint uScale; GLuint uNumTextures;
	GLuint uTexture; GLuint uTexture2; GLuint uTexture3; GLuint uLightPos; GLuint uColor; 
	GLuint uModel; GLuint uProjection; GLuint uPos; GLuint uLightColor; GLuint viewPos;

	std::vector<std::string> mTextureDiffuseResource;
	std::vector<TextureProperties*> mTextureDiffuseProperties;

	unsigned int iRows;
	unsigned int iCols;

	std::vector<GLuint> mVertexBuffer; std::vector<GLuint> mUVbuffer; std::vector<GLuint> mNormalBuffer; std::vector<GLuint> mIndexBuffer;
};

#endif // !LAND_SCAPE_H

