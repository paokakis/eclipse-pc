#ifndef SKY_H
#define SKY_H
#include "../../../Engine/Graphics/GraphicsManager.hpp"
#include "../../../Engine/TimeManager.hpp"
#include "../../../sources/glm/glm.hpp"
#include <vector>

typedef enum {
	BACKGROUND_SKY,
	BACKGROUND_STARS,
	BACKGROUND_OBJECT,
	BACKGROUND_LINES,
	BACKGROUND_STARFIELD
} eBGtype_t;

class Sky
{
public:
	Sky(GraphicsManager&, TimeManager&, eBGtype_t);
	~Sky();

	status load();
	void update();
	void draw();

	void setPos(const glm::vec3& pos) { mPos = pos; }
	void setFlightSpeed(const GLfloat spd) { mFlightSpeed = spd; }

private:
	struct Vertex {
		GLfloat x, y, z;
	};
	void prepVrtx(Vertex pVertices[4]);

	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;

	GLuint mShaderProgram;
	GLuint aPosition; GLuint u_pos; GLuint u_flightSpeed;
	GLuint u_time; GLuint u_resolution; GLuint uProjection;
	glm::mat4 mProjection;

	GLfloat mTime;
	GLfloat mFlightSpeed = 0.f;
	glm::vec2 mResolution;
	glm::vec3 mPos;
	eBGtype_t mType;

	std::vector<Vertex> mVertices;
	std::vector<GLushort> mIndexes;
};
#endif // SKY_H