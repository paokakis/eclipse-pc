#include "LandScape.h"
#include "../Engine/tools/Log.hpp"
#include "../Game/common/MapCoordinates.h"
#include "../Engine/Graphics/Configuration/Folders.h"
#include <FreeImage.h>

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

static int mId = 1330;
#define FOR(q,n) for(int q=0;q<n;q++)
#define SFOR(q,s,e) for(int q=s;q<=e;q++)
#define RFOR(q,n) for(int q=n;q>=0;q--)
#define RSFOR(q,s,e) for(int q=s;q>=e;q--)

#define ESZ(elem) (int)elem.size()

LandScape::LandScape(TimeManager& timeMan, GraphicsManager& graphMan, BphysicsManager& physMan, glm::vec3& scale, std::string heightMapPath, std::string texturePath) :
	mGraphicsManager(graphMan), mPhysicsManager(physMan), mScale(scale), mHeightMapPath(heightMapPath)
{
	Log::info("Landscape constructor.");

	if (texturePath != "") mTextureDiffuseResource.push_back(std::string(texturePath));
	mPos = LAND_POS;
	mScale = LAND_SCALE;

	mGraphicsManager.registerComponent(this, mId);
}

void LandScape::initialize()
{
	Log::info("Landscape initialize");
}

void LandScape::update()
{
	mProjection = mGraphicsManager.getMVP();
	mTranslation = glm::translate(mProjection, mPos);
	mRotation = glm::rotate(mTranslation, mAngle, mRot);
	mModel = glm::scale(mRotation, mScale);
}

LandScape::~LandScape()
{
	Log::info("Landscape destructor.");

}

status LandScape::load()
{
	if (retStatus == STATUS_OK)
	{
		retStatus = STATUS_KO;
		ReleaseHeightmap();
	}
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(mHeightMapPath.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(mHeightMapPath.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, mHeightMapPath.c_str());
	if (!dib)
		return false;

	unsigned char* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data
	iRows = FreeImage_GetHeight(dib);
	iCols = FreeImage_GetWidth(dib);

	// We also require our image to be either 24-bit (classic RGB) or 8-bit (luminance)
	if (bDataPointer == NULL || iRows == 0 || iCols == 0 || (FreeImage_GetBPP(dib) != 24 && FreeImage_GetBPP(dib) != 8))
		return false;

	// How much to increase data pointer to get to next pixel data
	unsigned int ptr_inc = FreeImage_GetBPP(dib) == 24 ? 3 : 1;
	// Length of one row in data
	unsigned int row_step = ptr_inc*iCols;

	// vboHeightmapData.CreateVBO();
	// All vertex data are here (there are iRows*iCols vertices in this heightmap), we will get to normals later
	std::vector< std::vector< glm::vec3> > vVertexData(iRows, std::vector<glm::vec3>(iCols));
	std::vector< std::vector< glm::vec2> > vCoordsData(iRows, std::vector<glm::vec2>(iCols));

	float fTextureU = float(iCols)*0.1f;
	float fTextureV = float(iRows)*0.1f;

	FOR(i, iRows)
	{
		FOR(j, iCols)
		{
			float fScaleC = float(j) / float(iCols - 1);
			float fScaleR = float(i) / float(iRows - 1);
			float fVertexHeight = float(*(bDataPointer + row_step*i + j*ptr_inc)) / 255.0f;
			vVertexData[i][j] = glm::vec3(-0.5f + fScaleC, fVertexHeight, -0.5f + fScaleR);
			vCoordsData[i][j] = glm::vec2(fTextureU*fScaleC, fTextureV*fScaleR);
		}
	}

	// Normals are here - the heightmap contains ( (iRows-1)*(iCols-1) quads, each one containing 2 triangles, therefore array of we have 3D array)
	std::vector< std::vector<glm::vec3> > vNormals[2];
	FOR(i, 2)vNormals[i] = std::vector< std::vector<glm::vec3> >(iRows - 1, std::vector<glm::vec3>(iCols - 1));

	FOR(i, iRows - 1)
	{
		FOR(j, iCols - 1)
		{
			glm::vec3 vTriangle0[] =
			{
				vVertexData[i][j],
				vVertexData[i + 1][j],
				vVertexData[i + 1][j + 1]
			};
			glm::vec3 vTriangle1[] =
			{
				vVertexData[i + 1][j + 1],
				vVertexData[i][j + 1],
				vVertexData[i][j]
			};

			glm::vec3 vTriangleNorm0 = glm::cross(vTriangle0[0] - vTriangle0[1], vTriangle0[1] - vTriangle0[2]);
			glm::vec3 vTriangleNorm1 = glm::cross(vTriangle1[0] - vTriangle1[1], vTriangle1[1] - vTriangle1[2]);

			vNormals[0][i][j] = glm::normalize(vTriangleNorm0);
			vNormals[1][i][j] = glm::normalize(vTriangleNorm1);
		}
	}

	std::vector< std::vector<glm::vec3> > vFinalNormals = std::vector< std::vector<glm::vec3> >(iRows, std::vector<glm::vec3>(iCols));

	FOR(i, iRows)
		FOR(j, iCols)
	{
		// Now we wanna calculate final normal for [i][j] vertex. We will have a look at all triangles this vertex is part of, and then we will make average vector
		// of all adjacent triangles' normals

		glm::vec3 vFinalNormal = glm::vec3(0.0f, 0.0f, 0.0f);

		// Look for upper-left triangles
		if (j != 0 && i != 0)
			FOR(k, 2)vFinalNormal += vNormals[k][i - 1][j - 1];
		// Look for upper-right triangles
		if (i != 0 && j != iCols - 1)vFinalNormal += vNormals[0][i - 1][j];
		// Look for bottom-right triangles
		if (i != iRows - 1 && j != iCols - 1)
			FOR(k, 2)vFinalNormal += vNormals[k][i][j];
		// Look for bottom-left triangles
		if (i != iRows - 1 && j != 0)
			vFinalNormal += vNormals[1][i][j - 1];
		vFinalNormal = glm::normalize(vFinalNormal);

		vFinalNormals[i][j] = vFinalNormal; // Store final normal of j-th vertex in i-th row
	}

	// First, create a VBO with only vertex data
	m_VAO = mGraphicsManager.loadVertexArray();
	glBindVertexArray(m_VAO);
	FOR(i, iRows)
	{
		FOR(j, iCols)
		{
			mVertexBuffer.push_back(mGraphicsManager.loadVertexBuffer(&vVertexData[i][j], sizeof(glm::vec3))); // Add vertex
			mUVbuffer.push_back(mGraphicsManager.loadVertexBuffer(&vCoordsData[i][j], sizeof(glm::vec2))); // Add tex. coord
			mNormalBuffer.push_back(mGraphicsManager.loadVertexBuffer(&vFinalNormals[i][j], sizeof(glm::vec3))); // Add normal
		}
	}
	// Now create a VBO with heightmap indices
	//vboHeightmapIndices.CreateVBO();
	int iPrimitiveRestartIndex = iRows*iCols;
	FOR(i, iRows - 1)
	{
		FOR(j, iCols)
			FOR(k, 2)
		{
			int iRow = i + (1 - k);
			int iIndex = iRow*iCols + j;
			mIndexBuffer.push_back(mGraphicsManager.loadIndexBuffer(&iIndex, sizeof(int)));
		}
		// Restart triangle strips
		mIndexBuffer.push_back(mGraphicsManager.loadIndexBuffer(&iPrimitiveRestartIndex, sizeof(int)));
	}

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// And now attach index data to this VAO
	// Here don't forget to bind another type of VBO - the element array buffer, or simplier indices to vertices
	glBindVertexArray(0);

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

	mRot = glm::vec3(glm::radians(180.f));
	mLightPos = SUN_POS;
	mColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	mLightColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
	mRotSpeed = 0.0f;

	retStatus = STATUS_OK; // If get here, we succeeded with generating heightmap
	return retStatus;

ERROR:
	retStatus = STATUS_KO;
	return STATUS_KO;
}

void LandScape::ReleaseHeightmap()
{
	// happens automatically.
}

void LandScape::draw()
{
	glUseProgram(mShaderProgram);

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

	// Now we're ready to render - we are drawing set of triangle strips using one call, but we g otta enable primitive restart
	glBindVertexArray(m_VAO);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(iRows*iCols);

	for (int k = 0; k < numOfTextures; ++k)
	{
		if (mTextureDiffuseProperties.size() > k)
		{
			glActiveTexture(GL_TEXTURE0 + k);
			glBindTexture(GL_TEXTURE_2D, mTextureDiffuseProperties[k]->texture);
		}
	}

	int iNumIndices = (iRows - 1)*iCols * 2 + iRows - 1;
	glDrawElements(GL_TRIANGLE_STRIP, iNumIndices, GL_UNSIGNED_INT, 0);
}