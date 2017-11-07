#ifndef OBJLOADER_H
#define OBJLOADER_H

typedef enum {
	OBJECT_CUBE,
	OBJECT_CONE,
	OBJECT_SPHERE,
	OBJECT_MESH
} Obj_type;

struct Assimp3D
{
	std::vector<unsigned int> indices;		// indices
	std::vector<glm::vec3> vertices;		// vertices
	std::vector<glm::vec2> uvs;				// texture coords
	std::vector<glm::vec3> normals;			// normals
	std::vector<glm::vec3> tangents;		// tangents
	std::vector<glm::vec3> bitangents;		// bit tangents
	struct TextureProperties* difuseTextureProps;
	struct TextureProperties* normalTextureProps;
	float width;
	float height;
	float depth;
	Obj_type objType;
};

std::vector<Assimp3D> loadAssimpObj(const char * path);

std::vector<Assimp3D> loadAssimpCone(float height, float radius1, float radius2, unsigned int tess, uint16_t num_of_objs, bool bOpen = false);
//std::vector<Assimp3D> loadAssimpSphere(unsigned int	tess);
std::vector<Assimp3D> loadAssimpCube();

#endif