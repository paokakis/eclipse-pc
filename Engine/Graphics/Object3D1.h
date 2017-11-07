#pragma once
#include "GraphicsManager.hpp"
#include "../TimeManager.hpp"
#include "../Game/common/effects/BillBoard.h"

typedef enum {
	OBJECT3D_CUBE,
	OBJECT3D_GENERIC,
	OBJECT3D_NORMALS,
	OBJECT3D_POINTS
} Object3D_Shader_TYPE;

class Object3D1
{
	friend class Game3DObject;
public:
	Object3D1(GraphicsManager&, TimeManager& timeMan, glm::vec3& pos, glm::vec3& scale,
		const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE,
		const std::string& textureNormalPath = "", bool enBillBoard = false);
	~Object3D1()
	{
		mGraphicsManager.removeComponent(mID);
		mGraphicsManager.remove3dObject(mPath); // TODO check if it is causing problems
		std::vector<std::string>::iterator iter;
		for (iter = mTextureDiffuseResource.begin(); iter != mTextureDiffuseResource.end(); ++iter)
		{
			mGraphicsManager.removeTexture(*iter); // TODO check if it is causing problems
		}

		for (iter = mTextureNormalResource.begin(); iter != mTextureNormalResource.end(); ++iter)
		{
			mGraphicsManager.removeTexture(*iter); // TODO check if it is causing problems
		}
	}

	void setPosition(const glm::vec3& pos) { mPos = pos; }
	void setScale(const glm::vec3& scale) { mScale = scale; }
	void setLightPos(const glm::vec3& lPos);
	void setColor(const glm::vec4& color);
	void setRotation(const glm::vec3& rot, float speed);
	void setRotation(float angle, const glm::vec3& axis);
	void setRotation(const glm::vec3& rot) { mRot = rot; }
	void setRotation(const glm::vec4& rot)
	{
		mRot.x = rot.x;
		mRot.y = rot.y;
		mRot.z = rot.z;
		mAngle = rot.w;
	}
	void setTexture(const std::vector<TextureProperties*>& textProperties);
	void setTexture(TextureProperties* textProperties);
	void setBillBoardLevel(float lvl)
	{
		if (billBoradEnabled)
		{
			mpBillBoard->setLevel(lvl);
		}
	}

	std::vector<Assimp3D>* getMesh() { return &object3DVec; }
protected:
	status load();
	void update();
	void draw();
private:
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	BillBoard* mpBillBoard;

	Object3D_Shader_TYPE mObjectType;

	std::vector<std::string> mTextureDiffuseResource;
	std::vector<TextureProperties*> mTextureDiffuseProperties;
	std::vector<std::string> mTextureNormalResource;
	std::vector<TextureProperties*> mTextureNormalProperties;
	std::string mPath;

	glm::vec3& mPos;
	glm::vec3 mRot;
	glm::vec3& mScale;
	glm::vec3 mLightPos;
	glm::vec4 mColor;
	glm::vec4 mLightColor;

	std::vector<Assimp3D> object3DVec;

	glm::mat4 mProjection;
	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	glm::mat4 mModel;

	float mRotSpeed;
	float mAngle;
	int mID;

	// standard glsl handles
	GLuint mShaderProgram;
	GLuint aTexture; GLuint uMVP; GLuint uScale;
	GLuint uTexture; GLuint uLightPos; GLuint uColor; GLuint uModel; GLuint uProjection; GLuint uPos; GLuint uLightColor; GLuint viewPos;
	std::vector<GLuint> mVertexBuffer; std::vector<GLuint> mUVbuffer; std::vector<GLuint> mNormalBuffer; std::vector<GLuint> mIndexBuffer;
	// normals glsl handles
	GLuint aTexCoords; GLuint aPosition; GLuint aNormal;
	GLuint uView; GLuint aTangent;
	GLuint diffuseMap; GLuint normalMap;
	std::vector<GLuint> mTangentBuffer; std::vector<GLuint> mBitangentBuffer;

	// billboard handles
	bool billBoradEnabled = false;
};