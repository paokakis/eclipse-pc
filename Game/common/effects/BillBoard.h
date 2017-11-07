#ifndef BILL_BOARD_H
#define BILL_BOARD_H
#include "../../../Engine/Graphics/GraphicsManager.hpp"
#include "../../../sources/glm/glm.hpp"
#include <vector>

class BillBoard : public GraphicsComponent
{
public:
	BillBoard(GraphicsManager&, float size, glm::vec3& pos);
	~BillBoard();

	void setLevel(float lvl) { LifeLevel = lvl / 100; }
	
protected:
	status load();
	void update();
	void draw();

private:
	struct Vertex {
		GLfloat x, y, z;
	};
	GraphicsManager& mGraphicsManager;

	GLuint mShaderProgram;
	GLuint aPosition; GLuint mVertexBuffer; GLuint LifeLevelID;
	GLuint u_time; GLuint u_centerPosition; GLuint u_color; GLuint s_texture;
	GLuint uProjection; GLuint uSize; GLuint u_lifetime;

	std::vector<std::string> mTextureResource;
	std::vector<TextureProperties*> mTextureProperties;

	float mSize;
	float LifeLevel = 0.0f;
	int mNumParticles;

	glm::vec3& mCenterPos;
};

#endif // !BILL_BOARD_H
