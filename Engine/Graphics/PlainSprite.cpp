#include "PlainSprite.h"
#include "../Tools/Log.hpp"

static const char* VERTEX_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"attribute vec4 aPosition;\n"					// Per-vertex position information we will pass in.
"uniform vec4 uColor;\n"						// Per-vertex color information we will pass in.
"attribute vec2 aTexture;\n"
"uniform vec3 uScale;\n"
"varying vec2 vTexture;\n"
"varying vec4 vColor;\n"						// This will be passed into the fragment shader.
"uniform mat4 uProjection;\n"					// A constant representing the combined model/view/projection matrix.
"void main() {\n"
" mat4 scale = mat4(\n"							// scale set accodring to depth
" vec4(uScale.x, 0.0, 0.0, 0.0),\n"
" vec4(0.0, uScale.y, 0.0, 0.0),\n"
" vec4(0.0, 0.0, 1.0, 0.0),\n"
" vec4(0.0, 0.0, 0.0, 1.0));\n"
" vTexture = aTexture;\n"
" vColor = uColor;\n"
" gl_Position = uProjection * aPosition * scale;\n"		// gl_Position is a special variable used to store the final position.Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
"}";

static const char* FRAGMENT_SHADER =
"#version 330 core\n"
"precision highp float;\n"
"varying vec2 vTexture;\n"
"varying vec4 vColor;\n"
"uniform sampler2D u_texture;\n"
"void main() {\n"
" vec4 basecolor = texture2D(u_texture, vTexture) * vColor;\n"
" if(basecolor.a == 0.0) { discard; }\n"
" gl_FragColor = basecolor;\n"
"}";

static bool loaded = false;

static void sortByZ(std::vector<sPlSpriteOpt_t>& pOpts)
{
	for (size_t i = 0; i < pOpts.size() - 1; ++i)
	{
		for (size_t j = i + 1; j < pOpts.size(); ++j)
		{
			if (pOpts[i].pos->z > pOpts[j].pos->z)
			{
				std::swap(pOpts[i], pOpts[j]);
			}
		}
	}
}

PlainSprite::PlainSprite(GraphicsManager& graphMan, TimeManager& timeMan) :
	mGraphicsManager(graphMan), mTimeManager(timeMan),
	mVertices(), mIndexes(), mAnimationKey(0), mSpriteKey(0), mAnimationOptsMap(), mTimeNow(0),
	mShaderProgram(0), aPosition(0), aTexture(0), uProjection(0), uTexture(0), uColor(0), active(false)
{
	mGraphicsManager.registerComponent(this, 111111);
}

void PlainSprite::initialize()
{
	if (!loaded) load(); 
}

PlainSprite::~PlainSprite()
{
	Log::info("PlainSprite destructor");
	loaded = false;

	mVertices.clear();
	mIndexes.clear();
	mAnimationVertices.clear();
	mAnimationIndexes.clear();

	for (size_t i = 0; i < mSingleFrameOpts.size(); ++i)
	{
		delete(mSingleFrameOpts[i]);
	}
	mSingleFrameOpts.clear();
	for (size_t i = 0; i < mAnimationOptsMap.size(); ++i)
	{
		delete(mAnimationOptsMap[i]);
	}
	mAnimationOptsMap.clear();
}

status PlainSprite::load()
{
	Log::info("PlainSprite load");

	//// Load texture resources
	//std::map<int32_t, sPlSpriteOpt_t*>::iterator iter;
	//std::map<int32_t, sAnimOptions_t*>::iterator mIter;
	//for (iter = mSingleFrameOpts.begin(); iter != mSingleFrameOpts.end(); ++iter)
	//{
	//	iter->second->mpTexture = (mGraphicsManager.loadTextureDiffuse(iter->second->imagePath));
	//	if (iter->second->mpTexture == 0) goto ERROR;

	//	mIndexes.push_back(0); mIndexes.push_back(1);
	//	mIndexes.push_back(2); mIndexes.push_back(2);
	//	mIndexes.push_back(1); mIndexes.push_back(3);

	//	for (int j = 0; j < 4; ++j)
	//	{
	//		mVertices.push_back(Vertex());
	//	}
	//	Log::info("Loaded texture with id %u", iter->second->mpTexture->texture);
	//}
	//for (mIter = mAnimationOptsMap.begin(); mIter != mAnimationOptsMap.end(); ++mIter)
	//{
	//	for (size_t j = 0; j < mIter->second->vecAnimArray.size(); ++j)
	//	{
	//		mIter->second->vecAnimArray[j]->pTextureProperties = (mGraphicsManager.loadTextureDiffuse(mIter->second->vecAnimArray[j]->imagePath));
	//		if (mIter->second->vecAnimArray[j]->pTextureProperties == 0) goto ERROR;

	//		mAnimationIndexes.push_back(0); mAnimationIndexes.push_back(1);
	//		mAnimationIndexes.push_back(2); mAnimationIndexes.push_back(2);
	//		mAnimationIndexes.push_back(1); mAnimationIndexes.push_back(3);

	//		for (size_t j = 0; j < 4; ++j)
	//		{
	//			mAnimationVertices.push_back(Vertex());
	//		}
	//		Log::info("Loaded animation texture with id %u", mIter->second->vecAnimArray[j]->pTextureProperties->texture);
	//	}
	//	mIter->second->prevTime = 0.f;//mTimeManager.elapsedTotal();
	//}
	
	// Load shaders
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0) goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	aTexture = glGetAttribLocation(mShaderProgram, "aTexture");
	uColor = glGetUniformLocation(mShaderProgram, "uColor");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uTexture = glGetUniformLocation(mShaderProgram, "u_texture");
	uScale = glGetUniformLocation(mShaderProgram, "uScale");

	loaded = true;

	//mGraphicsManager.glErrorCheck();

	Log::info("Loaded shader program %u", mShaderProgram);

	return STATUS_OK;
ERROR:
	Log::error("Error loading texture & shaders.");
	return STATUS_KO;
}

void PlainSprite::loadPlainSpr()
{

}

void PlainSprite::update()
{
	if (active)
	{
		mTimeNow += mTimeManager.elapsed();
		std::map<int32_t, sAnimOptions_t*>::iterator mIter;
		for (mIter = mAnimationOptsMap.begin(); mIter != mAnimationOptsMap.end(); ++mIter)
		{
			if (!mIter->second->paused)
			{
				if (mTimeNow - mIter->second->prevTime >= mIter->second->updateTime)
				{
					if (mIter->second->activeFrame == (int)(mIter->second->vecAnimArray.size() - 1))
					{
						mIter->second->activeFrame = 0;
						if (!mIter->second->loop)
						{
							mIter->second->stopped = true;
						}
					}
					else
					{
						mIter->second->activeFrame++;
					}
					mIter->second->prevTime = mTimeNow;
				}
			}
		}
	}
}

int32_t PlainSprite::registerSprite(sPlSpriteOpt_t& pOpts)
{
	//if (1) // !loaded
	{
		mSingleFrameOpts.insert(std::make_pair(mSpriteKey++, new sPlSpriteOpt_t(pOpts)));
		//loaded = false;

		mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->mpTexture = (mGraphicsManager.loadTextureDiffuse(pOpts.imagePath));
		if (mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->mpTexture == 0) return -1;

		mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->width = mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->mpTexture->width;
		mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->height = mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->mpTexture->height;
		pOpts.width = mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->width;
		pOpts.height = mSingleFrameOpts.at(mSingleFrameOpts.size() - 1)->height;

		mIndexes.push_back(0); mIndexes.push_back(1);
		mIndexes.push_back(2); mIndexes.push_back(2);
		mIndexes.push_back(1); mIndexes.push_back(3);

		for (int j = 0; j < 4; ++j)
		{
			mVertices.push_back(Vertex());
		}

		return (mSpriteKey - 1);
	}

	//return -1;
}

void PlainSprite::removeSprite(int32_t sprId)
{
	delete mSingleFrameOpts.at(sprId);
	mSingleFrameOpts.erase(sprId);
}

int32_t PlainSprite::registerAnimation(const sAnimOptions_t& animOpts)
{
	//if (1) // !loaded
	{
		mAnimationOptsMap.insert(std::make_pair(mAnimationKey++, new sAnimOptions_t(animOpts)));
		//loaded = false;

		mAnimationOptsMap.at(mAnimationOptsMap.size() - 1)->vecAnimArray[0]->pTextureProperties = mGraphicsManager.loadTextureDiffuse(mAnimationOptsMap.at(mAnimationOptsMap.size() - 1)->vecAnimArray[0]->imagePath);
		if (mAnimationOptsMap.at(mAnimationOptsMap.size() - 1)->vecAnimArray[0]->pTextureProperties == 0) return -1;

		mAnimationIndexes.push_back(0); mAnimationIndexes.push_back(1);
		mAnimationIndexes.push_back(2); mAnimationIndexes.push_back(2);
		mAnimationIndexes.push_back(1); mAnimationIndexes.push_back(3);

		for (size_t j = 0; j < 4; ++j)
		{
			mAnimationVertices.push_back(Vertex());
		}

		return (mAnimationKey - 1);
	}

	//return -1;
}

void PlainSprite::removeAnimation(int32_t animId)
{
	delete mAnimationOptsMap.at(animId);
	mAnimationOptsMap.erase(animId);
}

status PlainSprite::setSpriteProperties(int32_t key, sPlSpriteOpt_t opts)
{
	if (mSingleFrameOpts.find(key) == mSingleFrameOpts.end()) return STATUS_KO;

	mSingleFrameOpts[key]->active = opts.active;
	mSingleFrameOpts[key]->imagePath = opts.imagePath;
	mSingleFrameOpts[key]->pos = opts.pos;

	return STATUS_OK;
}

status PlainSprite::setAnimationProperties(int32_t key, sAnimOptions_t opts)
{
	if (mAnimationOptsMap.find(key) == mAnimationOptsMap.end()) return STATUS_KO;

	mAnimationOptsMap[key] = new sAnimOptions_t(opts);

	return STATUS_OK;
}

void PlainSprite::draw()
{
	if (active)
	{
		glUseProgram(mShaderProgram);
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &mGraphicsManager.getOrthoMVP()[0][0]);
		glUniform1i(uTexture, 0);

		glDisable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		std::map<int32_t, sPlSpriteOpt_t*>::iterator iter;
		int singleCurrent = 0;
		for (iter = mSingleFrameOpts.begin(); iter != mSingleFrameOpts.end(); ++iter)
		{
			if (iter->second->active)
			{
				Vertex* vertices = (&mVertices[singleCurrent * 4]);
				prepSingleVrtx(vertices, singleCurrent);

				// pass color data
				glUniform4fv(uColor, 1, &iter->second->colorBuffer.r);

				// pass xyz vertex data
				glEnableVertexAttribArray(aPosition);
				glVertexAttribPointer(aPosition, // Attribute Index
					3, // Size in bytes (x y and z)
					GL_FLOAT, // Data type
					GL_FALSE, // Normalized
					sizeof(Vertex),// Stride
					&(vertices[0].x)); // Location

				// pass u v texture data
				glEnableVertexAttribArray(aTexture);
				glVertexAttribPointer(aTexture, // Attribute Index
					2, // Size in bytes (u and v)
					GL_FLOAT, // Data type
					GL_FALSE, // Normalized
					sizeof(Vertex), // Stride
					&(vertices[0].u)); // Location

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, iter->second->mpTexture->texture);

				glDrawElements(GL_TRIANGLES,
					// Number of indexes
					6,
					// Indexes data type
					GL_UNSIGNED_SHORT,
					// First index
					&mIndexes[singleCurrent * 6]);
			}
			singleCurrent++;
		}

		std::map<int32_t, sAnimOptions_t*>::iterator mIter;
		int animCurrent = 0;
		for (mIter = mAnimationOptsMap.begin(); mIter != mAnimationOptsMap.end(); ++mIter)
		{
			if (!mIter->second->stopped)
			{
				Vertex* vertices = (&mAnimationVertices[animCurrent * 4]);
				prepAnimVrtx(vertices, animCurrent);

				// pass color data
				glUniform4fv(uColor, 1, &mIter->second->colorBuffer.r);

				// pass xyz vertex data
				glEnableVertexAttribArray(aPosition);
				glVertexAttribPointer(aPosition, // Attribute Index
					3, // Size in bytes (x y and z)
					GL_FLOAT, // Data type
					GL_FALSE, // Normalized
					sizeof(Vertex),// Stride
					&(vertices[0].x)); // Location

				// pass u v texture data
				glEnableVertexAttribArray(aTexture);
				glVertexAttribPointer(aTexture, // Attribute Index
					2, // Size in bytes (u and v)
					GL_FLOAT, // Data type
					GL_FALSE, // Normalized
					sizeof(Vertex), // Stride
					&(vertices[0].u)); // Location

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mIter->second->vecAnimArray[mIter->second->activeFrame]->pTextureProperties->texture);

				glDrawElements(GL_TRIANGLES,
					// Number of indexes
					6,
					// Indexes data type
					GL_UNSIGNED_SHORT,
					// First index
					&mAnimationIndexes[animCurrent * 6]);
			}
			animCurrent++;
		}

		//When all sprites are rendered, restore the OpenGL state
		glUseProgram(0);
		glDisableVertexAttribArray(aPosition);
		glDisableVertexAttribArray(aTexture);
		glDisable(GL_BLEND);
	}	
}

void PlainSprite::prepAnimVrtx(Vertex pVertices[4], int current)
{
	// Draws selected frame.
	GLfloat posX1 = mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->p_pos->x - float(mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->pTextureProperties->width / 2) + mAnimationOptsMap[current]->movePos.x;
	GLfloat posX2 = posX1 + mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->pTextureProperties->width;
	GLfloat posY1 = mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->p_pos->y - float(mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->pTextureProperties->height / 2) + mAnimationOptsMap[current]->movePos.y;
	GLfloat posY2 = posY1 + mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->pTextureProperties->height;
	GLfloat posZ1 = mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->p_pos->z + mAnimationOptsMap[current]->movePos.z;
	GLfloat posZ2 = mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->p_pos->z + mAnimationOptsMap[current]->movePos.z;
	GLfloat u1 = 1;
	GLfloat u2 = 0;
	GLfloat v1 = 1;
	GLfloat v2 = 0;
	pVertices[0].x = posX1; pVertices[0].y = posY1; pVertices[0].z = posZ1;
	pVertices[0].u = u2; pVertices[0].v = v1;
	pVertices[1].x = posX1; pVertices[1].y = posY2; pVertices[1].z = posZ2;
	pVertices[1].u = u2; pVertices[1].v = v2;
	pVertices[2].x = posX2; pVertices[2].y = posY1; pVertices[2].z = posZ1;
	pVertices[2].u = u1; pVertices[2].v = v1;
	pVertices[3].x = posX2; pVertices[3].y = posY2; pVertices[3].z = posZ2;
	pVertices[3].u = u1; pVertices[3].v = v2;

	glUniform3fv(uScale, 1, &mAnimationOptsMap[current]->vecAnimArray[mAnimationOptsMap[current]->activeFrame]->scale.x);
}

void PlainSprite::prepSingleVrtx(Vertex pVertices[4], int current)
{
	// Draws selected frame.
	GLfloat posX1 = mSingleFrameOpts[current]->pos->x - float(mSingleFrameOpts[current]->mpTexture->width / 2);
	GLfloat posY1 = mSingleFrameOpts[current]->pos->y - float(mSingleFrameOpts[current]->mpTexture->height / 2);
	GLfloat posX2 = posX1 + mSingleFrameOpts[current]->mpTexture->width;
	GLfloat posY2 = posY1 + mSingleFrameOpts[current]->mpTexture->height;
	GLfloat posZ1 = mSingleFrameOpts[current]->pos->z;
	GLfloat posZ2 = mSingleFrameOpts[current]->pos->z;
	GLfloat u1 = 1;
	GLfloat u2 = 0;
	GLfloat v1 = 1;
	GLfloat v2 = 0;
	pVertices[0].x = posX1; pVertices[0].y = posY1; pVertices[0].z = posZ1;
	pVertices[0].u = u2; pVertices[0].v = v1;
	pVertices[1].x = posX1; pVertices[1].y = posY2; pVertices[1].z = posZ2;
	pVertices[1].u = u2; pVertices[1].v = v2;
	pVertices[2].x = posX2; pVertices[2].y = posY1; pVertices[2].z = posZ1;
	pVertices[2].u = u1; pVertices[2].v = v1;
	pVertices[3].x = posX2; pVertices[3].y = posY2; pVertices[3].z = posZ2;
	pVertices[3].u = u1; pVertices[3].v = v2;

	glUniform3fv(uScale, 1, &mSingleFrameOpts[current]->scale.x);
}