#ifndef TEXT2D_H
#define TEXT2D_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../Tools/Types.hpp"
#include "GraphicsManager.hpp"
#include "../../sources/glm/glm.hpp"
#include <vector>

class Text2D
{
public:
	Text2D(GraphicsManager&);
	~Text2D();

	status initialize(int fontSize);
	status setFont(const char* text);
	void RenderText(std::string text, glm::vec2 pos, GLfloat scale, glm::vec3 color);
	TextureProperties* drawOnTexture(std::string text, glm::vec2 pos, glm::vec4& color, glm::vec4& backColor, int width, int height);

private:
	bool initialized = false;

	GraphicsManager& mGraphicsManager;
	FT_Library  mpLibrary;
	FT_Face     mpFace;

	GLuint VAO, VBO, textShaderProg;
	struct Character {
		GLuint TextureID; // ID handle of the glyph texture
		glm::ivec2 Size; // Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance; // Offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;
	std::vector<TextureProperties*> renderTexturesVec;

	bool mActive = false;
};

#endif