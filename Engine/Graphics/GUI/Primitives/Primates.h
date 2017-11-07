#pragma once
#include "../../GraphicsManager.hpp"
#include <vector>

typedef enum {
	SHP_POINT,
	SHP_LINE,
	SHP_LINE_STRIP,
	SHP_QUAD,
} draw_type_t;

typedef struct Shape{
	Shape(glm::vec3& beg, glm::vec3& en, glm::vec3& col, draw_type_t type)
	{
		begin = beg;
		end = en;
		color = col;
		shapeType = type;
	}
	glm::vec3 begin;
	glm::vec3 end;
	glm::vec3 color;
	draw_type_t shapeType;
} shape_properties_t;

class Primates : public GraphicsComponent
{
public:
	Primates(GraphicsManager& graphMan);

	void addLine(glm::vec3& from, glm::vec3& to, glm::vec3& color);

protected:
	status load();
	void update() {}
	void draw();

private:
	GraphicsManager& mGraphicsManager;

	GLuint aPosition; GLuint uProjection; GLuint uColor;
	GLuint mShaderProgram; GLuint lineVBO;

	std::vector<shape_properties_t> shapes;
	float lineCoords[6];

	const int COORDS_PER_VERTEX = 3;
	const int VertexStride = COORDS_PER_VERTEX * 4;
};