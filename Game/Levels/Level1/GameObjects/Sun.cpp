#include "Sun.h"
#include "../Engine/tools/Log.hpp"
#include "../Game/common/MapCoordinates.h"

static const char* SUN_VERTEX = R"glsl(
#version 400 core
precision highp float;
// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;
uniform mat4 uProjection;
uniform mat4 uModel;
uniform vec4 uColor;
uniform vec4 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uPos;
out vec4 v_Color;
out vec3 v_Position;
out vec3 v_Normal;
out vec3 v_LightPos;
out vec4 v_LightColor;
// Output data ; will be interpolated for each fragment.
out vec2 UV;
// Values that stay constant for the whole mesh.
void main() {
	vec4 vPosition = vec4(aPosition, 1.0);
	// Transform the vertex into eye space.
	v_Position = vec3(uProjection * vec4(uPos, 1.0));
	// Pass through the color.
	v_Color = uColor;
	v_LightColor = uLightColor;
	v_LightPos = vec3(uProjection * vec4(uLightPos, 1.0));
	// Transform the normal's orientation into eye space.
	v_Normal = vec3(uModel * vec4(aNormal, 0.0));
	// UV of the vertex. No special space for this one.
	UV = aTexture;
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = uModel * vPosition;
})glsl";

static const char* SUN_FRAGMENT = R"glsl(
#version 330 core
precision highp float;

in vec2 UV;
in vec4 v_Color;
in vec4 v_LightColor;
in vec3 v_Position;
in vec3 v_Normal;
in vec3 v_LightPos;
in int numOfTextures;
uniform vec3 viewPos;
uniform sampler2D u_texture;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main() {
	vec4 tex1Color = texture2D(u_texture, UV);
	vec4 tex2Color = texture2D(u_texture1, UV);
	vec4 tex3Color = texture2D(u_texture2, UV);

	vec4 finalColor = mix(tex2Color, tex1Color, tex3Color.a);

	// Ambient
	float ambientStrength = 10.0f;
	vec3 ambient = ambientStrength * finalColor.rgb;
	// Diffuse 
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(v_LightPos - v_Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * finalColor.rgb;
	// Specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - v_Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * v_LightColor.rgb * v_Color.rgb;

	gl_FragColor =  vec4((ambient + diffuse + specular), 1.0);
})glsl";

PlanetSun::PlanetSun(GraphicsManager& graphMan, TimeManager& timeMan, glm::vec3& pos, glm::vec3& scale,
	const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE objType, int id,
	const std::string& textureNormalPath, bool enBillBoard, texturesPath_t* texturesPath) : 
	Object3D(graphMan, timeMan, pos, scale, objPath, texturePath, objType, id, textureNormalPath, enBillBoard, texturesPath),
	mGraphicsManager(graphMan), mTimeManager(timeMan)
{
	Log::info("Planet Sun Constructor");

	mTexturePaths.push_back(SUN_TEXTURE);
	mTexturePaths.push_back(SUN_TEXTURE1);
	mTexturePaths.push_back(SUN_TEXTURE2);

	mObjPath = SUN_PATH;
	
	numOfTextures = 3;

	mGraphicsManager.registerComponent(this, mID);
}

status PlanetSun::load()
{
	object3DVec = mGraphicsManager.load3dObject(mObjPath);
	// Loads the vertex buffer into OpenGL.
	for (int i = 0; i < object3DVec.size(); ++i)
	{
		mVertexBuffer.push_back(mGraphicsManager.loadVertexBuffer((uint8_t*)&object3DVec[i].vertices[0], sizeof(glm::vec3) * object3DVec[i].vertices.size()));
		mUVbuffer.push_back(mGraphicsManager.loadVertexBuffer((uint8_t*)&object3DVec[i].uvs[0], sizeof(glm::vec2) * object3DVec[i].uvs.size()));
		mNormalBuffer.push_back(mGraphicsManager.loadVertexBuffer((uint8_t*)&object3DVec[i].normals[0], sizeof(glm::vec3) * object3DVec[i].normals.size()));
		mIndexBuffer.push_back(mGraphicsManager.loadIndexBuffer((uint8_t*)&object3DVec[i].indices[0], sizeof(unsigned int) * object3DVec[i].indices.size()));
		if (mVertexBuffer.back() == 0 || mUVbuffer.back() == 0
			|| mNormalBuffer.back() == 0 || mIndexBuffer.back() == 0)

			goto ERROR;
	}

	// Loads the diffuse texture.
	for (int i = 0; i < mTexturePaths.size(); ++i)
	{
		TextureProperties* textureProperties = mGraphicsManager.loadTextureDiffuse(mTexturePaths[i]);
		if (textureProperties == NULL)
			goto ERROR;
		mTextures.push_back(textureProperties);
	}

	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(SUN_VERTEX, SUN_FRAGMENT);
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	aTexCoords = glGetAttribLocation(mShaderProgram, "aTexture");
	aNormal = glGetAttribLocation(mShaderProgram, "aNormal");
	uLightPos = glGetUniformLocation(mShaderProgram, "uLightPos");
	uPos = glGetUniformLocation(mShaderProgram, "uPos");
	uColor = glGetUniformLocation(mShaderProgram, "uColor");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uModel = glGetUniformLocation(mShaderProgram, "uModel");
	uTexture = glGetUniformLocation(mShaderProgram, "u_texture");
	uTexture2 = glGetUniformLocation(mShaderProgram, "u_texture1");
	uTexture3 = glGetUniformLocation(mShaderProgram, "u_texture2");
	uLightColor = glGetUniformLocation(mShaderProgram, "uLightColor");
	viewPos = glGetUniformLocation(mShaderProgram, "viewPos");
	uNumTextures = glGetUniformLocation(mShaderProgram, "numOfTextures");

	mRot = glm::vec3(glm::radians(180.f));
	mLightPos = SUN_POS;
	mColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	mLightColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	mRotSpeed = 0.0f;

	return STATUS_OK;

ERROR:
	Log::error("Planet Sun Error Loading textures");

	return STATUS_KO;
}

void PlanetSun::update()
{
	mProjection = mGraphicsManager.getMVP();
	mTranslation = glm::translate(mProjection, mPos);
	mRotation = glm::rotate(mTranslation, mAngle, mRot);
	mModel = glm::scale(mRotation, mScale);
}

void PlanetSun::draw()
{
	glDepthFunc(GL_LESS);
	glUseProgram(mShaderProgram);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(aPosition);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(aTexCoords);
	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(aNormal);
	
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &mModel[0][0]);
	glUniform3fv(uLightPos, 1, &mLightPos.r);
	glUniform3fv(viewPos, 1, &mGraphicsManager.getCamPosition().x);
	glUniform3fv(uPos, 1, &mPos.x);
	glUniform4fv(uColor, 1, &mColor.r);
	glUniform4fv(uLightColor, 1, &mLightColor.r);
	glUniform1i(uTexture, 0);
	glUniform1i(uTexture2, 1);
	glUniform1i(uTexture3, 2);
	glUniform1i(uNumTextures, numOfTextures);

	for (int i = 0; i < object3DVec.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[i]);
		glVertexAttribPointer(
			aPosition,							// The attribute we want to configure
			3,									// size
			GL_FLOAT,							// type
			GL_FALSE,							// normalized?
			0,									// stride
			(void*)0							// array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, mUVbuffer[i]);
		glVertexAttribPointer(
			aTexCoords,							// The attribute we want to configure
			2,									// size : U+V => 2
			GL_FLOAT,							// type
			GL_FALSE,							// normalized?
			0,									// stride
			(void*)0							// array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer[i]);
		glVertexAttribPointer(
			aNormal,					  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,							  // stride
			(void*)0                      // array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer[i]);
		for (int k = 0; k < numOfTextures; ++k)
		{
			if (mTextures.size() > k)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, mTextures[k]->texture);
			}
		}
		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(aPosition);
	glDisableVertexAttribArray(aTexCoords);
	glDisableVertexAttribArray(aNormal);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
}

//void PlanetSun::setRotation(float angle, const glm::vec3& axis)
//{
//	mRot = axis;
//	mAngle = angle;
//}
//
//void PlanetSun::setRotation(const glm::vec3& rot, float speed)
//{
//	mRot = rot;
//	mRotSpeed = speed;
//	mAngle = mRotSpeed * mTimeManager.elapsed();
//}
