#include "SkyBox.h"
#include "../tools/Log.hpp"
#include "../Graphics/Configuration/Folders.h"

#define SKYBOX_VERTEX	("Assets/droidblaster/Shaders/skybox.vs")
#define SKYBOX_FRAGMENT	("Assets/droidblaster/Shaders/skybox.frag")

SkyBox::SkyBox(GraphicsManager& graphMan, eSkyboxType_t skyboxType) : mGraphicsManager(graphMan), mSkyboxType(skyboxType)
{
	Log::info("SkyBox constructor");
}

SkyBox::~SkyBox()
{
	Log::info("SkyBox destructor");

	mGraphicsManager.removeTexture(skyboxTexture->textureResource->getPath());
}

status SkyBox::load()
{
	Log::info("SkyBox load");

	std::vector<std::string> faces;

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	skyboxVAO = mGraphicsManager.loadVertexArray();
	skyboxVBO = mGraphicsManager.loadVertexBuffer((uint8_t*)&skyboxVertices[0], sizeof(skyboxVertices));
	if (!skyboxVAO || !skyboxVBO) goto ERROR;

	// Setup and compile our shaders
	skyboxShader = mGraphicsManager.loadShaderFile(SKYBOX_VERTEX, SKYBOX_FRAGMENT);
	if (!skyboxShader) goto ERROR;

	aPosition = glGetAttribLocation(skyboxShader, "position");
	uView = glGetUniformLocation(skyboxShader, "view");
	uTexture = glGetUniformLocation(skyboxShader, "skybox");

	switch (mSkyboxType)
	{
	case SKYBOX_0:
		faces.push_back(SKYBOX_RIGHT);
		faces.push_back(SKYBOX_LEFT);
		faces.push_back(SKYBOX_TOP);
		faces.push_back(SKYBOX_BOT);
		faces.push_back(SKYBOX_FRONT);
		faces.push_back(SKYBOX_BACK);
		break;
	case SKYBOX_1:
		faces.push_back(SKYBOX1_RIGHT);
		faces.push_back(SKYBOX1_LEFT);
		faces.push_back(SKYBOX1_TOP);
		faces.push_back(SKYBOX1_BOT);
		faces.push_back(SKYBOX1_FRONT);
		faces.push_back(SKYBOX1_BACK);
		break;
	}
	// Cubemap (Skybox)
	skyboxTexture = mGraphicsManager.loadCubeMap(faces);

	return STATUS_OK;

ERROR:
	Log::error("Error loading skybox");

	return STATUS_KO;
}

void SkyBox::update()
{
	view = glm::mat4(glm::mat3(mGraphicsManager.getViewMatrix()));	// Remove any translation component of the view matrix
}

void SkyBox::draw()
{
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	glUseProgram(skyboxShader);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, &mGraphicsManager.getProjection()[0][0]);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->texture);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}