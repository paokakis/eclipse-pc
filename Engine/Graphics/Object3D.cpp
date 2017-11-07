#include "Object3D.h"
#include "..\tools\Log.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "../Game/common/MapCoordinates.h"
#include <string>
#include "../Engine/Graphics/Configuration/Folders.h"

static const char* VERTEX_GENERAL = R"glsl(
#version 330 core
precision highp float;
// Input vertex data, different for all executions of this shader.
in vec3 aPosition;
in vec2 aTexture;
in vec3 aNormal;
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

static const char* FRAGMENT_GENERAL = R"glsl(
#version 330 core
precision highp float;

struct Material {
    sampler2D diffuse;
	sampler2D normal;
    sampler2D specular;
    float     shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 UV;
in vec4 v_Color;
in vec4 v_LightColor;
in vec3 v_Position;
in vec3 v_Normal;
in vec3 v_LightPos;
uniform vec3 viewPos;
uniform sampler2D u_texture;

uniform Material material;
uniform Light light;

void main() {
	// Ambient
	float ambientStrength = 0.01f;
	vec3 ambient = ambientStrength * light.ambient;
	// Diffuse 
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(v_LightPos - v_Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * texture(material.diffuse, UV);
	// Specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - v_Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * light.specular * vec3(texture(material.specular, UV));
	gl_FragColor = v_Color * vec4((ambient + diffuse + specular), 1.0);
})glsl";

static const char* VERTEX_SHADER = R"glsl(
#version 330 core
precision highp float;
// Input vertex data, different for all executions of this shader.
in vec3 aPosition;
in vec2 aTexture;
in vec3 aNormal;
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

static const char* FRAGMENT_SHADER = R"glsl(
#version 330 core
precision highp float;

struct Material {
    sampler2D diffuse;
	sampler2D normal;
    sampler2D specular;
    float     shininess;
};

in vec2 UV;
in vec4 v_Color;
in vec4 v_LightColor;
in vec3 v_Position;
in vec3 v_Normal;
in vec3 v_LightPos;
in int numOfTextures;
uniform vec3 viewPos;
uniform sampler2D u_texture;
uniform sampler2D u_texture2;

uniform Material material;

vec4 getOutColor()
{
	vec4 outColor;
	
	//if (int(2) == numOfTextures)
	//{
		vec4 tex1 = texture(u_texture, UV);
		vec4 tex2 = texture(u_texture2, UV);
		
		outColor = mix(tex1, tex2, tex1.a);
	//}
	
	return tex1;
}

void main() {
	// Ambient
	float ambientStrength = 0.01f;
	vec3 ambient = ambientStrength * v_LightColor.rgb;
	// Diffuse 
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(v_LightPos - v_Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * v_LightColor.rgb;
	// Specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - v_Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * v_LightColor.rgb;

	

	gl_FragColor = getOutColor() * v_Color * vec4((ambient + diffuse + specular), 1.0);
})glsl";

static const char* VERTEX_NORMAL = R"glsl(
#version 330 core
precision highp float;
in vec3 position;
in vec2 texCoords;
in vec3 normal;
in vec3 tangent;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 uPos;

void main() {
	gl_Position = model * vec4(position, 1.0f);
	vs_out.FragPos = vec3(projection * vec4(uPos, 1.0));
	vs_out.TexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.Normal = vec3(normalize(model * vec4(normal, 0.0)));

	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TBN = TBN;

	vs_out.TangentLightPos = TBN * vec3(projection * vec4(lightPos, 1.0));
	vs_out.TangentViewPos = TBN * vec3(projection * vec4(viewPos, 1.0));
	vs_out.TangentFragPos = TBN * vs_out.FragPos;
	vs_out.Tangent = T;
	vs_out.Bitangent = B;
})glsl";

static const char* FRAGMENT_NORMAL = R"glsl(
#version 330 core
precision highp float;
out vec4 FragColor;
in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{
	vec3 normal = fs_in.Normal;
	mat3 tbn;
	// Obtain normal from normal map in range [0,1]
	normal = texture(normalMap, fs_in.TexCoords).rgb;
	// Transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);
	// Then transform normal in tangent space to world-space via TBN matrix
	//tbn = mat3(fs_in.Tangent, fs_in.Bitangent, fs_in.Normal); // TBN calculated in fragment shader
	//normal = normalize(tbn * normal); // This works!
	//normal = normalize(fs_in.TBN * normal); // This gives incorrect results
	// Get diffuse color
	vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
	// Ambient
	vec3 ambient = 0.1 * color;
	// Diffuse
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;
	// Specular
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec;

	FragColor = vec4(ambient + diffuse + specular, 1.0f);
})glsl";

static const char* STENCIL_VERTEX = R"glsl(
#version 330 core
precision highp float;
layout (location = 0) in vec3 position;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(position, 1.0f);
})glsl";

static const char* STENCIL_FRAGMENT = R"glsl(
#version 330 core
precision highp float;
out vec4 outColor;

void main()
{
    outColor = vec4(1.0, 1.0, 0.0, 1.0);
})glsl";

static const char* SHADOW_VERTEX = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform vec3 uPos;

void main()
{
    gl_Position = model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(projection * vec4(uPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * normal;
    vs_out.TexCoords = texCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
})glsl";

static const char* SHADOW_FRAGMENT = R"glsl(
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // Ambient
    vec3 ambient = 0.3 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // Calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPosLightSpace) : 0.0;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0f);
})glsl";

Object3D::Object3D(GraphicsManager& graphMan, TimeManager& timeMan, glm::vec3& pos, glm::vec3& scale, 
	const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE objType, int id, 
	const std::string& textureNormalPath, bool enBillBoard, texturesPath_t* texturesPath) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mID(id), billBoradEnabled(enBillBoard),
	mObjPath(objPath), mPos(pos), mScale(scale), mObjectType(objType)
{
	Log::info("Object3D constructor with id %d", id);
	{
		if (texturePath != "") mTextureDiffuseResource.push_back(std::string(texturePath));
		if (textureNormalPath != "") mTextureNormalResource.push_back(std::string(textureNormalPath));

		numOfTextures = 1;

		if (texturesPath)
		{
			mTextureMaps.diffusePath = texturesPath->diffuse;
			mTextureMaps.normalPath = texturesPath->normal;
			mTextureMaps.specularPath = texturesPath->specular;
		}
	}
	
	mGraphicsManager.registerComponent(this, mID);
}

status Object3D::load()
{
	object3DVec = mGraphicsManager.load3dObject(mObjPath);
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

	mVAO = mGraphicsManager.loadVertexArray();
	glBindVertexArray(mVAO);
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

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	for (int i = 0; i < object3DVec.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[i]);
		glVertexAttribPointer(
			0,							// The attribute we want to configure
			3,									// size
			GL_FLOAT,							// type
			GL_FALSE,							// normalized?
			0,									// stride
			(void*)0							// array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, mUVbuffer[i]);
		glVertexAttribPointer(
			1,							// The attribute we want to configure
			2,									// size : U+V => 2
			GL_FLOAT,							// type
			GL_FALSE,							// normalized?
			0,									// stride
			(void*)0							// array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer[i]);
		glVertexAttribPointer(
			2,					  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,							  // stride
			(void*)0                      // array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer[i]);
	}

	glBindVertexArray(0);

	// Loads the diffuse texture.
	for (int i = 0; i < mTextureDiffuseResource.size(); ++i)
	{
		TextureProperties* textureProperties = mGraphicsManager.loadTextureDiffuse(mTextureDiffuseResource[i]);
		if (textureProperties == NULL)
			goto ERROR;
		mTextureDiffuseProperties.push_back(textureProperties);
	}	

	if(mTextureMaps.diffusePath != "")
		mTextureMaps.diffuse = mGraphicsManager.loadTextureDiffuse(mTextureMaps.diffusePath);
	if(mTextureMaps.normalPath != "")
		mTextureMaps.normal = mGraphicsManager.loadTextureDiffuse(mTextureMaps.normalPath);
	if(mTextureMaps.specularPath != "")
		mTextureMaps.specular = mGraphicsManager.loadTextureDiffuse(mTextureMaps.specularPath);

	mStencilShader = mGraphicsManager.loadShader(STENCIL_VERTEX, STENCIL_FRAGMENT);
	if (!mStencilShader)
		goto ERROR;

	asPosition = glGetAttribLocation(mStencilShader, "position");
	usModel = glGetUniformLocation(mStencilShader, "model");

	mShadowProgram = mGraphicsManager.loadShader(SHADOW_VERTEX, SHADOW_FRAGMENT);
	if (!mShadowProgram)
		goto ERROR;

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
		uNumTextures = glGetUniformLocation(mShaderProgram, "numOfTextures");
	}
	else if (OBJECT3D_GENERIC == mObjectType || OBJECT3D_CUBE == mObjectType || OBJECT3D_POINTS == mObjectType)
	{

		// Creates and retrieves shader attributes and uniforms.
		mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER); // mGraphicsManager.loadShaderFile(STANDARD_VERTEX, STANDARD_FRAGMENT);
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
		uTexture2 = glGetUniformLocation(mShaderProgram, "u_texture2");
		uLightColor = glGetUniformLocation(mShaderProgram, "uLightColor");
		viewPos = glGetUniformLocation(mShaderProgram, "viewPos");
		uNumTextures = glGetUniformLocation(mShaderProgram, "numOfTextures");
	}
		
	// billboard
	if(billBoradEnabled)
	{		
		mpBillBoard = new BillBoard(mGraphicsManager, 100.f, mPos);
	}

	mGraphicsManager.glErrorCheck();
	
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

status Object3D::addTextureDiffuse(const std::string& texturePath)
{
	if (texturePath != "") mTextureDiffuseResource.push_back(std::string(texturePath));
	TextureProperties* textureProperties = mGraphicsManager.loadTextureDiffuse(mTextureDiffuseResource.back());
	if (textureProperties == NULL)
	{
		Log::error("Error adding texture %s", texturePath.c_str());
		return STATUS_KO;
	}
	mTextureDiffuseProperties.push_back(textureProperties);
	numOfTextures++;

	return STATUS_OK;
}

void Object3D::initialize()
{
	mActive = true;
}

void Object3D::setTexture(const std::vector<TextureProperties*>& textProperties)
{
	mTextureDiffuseProperties = textProperties;
}

void Object3D::setTexture(TextureProperties* textProperties)
{
	mTextureDiffuseProperties.clear();
	mTextureDiffuseProperties.push_back(textProperties);
}

void Object3D::setLightPos(const glm::vec3& lPos)
{
	mLightPos = lPos;
}

void Object3D::setColor(const glm::vec4& color)
{
	mColor = color;
}

void Object3D::setRotation(float angle, const glm::vec3& axis)
{
	mRot = axis;
	mAngle = angle;
}

void Object3D::setRotation(const glm::vec3& rot, float speed)
{
	mRot = rot;
	mRotSpeed = speed;
	mAngle = mRotSpeed * mTimeManager.elapsed();
}

void Object3D::update()
{
	mProjection = mGraphicsManager.getMVP();
	mTranslation = glm::translate(mProjection, mPos);
	mRotation = glm::rotate(mTranslation, mAngle, mRot);
	mModel = glm::scale(mRotation, mScale);
}

void Object3D::draw3D()
{
	if (mActive)
	{
		glDepthFunc(GL_LESS);
		glUseProgram(mShaderProgram);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

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
			glUniform1i(uNumTextures, numOfTextures);
		}
		else if (OBJECT3D_GENERIC == mObjectType || OBJECT3D_CUBE == mObjectType || OBJECT3D_POINTS == mObjectType)
		{
			glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
			glUniformMatrix4fv(uModel, 1, GL_FALSE, &mModel[0][0]);
			glUniform3fv(uLightPos, 1, &mLightPos.r);
			glUniform3fv(viewPos, 1, &mGraphicsManager.getCamPosition().x);
			glUniform3fv(uPos, 1, &mPos.x);
			glUniform4fv(uColor, 1, &mColor.r);
			glUniform4fv(uLightColor, 1, &mLightColor.r);
			if (numOfTextures == 1)
			{
				glUniform1i(uTexture, 0);
			}
			else
			{
				glUniform1i(uTexture, 0);
				glUniform1i(uTexture2, 1);
			}
			glUniform1i(uNumTextures, numOfTextures);
		}

		glBindVertexArray(mVAO);
		for (int i = 0; i < object3DVec.size(); ++i)
		{
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
				for (int k = 0; k < numOfTextures; ++k)
				{
					if (mTextureDiffuseProperties.size() > k)
					{
						glActiveTexture(GL_TEXTURE0 + k);
						glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[k]->texture);
					}
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
				for (int k = 0; k < numOfTextures; ++k)
				{
					if (mTextureDiffuseProperties.size() > k)
					{
						glActiveTexture(GL_TEXTURE0 + k);
						glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[k]->texture);
					}
				}	// diffuse
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

		glBindVertexArray(0);
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (mObjectType == OBJECT3D_NORMALS)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(aTangent);
		}
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
	}
}

void Object3D::drawSilhuette()
{
	if (mSilhuetteEnabled && mActive)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		glm::mat4 model = glm::scale(mModel, glm::vec3(mSilhuetteScale));

		glUseProgram(mStencilShader);
		glUniformMatrix4fv(usModel, 1, GL_FALSE, &model[0][0]);

		glEnableVertexAttribArray(asPosition);

		for (int i = 0; i < object3DVec.size(); ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[i]);
			glVertexAttribPointer(
				asPosition,							// The attribute we want to configure
				3,									// size
				GL_FLOAT,							// type
				GL_FALSE,							// normalized?
				0,									// stride
				(void*)0							// array buffer offset
			);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer[i]);
			// Draw the triangles !
			glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void Object3D::drawShadows(GLuint depthTexture, glm::mat4 lightSpaceMatrix)
{
	if (mActive)
	{
		glDepthFunc(GL_LESS);
		glUseProgram(mShadowProgram);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		glUniform1i(glGetUniformLocation(mShadowProgram, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(mShadowProgram, "shadowMap"), 1);
		glUniformMatrix4fv(glGetUniformLocation(mShadowProgram, "projection"), 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
		glUniformMatrix4fv(glGetUniformLocation(mShadowProgram, "view"), 1, GL_FALSE, &mGraphicsManager.getViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(mShadowProgram, "model"), 1, GL_FALSE, &mModel[0][0]);
		// Set light uniforms
		glUniform3fv(glGetUniformLocation(mShadowProgram, "lightPos"), 1, &mLightPos.r);
		glUniform3fv(glGetUniformLocation(mShadowProgram, "viewPos"), 1, &mGraphicsManager.getCamPosition().x);
		glUniformMatrix4fv(glGetUniformLocation(mShadowProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
		
		glUniform1i(glGetUniformLocation(mShadowProgram, "shadows"), true);

		glUniform3fv(glGetUniformLocation(mShadowProgram, "uPos"), 1, &mPos.x);
		

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(aPosition);
		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(aTexCoords);
		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(aNormal);
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
				aNormal,							// The attribute we want to configure
				3,								    // size
				GL_FLOAT,							// type
				GL_FALSE,							// normalized?
				0,									// stride
				(void*)0							// array buffer offset
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
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, depthTexture);
						// Draw the triangles !
						glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size() - j, GL_UNSIGNED_INT, nullptr);
					}
					else
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, depthTexture);
						// Draw the triangles !
						glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size() - j, GL_UNSIGNED_INT, nullptr);
					}
				}
				break;
			case OBJECT3D_GENERIC:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[0]->texture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				// Draw the triangles !
				glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
				break;
			default:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[0]->texture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				// Draw the triangles !
				glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
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
}

void Object3D::drawDepth(GLuint programID)
{
	if (mActive)
	{
		glDepthFunc(GL_LESS);
		//glUseProgram(mSimpleDepthProgram);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &mModel[0][0]);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		for (int i = 0; i < object3DVec.size(); ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[i]);
			glVertexAttribPointer(
				0,									// The attribute we want to configure
				3,									// size
				GL_FLOAT,							// type
				GL_FALSE,							// normalized?
				0,									// stride
				(void*)0							// array buffer offset
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
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[0]->texture);
				// Draw the triangles !
				glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
				break;
			default:
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[0]->texture);
				// Draw the triangles !
				glDrawElements(GL_TRIANGLES, object3DVec[i].indices.size(), GL_UNSIGNED_INT, nullptr);
				break;
			}

		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
	}
}