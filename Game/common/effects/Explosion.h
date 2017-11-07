#ifndef EXPLOSION_H
#define EXPLOSION_H
#include "../../../Engine/Graphics/GraphicsManager.hpp"
#include "../../../Engine/TimeManager.hpp"
#include "../../../sources/glm/glm.hpp"
#include <vector>

class Explosion : public GraphicsComponent
{
public:
	Explosion(GraphicsManager&, TimeManager&, float size);
	~Explosion();

	void start(glm::vec3, float);
	bool finishedAnimating();

protected:
	status load();
	void update();
	void draw();

private:
	struct Vertex {
		GLfloat x, y, z;
	};
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;

	GLuint mShaderProgram; 
	GLuint aPosition; GLuint mVertexBuffer;
	GLuint u_time; GLuint u_centerPosition; GLuint u_color; GLuint s_texture; 
	GLuint uProjection; GLuint uSize; GLuint u_lifetime;

	std::vector<std::string> mTextureResource;
	std::vector<TextureProperties*> mTextureProperties;

	GLfloat mTime;
	GLfloat mLifeTime;
	GLfloat mSize;
	int mNumParticles;
	bool mStart;

	glm::vec3 mCenterPos;
	glm::vec4 mColor;

	uint16_t mCurrentTex;
	float mUpdateTimeTex;
	float mTimeTex;
};

#endif // EXPLOSION_H