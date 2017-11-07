#ifndef GRAPHICSMANAGER_HPP
#define GRAPHICSMANAGER_HPP

#include <gl\glew.h>
#include <gl\GL.h>
#include <GLFW\glfw3.h>
#include <map>
#include <vector>
#include "../Tools/Types.hpp"
#include "../Resource.hpp"
#include "../TimeManager.hpp"
#include "../../sources/glm/glm.hpp"
#include "../../sources/glm/gtc/matrix_transform.hpp"
#include "../Portable.h"
#include "Camera2.h"
#include "../tools/objloader.hpp"
#include "../Engine/tools/Log.hpp"
#pragma comment(lib, "opengl32.lib")

#define DEPTH			(1000000.f)

struct TextureProperties {
	Resource* textureResource;
	GLuint texture;
	float width;
	float height;
};

typedef enum {
	VBO_DYNAMIC,
	VBO_STATIC
} VBO_Type_t;

class GraphicsComponent {
	friend class GraphicsManager;
public:
	virtual ~GraphicsComponent() {}
protected:
	virtual status load() = 0;
	virtual void update() = 0;
	virtual void draw() {}

	virtual void draw3D() {}
	virtual void drawDepth(GLuint programID) {}
	virtual void drawShadows(GLuint depthTexture, glm::mat4 lightSpaceMatrix) {}
	virtual void drawSilhuette() {}
};

class GraphicsManager {
public:
	GraphicsManager(TimeManager&);
	~GraphicsManager();
	float getRenderWidth() { return static_cast<float>(mRenderWidth); }
	float getRenderHeight() { return static_cast<float>(mRenderHeight); }
	float getDepth() { return static_cast<float>(DEPTH); }

	GLfloat* getMVPMatrix() const { return mCamera->getProjectionMatrix(); }
	glm::mat4& getMVP() const { return mCamera->getMVP(); }
	glm::mat4& getProjection() const { return mCamera->getProjection(); }
	glm::mat4& getOrthoMVP() const { return mCamera->getOrthoMVP(); }
	glm::mat4& getPerspectiveProjection() const { return mCamera->getPerspectiveProjection(); }
	glm::mat4& getViewMatrix() const { return mCamera->getViewMatrix(); }
	glm::vec3& getCamPosition() const { return mCamera->getCamPosition(); }
	glm::vec3& getCamTarget() const { return mCamera->getCamTarget(); }
	glm::vec3& getCamUp() const { return mCamera->getCameraUp(); }
	glm::vec3& getCamRight() const { return mCamera->getCameraRight(); }
	glm::vec3& getCamCorshair() const { return mCamera->getCamCorshair(); }
	float getCamFOV() const { return mCamera->getFOV(); }

	void setCamMouseMove(bool val) { mCamera->setMouseMove(val); }
	void setCamPosition(glm::vec3& pos) { mCamera->setCamPosition(pos); }
	void setCamTarget(glm::vec3& targ) { mCamera->setCamTarget(targ); }
	void setCamUp(glm::vec3& _up) { mCamera->setCamUp(_up); }
	void setCamYawPitch(float yaw, float pitch) { mCamera->setCamYawPitch(yaw, pitch); }
	void setCamFront(glm::vec3& pos) { mCamera->setCamFront(pos); }
	void setCamRotateAroundPos(const glm::vec3& objPos, float radius, float height) { mCamera->rotateAroundPos(objPos, radius, height); }
	void setCamLookAt(const glm::vec3& pos) { mCamera->lookAt(pos); }

	GLFWwindow* getDisplay() { return mDisplay; }

	void registerComponent(GraphicsComponent* pComponent, GLuint slot);
	status removeComponent(GLuint slot);

	status start();
	status update();
	void stop();

	TextureProperties* const loadTextureDiffuse(const std::string, bool invert = false);
	TextureProperties* const loadTextureNormal(const std::string);
	TextureProperties* const loadCubeMap(std::vector<std::string> faces);
	GLuint loadShaderFile(const char* vertex, const char* fragment);
	status removeTexture(const std::string path);
	std::vector<Assimp3D> load3dObject(const std::string path);
	std::vector<Assimp3D> loadStandard3dObject(Obj_type type);
	status remove3dObject(const std::string path);
	GLuint loadShader(const char* pVertexShader, const char* pFragmentShader);
	GLuint loadAllShaders(const char* pVertexShader, const char* pFragmentShader, const char* pGeometryShader);
	GLuint loadVertexBuffer(const void* pVertexBuffer, int32_t pVertexBufferSize, VBO_Type_t vboType = VBO_STATIC);
	GLuint loadVertexArray();
	GLuint loadIndexBuffer(const void* pIndexBuffer, int32_t pINdexBufferSize);
	bool glErrorCheck()
	{
		GLenum error;
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			Log::error("Error : %s", glewGetErrorString(error));

			return true;
		}
		return false;
	}

	void setBloomExposure(float exp) { mBloomExposure = exp; }
	void enableBloom(bool val) { mEnableBloom = val; }
	void setGammaCorrection(float val) { mGamma = val; }

	static GraphicsManager* getInstance() { return mRef; }
private:
	TimeManager& mTimeManager;
	Camera2* mCamera = NULL;
	struct RenderVertex {
		GLfloat x, y, u, v;
	};
	status initializeRenderBuffer();

	int32_t mRenderWidth; int32_t mRenderHeight;
	float mGamma;
	GLFWwindow* mDisplay;

	// Graphics resources.
	std::map<Resource*, TextureProperties*> mTextures;
	std::map<std::pair<const char*, const char*>, GLuint> mFileShadersCache;
	std::map<std::pair<const char*, const char*>, GLuint> mShadersCache;
	std::multimap<int, GraphicsComponent*> mComponents;
	std::map<std::string, std::vector<Assimp3D>*> m3Dcomponents;
	typedef std::multimap<int, GraphicsComponent*>::iterator gComponentVectorIterator;
	typedef std::vector<GLuint>::iterator gluintVectorIterator;
	typedef std::map<Resource*, TextureProperties*>::iterator textureMapIterator;
	typedef std::map<std::string, std::vector<Assimp3D>*>::iterator m3DcomponentsIterator;

	// Rendering resources.
	GLint mScreenFrameBuffer;
	GLuint mRenderFrameBuffer; GLuint mRenderVertexBuffer; GLuint mRenderDepthBuffer; GLuint mRenderShadowBuffer;
	GLuint mRenderTexture; GLuint mDepthTexture;
	GLuint mRenderShaderProgram; 
	GLuint aPosition; GLuint aTexture;
	GLuint uProjection; GLuint uTexture;
	GLuint uGamma;

	// opengl containers 
	std::vector<GLuint> mShaders;
	std::vector<GLuint> mVertexBuffers;
	std::vector<GLuint> mVertexArrays;
	std::vector<GLuint> mIndexBuffers;

	static GraphicsManager* mRef;

	bool mFullScreen;
	int32_t MSAA; GLuint uMSAA;

	// Bloom vars
	GLuint hdrFBO; GLuint bloomRboDepth; GLuint mBlurProgram; GLuint mBloomFinalProgram;
	GLuint pingpongFBO[2];
	GLuint pingpongColorbuffers[2];
	GLuint colorBuffers[2];
	GLuint uTexture0; GLuint uTexture1;
	float mBloomExposure;
	bool mEnableBloom;
	GLuint mBloomAmmount = 15;
	
	// Shadow mapping vars
	GLuint simpleDepthShader;
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO; GLuint depthMap;
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;

	// Private funcs
private:
	void drawBloom();
	void drawShadows();
};

#endif // GRAPHICSMANAGER_HPP