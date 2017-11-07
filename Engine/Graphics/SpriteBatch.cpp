#include "SpriteBatch.h"
#include "../Tools/Log.hpp"

/*Write the GLSL vertex and fragment shaders as constant strings.
The shader code is written inside a main() function similar to what can be coded
in C. As any normal computer program, shaders require variables to process data:
attributes (per-vertex data like the position), uniforms (global parameters per draw
call), and varying (values interpolated per fragment like the texture coordinates).
Here, texture coordinates are passed to the fragment shader in vTexture. The
vertex position is transformed from a 2D vector to a 4D vector into a predefined
GLSL variable gl_Position. The fragment shader retrieves interpolated texture
coordinates in vTexture. This information is used as an index in the predefined
function texture2D() to access the texture color. Color is saved in the predefined
output variable gl_FragColor, which represents the final pixel:*/

static const char* VERTEX_SHADER =
"#version 330\n"
"precision highp float;\n"
"attribute vec4 aPosition;\n"
"attribute vec2 aTexture;\n"
"uniform float uTexSize;\n"
"varying vec2 vTexture;\n"
"uniform mat4 uProjection;\n"
"void main() {\n"
" vTexture = aTexture * uTexSize;\n"
" gl_Position = uProjection * aPosition;\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 330\n"
"precision highp float;\n"
"varying vec2 vTexture;\n"
"uniform sampler2D u_texture;\n"
"void main() {\n"
" gl_FragColor = texture2D(u_texture, vTexture);\n"
"}";

static bool loaded = false;

SpriteBatch::SpriteBatch(TimeManager& pTimeManager, GraphicsManager& pGraphicsManager) :
	mTimeManager(pTimeManager),
	mGraphicsManager(pGraphicsManager),
	mSprites(), mVertices(), mIndexes(),
	mShaderProgram(0),
	aPosition(0), aTexture(0), uProjection(0), uTexture(0),
	mTexSize(1.0f), active(false)
{
	mGraphicsManager.registerComponent(this, (uint16_t)20);
}

void SpriteBatch::initialize()
{
	spriteVectorIterator spriteIt;
	for (spriteIt = mSprites.begin(); spriteIt < mSprites.end(); ++spriteIt)
	{
		(*spriteIt)->initialize();
		if (!loaded) (*spriteIt)->load(mGraphicsManager); // TODO if game screen not pointer REMOVE THIS!! (check guimanager)
	}
}

SpriteBatch::~SpriteBatch()
{
	spriteVectorIterator spriteIt;
	for (spriteIt = mSprites.begin(); spriteIt < mSprites.end(); ++spriteIt)
	{
		delete (*spriteIt);
	}
}

Sprite* SpriteBatch::registerSprite(const std::string pTextureResource, int32_t pHeight, int32_t pWidth, float scale, glm::vec3& pLoc)
{
	int32_t spriteCount = mSprites.size();
	int32_t index = spriteCount * 4; // Points to 1st vertex.
	// Precomputes the index buffer.
	mIndexes.push_back(index + 0); mIndexes.push_back(index + 1);
	mIndexes.push_back(index + 2); mIndexes.push_back(index + 2);
	mIndexes.push_back(index + 1); mIndexes.push_back(index + 3);
	for (int i = 0; i < 4; ++i)
	{
		mVertices.push_back(Sprite::Vertex());
	}
	// Appends a new sprite to the sprite array.
	mSprites.push_back(new Sprite(mGraphicsManager, pTextureResource, pHeight, pWidth, pLoc));
	mTexSize.push_back(1.0f / scale);

	loaded = false;

	return mSprites.back();
}

status SpriteBatch::load()
{
	Log::info("SpriteBatch load");

	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0) return STATUS_KO;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	aTexture = glGetAttribLocation(mShaderProgram, "aTexture");
	uTexSize = glGetUniformLocation(mShaderProgram, "uTexSize");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uTexture = glGetUniformLocation(mShaderProgram, "u_texture");
	// Loads sprites.
	spriteVectorIterator spriteIt;
	for (spriteIt = mSprites.begin(); spriteIt < mSprites.end(); ++spriteIt)
	{
		if ((*spriteIt)->load(mGraphicsManager) != STATUS_OK) goto ERROR;
	}

	loaded = true;

	mGraphicsManager.glErrorCheck();

	return STATUS_OK;
ERROR:
	Log::error("Error loading sprite batch");
	return STATUS_KO;
}

void SpriteBatch::draw()
{
	if (active)
	{
		glUseProgram(mShaderProgram);
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager.getMVPMatrix());
		glUniform1i(uTexture, 0);
		glEnableVertexAttribArray(aPosition);
		glVertexAttribPointer(aPosition, // Attribute Index
			3, // Size in bytes (x y & z)
			GL_FLOAT, // Data type
			GL_FALSE, // Normalized
			sizeof(Sprite::Vertex),// Stride
			&(mVertices[0].x)); // Location
		glEnableVertexAttribArray(aTexture);
		glVertexAttribPointer(aTexture, // Attribute Index
			2, // Size in bytes (u and v)
			GL_FLOAT, // Data type
			GL_FALSE, // Normalized
			sizeof(Sprite::Vertex), // Stride
			&(mVertices[0].u)); // Location

		const int32_t vertexPerSprite = 4;
		const int32_t indexPerSprite = 6;
		float timeStep = mTimeManager.elapsed();
		int32_t spriteCount = mSprites.size();
		int32_t currentSprite = 0, firstSprite = 0;
		while (bool canDraw = (currentSprite < spriteCount))
		{
			// Switches texture.
			Sprite* sprite = mSprites[currentSprite];
			GLuint currentTexture = sprite->mTexture;

			// Enable transparency
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sprite->mTexture);
			// Generate sprite vertices for current textures.
			do
			{
				// Set the scale
				glUniform1f(uTexSize, mTexSize[currentSprite]);
				sprite = mSprites[currentSprite];
				if (sprite->mTexture == currentTexture)
				{
					Sprite::Vertex* vertices = (&mVertices[currentSprite * 4]);
					sprite->draw(vertices, timeStep);
				}
				else
				{
					break;
				}
			} while (canDraw = (++currentSprite < spriteCount));
			glDrawElements(GL_TRIANGLES,
				// Number of indexes
				(currentSprite - firstSprite) * indexPerSprite,
				GL_UNSIGNED_SHORT, // Indexes data type
								   // First index
				&mIndexes[firstSprite * indexPerSprite]);
			firstSprite = currentSprite;
		}
		//When all sprites are rendered, restore the OpenGL state
		glUseProgram(0);
		glDisableVertexAttribArray(aPosition);
		glDisableVertexAttribArray(aTexture);
		glDisable(GL_BLEND);
	}
}