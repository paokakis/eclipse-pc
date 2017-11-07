#include "Object3D1.h"
#include "..\tools\Log.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "../Game/common/MapCoordinates.h"
#include <string>
#include "../Engine/Graphics/Configuration/Folders.h"

static const char* VERTEX_SHADER =
"#version 150 core\n"
"precision highp float;\n"
// Input vertex data, different for all executions of this shader.
"in vec3 aPosition;\n"
"in vec2 aTexture;\n"
"in vec3 aNormal;\n"
"uniform mat4 uProjection;\n"
"uniform mat4 uModel;\n"
"uniform vec4 uColor;\n"
"uniform vec4 uLightColor;\n"
"uniform vec3 uLightPos;\n"
"uniform vec3 uPos;\n"
"out vec4 v_Color;\n"
"out vec3 v_Position;\n"
"out vec3 v_Normal;\n"
"out vec3 v_LightPos;\n"
"out vec4 v_LightColor;\n"
// Output data ; will be interpolated for each fragment.
"out vec2 UV;\n"
// Values that stay constant for the whole mesh.
"void main() {\n"
" vec4 vPosition = vec4(aPosition, 1.0);\n"
// Transform the vertex into eye space.
" v_Position = vec3(uProjection * vec4(vPosition.xyz + uPos.xyz, 1.0));\n"
// Pass through the color.
" v_Color = uColor;\n"
" v_LightColor = uLightColor;\n"
" v_LightPos = vec3(uProjection * vec4(uLightPos, 1.0));\n"
// Transform the normal's orientation into eye space.
" v_Normal = vec3(uModel * vec4(aNormal, 0.0));\n"
// UV of the vertex. No special space for this one.
" UV = aTexture;\n"
// Output position of the vertex, in clip space : MVP * position
" gl_Position = uModel * vPosition;\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"in vec2 UV;\n"
"in vec4 v_Color;\n"
"in vec4 v_LightColor;\n"
"in vec3 v_Position;\n"
"in vec3 v_Normal;\n"
"in vec3 v_LightPos;\n"
"uniform vec3 viewPos;\n"
"uniform sampler2D u_texture;\n"
"void main() {\n"
// Ambient
" float ambientStrength = 0.1f;\n"
" vec3 ambient = ambientStrength * v_LightColor.rgb;\n"
// Diffuse 
" vec3 norm = normalize(v_Normal);\n"
" vec3 lightDir = normalize(v_LightPos - v_Position);\n"
" float diff = max(dot(norm, lightDir), 0.0);\n"
" vec3 diffuse = diff * v_LightColor.rgb;\n"
// Specular
" float specularStrength = 0.5f;\n"
" vec3 viewDir = normalize(viewPos - v_Position);\n"
" vec3 reflectDir = reflect(-lightDir, norm);\n"
" float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
" vec3 specular = specularStrength * spec * v_LightColor.rgb;\n"
" gl_FragColor = texture2D(u_texture, UV) * v_Color * vec4((ambient + diffuse + specular), 1.0);\n"
"}";

static const char* VERTEX_NORMAL =
"#version 150 core\n"
"precision highp float;\n"
"in vec3 position;\n"
"in vec2 texCoords;\n"
"in vec3 normal;\n"
"in vec3 tangent;\n"

"out VS_OUT{\n"
" vec3 FragPos;\n"
" vec3 Normal;\n"
" vec2 TexCoords;\n"
" vec3 Tangent;\n"
" vec3 Bitangent;\n"
" mat3 TBN;\n"
" vec3 TangentLightPos;\n"
" vec3 TangentViewPos;\n"
" vec3 TangentFragPos;\n"
"} vs_out;\n"

"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"

"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"

"uniform vec3 uPos;\n"

"void main()\n"
"{\n"
" gl_Position = model * vec4(position, 1.0f);\n"
" vs_out.FragPos = vec3(projection * vec4(uPos, 1.0));\n"
" vs_out.TexCoords = texCoords;\n"

" mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
" vs_out.Normal = vec3(normalize(model * vec4(normal, 0.0)));\n"

" vec3 T = normalize(normalMatrix * tangent);\n"
" vec3 N = normalize(normalMatrix * normal);\n"
" T = normalize(T - dot(T, N) * N);\n"
" vec3 B = cross(N, T);\n"
" mat3 TBN = transpose(mat3(T, B, N));\n"
" vs_out.TBN = TBN;\n"

" vs_out.TangentLightPos = TBN * vec3(projection * vec4(lightPos, 1.0));\n"
" vs_out.TangentViewPos = TBN * vec3(projection * vec4(viewPos, 1.0));\n"
" vs_out.TangentFragPos = TBN * vs_out.FragPos;\n"
" vs_out.Tangent = T;\n"
" vs_out.Bitangent = B;\n"
"}\n";

static const char* FRAGMENT_NORMAL =
"#version 150 core\n"
"precision highp float;\n"
"out vec4 FragColor;\n"
"in VS_OUT{\n"
" vec3 FragPos;\n"
" vec3 Normal;\n"
" vec2 TexCoords;\n"
" vec3 Tangent;\n"
" vec3 Bitangent;\n"
" mat3 TBN;\n"
" vec3 TangentLightPos;\n"
" vec3 TangentViewPos;\n"
" vec3 TangentFragPos;\n"
"} fs_in;\n"

" uniform sampler2D diffuseMap;\n"
" uniform sampler2D normalMap;\n"

"void main()\n"
"{\n"
" vec3 normal = fs_in.Normal;\n"
" mat3 tbn;\n"
// Obtain normal from normal map in range [0,1]
" normal = texture(normalMap, fs_in.TexCoords).rgb;\n"
// Transform normal vector to range [-1,1]
" normal = normalize(normal * 2.0 - 1.0);\n"
// Then transform normal in tangent space to world-space via TBN matrix
//" tbn = mat3(fs_in.Tangent, fs_in.Bitangent, fs_in.Normal);\n" // TBN calculated in fragment shader
//" normal = normalize(tbn * normal);\n" // This works!
//" normal = normalize(fs_in.TBN * normal);\n" // This gives incorrect results
// Get diffuse color
" vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;\n"
// Ambient
" vec3 ambient = 0.1 * color;\n"
// Diffuse
" vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);\n"
" float diff = max(dot(lightDir, normal), 0.0);\n"
" vec3 diffuse = diff * color;\n"
// Specular
" vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);\n"
" vec3 reflectDir = reflect(-lightDir, normal);\n"
" vec3 halfwayDir = normalize(lightDir + viewDir);\n"
" float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);\n"
" vec3 specular = vec3(0.2) * spec;\n"

" FragColor = vec4(ambient + diffuse + specular, 1.0f);\n"
"}\n";


Object3D1::Object3D1(GraphicsManager& graphMan, TimeManager& timeMan, glm::vec3& pos, glm::vec3& scale,
	const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE objType,
	const std::string& textureNormalPath, bool enBillBoard) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), billBoradEnabled(enBillBoard),
	mPath(objPath), mPos(pos), mScale(scale), mObjectType(objType)
{
	Log::info("Object3D1 constructor");
	if (texturePath != "") mTextureDiffuseResource.push_back(std::string(texturePath));
	if (textureNormalPath != "") mTextureNormalResource.push_back(std::string(textureNormalPath));
}

status Object3D1::load()
{
	object3DVec = mGraphicsManager.load3dObject(mPath);
	switch (mObjectType)
	{
	case OBJECT3D_CUBE:
		for (int i = 0; i < object3DVec[0].uvs.size(); i += 4)
		{
			object3DVec[0].uvs[i].x = 1;
			object3DVec[0].uvs[i].y = 0;

			object3DVec[0].uvs[i + 1].x = 1;
			object3DVec[0].uvs[i + 1].y = 1;

			object3DVec[0].uvs[i + 2].x = 0;
			object3DVec[0].uvs[i + 2].y = 1;

			object3DVec[0].uvs[i + 3].x = 0;
			object3DVec[0].uvs[i + 3].y = 0;
		}
		break;
	case OBJECT3D_GENERIC:
		break;
	case OBJECT3D_NORMALS:
		break;
	default:
		break;
	}

	// Loads the vertex buffer into OpenGL.
	for (int i = 0; i < object3DVec.size(); ++i)
	{
		mVertexBuffer.push_back(mGraphicsManager.loadVertexBuffer((uint8_t*)&object3DVec[i].vertices[0], sizeof(glm::vec3) * object3DVec[i].vertices.size()));
		mUVbuffer.push_back(mGraphicsManager.loadVertexBuffer((uint8_t*)&object3DVec[i].uvs[0], sizeof(glm::vec2) * object3DVec[i].uvs.size()));
		mNormalBuffer.push_back(mGraphicsManager.loadVertexBuffer((uint8_t*)&object3DVec[i].normals[0], sizeof(glm::vec3) * object3DVec[i].normals.size()));
		mIndexBuffer.push_back(mGraphicsManager.loadIndexBuffer((uint8_t*)&object3DVec[i].indices[0], sizeof(unsigned int) * object3DVec[i].indices.size()));
		// tangent & bitTangent info
		mTangentBuffer.push_back(mGraphicsManager.loadVertexBuffer(&object3DVec[i].tangents[0], sizeof(glm::vec3) * object3DVec[i].tangents.size()));
		mBitangentBuffer.push_back(mGraphicsManager.loadVertexBuffer(&object3DVec[i].bitangents[0], sizeof(glm::vec3) * object3DVec[i].bitangents.size()));

		if (mVertexBuffer.back() == 0 || mUVbuffer.back() == 0
			|| mNormalBuffer.back() == 0 || mIndexBuffer.back() == 0
			|| mTangentBuffer.back() == 0)

			goto ERROR;
	}

	// Loads the diffuse texture.
	for (int i = 0; i < mTextureDiffuseResource.size(); ++i)
	{
		TextureProperties* textureProperties = mGraphicsManager.loadTextureDiffuse(mTextureDiffuseResource[i]);
		if (textureProperties == NULL)
			goto ERROR;
		mTextureDiffuseProperties.push_back(textureProperties);
	}

	if (mObjectType == OBJECT3D_NORMALS)
	{
		// Loads the normal texture.
		for (int i = 0; i < mTextureNormalResource.size(); ++i)
		{
			TextureProperties* textureProperties = mGraphicsManager.loadTextureDiffuse(mTextureNormalResource[i]);
			if (textureProperties == NULL)
				goto ERROR;
			mTextureNormalProperties.push_back(textureProperties);
		}

		// Creates and retrieves shader attributes and uniforms.
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_NORMAL, FRAGMENT_NORMAL);
		if (mShaderProgram == 0)
			goto ERROR;

		aPosition = glGetAttribLocation(mShaderProgram, "position");
		aNormal = glGetAttribLocation(mShaderProgram, "normal");
		aTexCoords = glGetAttribLocation(mShaderProgram, "texCoords");
		aTangent = glGetAttribLocation(mShaderProgram, "tangent");
		uProjection = glGetUniformLocation(mShaderProgram, "projection");
		uView = glGetUniformLocation(mShaderProgram, "view");
		uModel = glGetUniformLocation(mShaderProgram, "model");
		uLightPos = glGetUniformLocation(mShaderProgram, "lightPos");
		uPos = glGetUniformLocation(mShaderProgram, "uPos");
		viewPos = glGetUniformLocation(mShaderProgram, "viewPos");
		diffuseMap = glGetUniformLocation(mShaderProgram, "diffuseMap");
		normalMap = glGetUniformLocation(mShaderProgram, "normalMap");
	}
	else if (OBJECT3D_GENERIC == mObjectType || OBJECT3D_CUBE == mObjectType || OBJECT3D_POINTS == mObjectType)
	{
		// Creates and retrieves shader attributes and uniforms.
		mShaderProgram = mGraphicsManager.loadShaderFile(STANDARD_VERTEX, STANDARD_FRAGMENT);
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
		uLightColor = glGetUniformLocation(mShaderProgram, "uLightColor");
		viewPos = glGetUniformLocation(mShaderProgram, "viewPos");
	}

	// billboard
	if (billBoradEnabled)
	{
		mpBillBoard = new BillBoard(mGraphicsManager, 100.f, mPos);
	}

	mRot = glm::vec3(glm::radians(180.f));
	mLightPos = SUN_POS;
	mColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	mLightColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	mRotSpeed = 0.0f;

	Log::info("Object3D loaded successfuly , num of objects %d.", object3DVec.size());

	return STATUS_OK;
ERROR:
	Log::error("Error loading Object3D");

	return STATUS_KO;
}

void Object3D1::setRotation(float angle, const glm::vec3& axis)
{
	mRot = axis;
	mAngle = angle;
}

void Object3D1::setTexture(const std::vector<TextureProperties*>& textProperties)
{
	mTextureDiffuseProperties = textProperties;
}

void Object3D1::setTexture(TextureProperties* textProperties)
{
	mTextureDiffuseProperties.clear();
	mTextureDiffuseProperties.push_back(textProperties);
}

void Object3D1::setLightPos(const glm::vec3& lPos)
{
	mLightPos = lPos;
}

void Object3D1::setColor(const glm::vec4& color)
{
	mColor = color;
}

void Object3D1::setRotation(const glm::vec3& rot, float speed)
{
	mRot = rot;
	mRotSpeed = speed;
	mAngle = mRotSpeed * mTimeManager.elapsed();
}

void Object3D1::update()
{
	mProjection = mGraphicsManager.getMVP();
	mTranslation = glm::translate(mProjection, mPos);
	mRotation = glm::rotate(mTranslation, mAngle, mRot);
	mModel = glm::scale(mRotation, mScale);
}

void Object3D1::draw()
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
	if (mObjectType == OBJECT3D_NORMALS)
	{
		// 4th attribute buffer : Tangent
		glEnableVertexAttribArray(aTangent);
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &mModel[0][0]);
		glUniformMatrix4fv(uView, 1, GL_FALSE, &mGraphicsManager.getViewMatrix()[0][0]);
		glUniform3fv(uLightPos, 1, &mLightPos.r);
		glUniform3fv(viewPos, 1, &mGraphicsManager.getCamPosition().x);
		glUniform3fv(uPos, 1, &mPos.x);
		glUniform1i(diffuseMap, 0);
		glUniform1i(normalMap, 1);
	}
	else if (mObjectType == OBJECT3D_GENERIC || mObjectType == OBJECT3D_CUBE || OBJECT3D_POINTS == mObjectType)
	{
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &mModel[0][0]);
		glUniform3fv(uLightPos, 1, &mLightPos.r);
		glUniform3fv(viewPos, 1, &mGraphicsManager.getCamPosition().x);
		glUniform3fv(uPos, 1, &mPos.x);
		glUniform4fv(uColor, 1, &mColor.r);
		glUniform4fv(uLightColor, 1, &mLightColor.r);
		glUniform1i(uTexture, 0);
	}

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
		switch (mObjectType)
		{
		case OBJECT3D_CUBE:
			for (int j = 0; j < 36; j += 6)
			{
				if (mTextureDiffuseProperties.size() >(j / 6))
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[j / 6]->texture);
					// Draw the triangles !
					glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size() - j, GL_UNSIGNED_INT, nullptr);
				}
				else
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, 0);
					// Draw the triangles !
					glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size() - j, GL_UNSIGNED_INT, nullptr);
				}
			}
			break;
		case OBJECT3D_GENERIC:
			if (mTextureDiffuseProperties.size() > i)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[i]->texture);
			}
			// Draw the triangles !
			glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
			break;
		case OBJECT3D_NORMALS:
			glBindBuffer(GL_ARRAY_BUFFER, mTangentBuffer[i]);
			glVertexAttribPointer(
				aTangent,					  // The attribute we want to configure
				3,                            // size
				GL_FLOAT,                     // type
				GL_FALSE,                     // normalized?
				0,							  // stride
				(void*)0                      // array buffer offset
			);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[0]->texture);	// diffuse
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mTextureNormalProperties[0]->texture);		// normal

			glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
			break;
		case OBJECT3D_POINTS:
			if (mTextureDiffuseProperties.size() > i)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[i]->texture);
			}
			// Draw the Points !
			glDrawElements(GL_POINTS, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
			break;
		default:
			break;
		}

	}

	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(aPosition);
	glDisableVertexAttribArray(aTexCoords);
	glDisableVertexAttribArray(aNormal);
	if (mObjectType == OBJECT3D_NORMALS)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableVertexAttribArray(aTangent);
	}
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
}