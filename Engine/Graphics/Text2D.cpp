#include "Text2D.h"
#include "../Tools/Log.hpp"
#include "../Resource.hpp"
#include <freetype\ftglyph.h>
#include "../Engine/Graphics/Configuration/Folders.h"

static const char* VERTEX_SHADER = R"glsl(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;
uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
})glsl";

static const char* FRAGMENT_SHADER = R"glsl(
#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	color = vec4(textColor, 1.0) * sampled;
} )glsl";

Text2D::Text2D(GraphicsManager& pGraphicsManager) :
	mGraphicsManager(pGraphicsManager),
	mpLibrary(NULL), mpFace(NULL)
{
	Log::info("Text constructor");
}

Text2D::~Text2D()
{
	Log::info("Text destructor");
	for (size_t i = 0; i < renderTexturesVec.size(); ++i)
	{
		glDeleteTextures(1, &renderTexturesVec[i]->texture);
		delete renderTexturesVec[i];
	}
	renderTexturesVec.clear();
}

status Text2D::setFont(const char* path)
{
	if (FT_New_Face(mpLibrary, path, 0, &mpFace))
	{
		Log::error("FREETYPE: Failed to load font");

		return STATUS_KO;
	}
		
	return STATUS_OK;
}

status Text2D::initialize(int fontSize)
{
	if (!initialized)
	{
		if (FT_Init_FreeType(&mpLibrary))
		{
			Log::error("FREETYPE: Could not init FreeType Library");
			return STATUS_KO;
		}

		if (FT_New_Face(mpLibrary, DEFAULT_FONT_PATH, 0, &mpFace))
		{
			Log::error("FREETYPE: Failed to load font");
			return STATUS_KO;
		}

		FT_Set_Pixel_Sizes(mpFace, 0, fontSize);

		if (FT_Load_Char(mpFace, 'X', FT_LOAD_RENDER))
		{
			Log::error("FREETYTPE: Failed to load Glyph");
			return STATUS_KO;
		}

		textShaderProg = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph
			if (FT_Load_Char(mpFace, c, FT_LOAD_RENDER))
			{
				Log::error("ERROR::FREETYTPE: Failed to load Glyph");
				continue;
			}
			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				mpFace->glyph->bitmap.width,
				mpFace->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				mpFace->glyph->bitmap.buffer
			);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			Character character = {
				texture,
				glm::ivec2(mpFace->glyph->bitmap.width, mpFace->glyph->bitmap.rows),
				glm::ivec2(mpFace->glyph->bitmap_left, mpFace->glyph->bitmap_top),
				mpFace->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		FT_Done_Face(mpFace);
		FT_Done_FreeType(mpLibrary);

		initialized = true;
		mActive = true;
	}
	
	return STATUS_OK;
}

void Text2D::RenderText(std::string text, glm::vec2 pos, GLfloat scale, glm::vec3 color)
{
	if (mActive)
	{
		// Activate corresponding render state
		glUseProgram(textShaderProg);
		glEnable(GL_BLEND);
		glUniformMatrix4fv(glGetUniformLocation(textShaderProg, "projection"), 1, GL_FALSE, &mGraphicsManager.getOrthoMVP()[0][0]);
		glUniform3f(glGetUniformLocation(textShaderProg, "textColor"), color.r, color.g, color.b);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
		// Iterate through all characters
		std::string::const_iterator c;
		GLfloat currLine = 0.0;
		GLfloat startOfLine = (pos.x - (strlen(text.c_str()) * (20)));
		GLfloat prevCharPos = startOfLine;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];
			
			if (*c == '\n')
			{
				currLine -= (ch.Bearing.y + 10) * scale;
				prevCharPos = startOfLine;

				continue;
			}
			GLfloat xpos = (prevCharPos + ch.Bearing.x) * scale;
			prevCharPos += ch.Size.x + 2;
			GLfloat ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale + currLine;
			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos, ypos, 0.0, 1.0 },
				{ xpos + w, ypos, 1.0, 1.0 },
				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos + w, ypos, 1.0, 1.0 },
				{ xpos + w, ypos + h, 1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1 / 64 pixels)
			pos.x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels(2 ^ 6 = 64)
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
	}	
}

TextureProperties* Text2D::drawOnTexture(std::string text, glm::vec2 pos, glm::vec4& color, glm::vec4& backColor, int width, int height)
{
	Log::info("Rendering text to texture...");

	GLuint FBO, depthBuff;
	TextureProperties* textProp = new TextureProperties;

	glGenTextures(1, &textProp->texture);
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &depthBuff);

	if (textProp == NULL || textProp->texture == NULL || FBO == NULL || depthBuff == NULL) goto ERROR;

	glBindTexture(GL_TEXTURE_2D, textProp->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

	/*Attach the previous texture to it with glBindFramebuffer().*/
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textProp->texture, 0);

	// depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuff);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, width, height);
	glClearColor(backColor.r, backColor.g, backColor.b, backColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderText(text, pos, 1.f, color);

	glViewport(0, 0, mGraphicsManager.getRenderWidth(), mGraphicsManager.getRenderHeight());
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &depthBuff);

	textProp->width = width;
	textProp->height = height;
	textProp->textureResource = NULL;

	renderTexturesVec.push_back(textProp);

	return renderTexturesVec.back();

ERROR:
	Log::error("Error rendering text to texture");

	glDeleteTextures(1, &textProp->texture);
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &depthBuff);
	delete textProp;

	return NULL;
}