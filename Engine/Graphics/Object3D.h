#pragma once
#include "GraphicsManager.hpp"
#include "../TimeManager.hpp"
#include "../Game/common/effects/BillBoard.h"

typedef enum {
	OBJECT3D_CUBE,
	OBJECT3D_GENERIC,
	OBJECT3D_NORMALS,
	OBJECT3D_POINTS,
} Object3D_Shader_TYPE;

typedef struct
{
	std::string diffuse;
	std::string normal;
	std::string specular;
} texturesPath_t;

class Object3D : public GraphicsComponent
{
public:
	Object3D(GraphicsManager&, TimeManager& timeMan, glm::vec3& pos, glm::vec3& scale,
		const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE, 
		int id, const std::string& textureNormalPath = "", bool enBillBoard = false, texturesPath_t* texturesPath = 0);
	~Object3D()
	{
		mGraphicsManager.removeComponent(mID);
		mGraphicsManager.remove3dObject(mObjPath); // TODO check if it is causing problems
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

	void initialize();
	void setPosition(const glm::vec3& pos) { mPos = pos; }
	void setScale(const glm::vec3& scale) { mScale = scale; }
	void setLightPos(const glm::vec3& lPos);
	void setLightColor(const glm::vec4& lColor) { mLightColor = lColor; }
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
	void setSilhuetteEnabled(bool val)
	{
		mSilhuetteEnabled = val;
	}
	bool getSilhuetteEnabled()
	{
		return mSilhuetteEnabled;
	}
	status addTextureDiffuse(const std::string& texturePath);

	std::vector<Assimp3D>* getMesh() { return &object3DVec; }

	void setEnabled(bool val) { mActive = val; }
protected:
	status load();
	void update();
	void draw3D();

	void drawDepth(GLuint programID);
	void drawShadows(GLuint depthTexture, glm::mat4 lightSpaceMatrix);
	void drawSilhuette();

protected:
	glm::mat4 mProjection;
	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	glm::mat4 mModel;

	glm::vec3& mPos;
	glm::vec3 mRot;
	glm::vec3& mScale;
	glm::vec3 mLightPos;
	glm::vec4 mColor;
	glm::vec4 mLightColor;

	std::string mObjPath;
	std::vector<Assimp3D> object3DVec;

	float mRotSpeed;
	float mAngle;
	int numOfTextures = 1;

	std::vector<GLuint> mVertexBuffer; std::vector<GLuint> mUVbuffer; std::vector<GLuint> mNormalBuffer; std::vector<GLuint> mIndexBuffer;
private:
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	BillBoard* mpBillBoard;

	Object3D_Shader_TYPE mObjectType;

	struct {
		std::string diffusePath;
		std::string normalPath;
		std::string specularPath;
		TextureProperties* diffuse;
		TextureProperties* normal;
		TextureProperties* specular;
		float shininess;
	} mTextureMaps;

	std::vector<std::string> mTextureDiffuseResource;
	std::vector<TextureProperties*> mTextureDiffuseProperties;
	std::vector<std::string> mTextureNormalResource;
	std::vector<TextureProperties*> mTextureNormalProperties;

	int mID;
	bool mActive = false;

	// standard glsl handles
	GLuint mVAO, mVBO;
	GLuint mShaderProgram; 
	GLuint aTexture; GLuint uMVP; GLuint uScale;
	GLuint uTexture; GLuint uTexture2; GLuint uTexture3; GLuint uLightPos; GLuint uColor; GLuint uModel; GLuint uProjection; GLuint uPos; GLuint uLightColor; GLuint viewPos;
	
	// normals glsl handles
	GLuint aTexCoords; GLuint aPosition; GLuint aNormal;
	GLuint uView; GLuint aTangent;
	GLuint diffuseMap; GLuint normalMap;	
	std::vector<GLuint> mTangentBuffer; std::vector<GLuint> mBitangentBuffer;

	// billboard handles
	bool billBoradEnabled = false;
	
	// Sillhuette handles
	bool mSilhuetteEnabled = false;
	GLfloat mSilhuetteScale = 1.1;
	GLuint mStencilShader;

	GLuint asPosition = 0;
	GLuint asTexCoords = 0;
	GLuint usModel = 0;
	GLuint uNumTextures = 0;
	//GLuint usView = 0;
	//GLuint usProjection = 0;
	//GLuint usPos = 0;

	// Sun properties
	std::vector<std::string> mTexturePaths;
	std::vector<TextureProperties*> mSunTextures;

	// shadow vars
	GLuint mShadowProgram; 
};