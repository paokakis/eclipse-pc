#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include "Log.hpp"
#include "glm.hpp"
#include "objloader.hpp"
#include "StandardShapes.h"

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

using namespace Assimp;

void SetupMapping(aiMaterial* mat, aiTextureMapping mode, const aiVector3D& axis = aiVector3D(0.f, 0.f, -1.f))
{
	// Check whether there are texture properties defined - setup
	// the desired texture mapping mode for all of them and ignore
	// all UV settings we might encounter. WE HAVE NO UVS!

	std::vector<aiMaterialProperty*> p;
	p.reserve(mat->mNumProperties + 1);

	for (unsigned int i = 0; i < mat->mNumProperties; ++i)
	{
		aiMaterialProperty* prop = mat->mProperties[i];
		if (!::strcmp(prop->mKey.data, "$tex.file")) {
			// Setup the mapping key
			aiMaterialProperty* m = new aiMaterialProperty();
			m->mKey.Set("$tex.mapping");
			m->mIndex = prop->mIndex;
			m->mSemantic = prop->mSemantic;
			m->mType = aiPTI_Integer;

			m->mDataLength = 4;
			m->mData = new char[4];
			*((int*)m->mData) = mode;

			p.push_back(prop);
			p.push_back(m);

			// Setup the mapping axis
			if (mode == aiTextureMapping_CYLINDER || mode == aiTextureMapping_PLANE || mode == aiTextureMapping_SPHERE) {
				m = new aiMaterialProperty();
				m->mKey.Set("$tex.mapaxis");
				m->mIndex = prop->mIndex;
				m->mSemantic = prop->mSemantic;
				m->mType = aiPTI_Float;

				m->mDataLength = 12;
				m->mData = new char[12];
				*((aiVector3D*)m->mData) = axis;
				p.push_back(m);
			}
		}
		else if (!::strcmp(prop->mKey.data, "$tex.uvwsrc")) {
			delete mat->mProperties[i];
		}
		else p.push_back(prop);
	}

	if (p.empty())return;

	// rebuild the output array
	if (p.size() > mat->mNumAllocated) {
		delete[] mat->mProperties;
		mat->mProperties = new aiMaterialProperty*[p.size() * 2];

		mat->mNumAllocated = p.size() * 2;
	}
	mat->mNumProperties = (unsigned int)p.size();
	::memcpy(mat->mProperties, &p[0], sizeof(void*)*mat->mNumProperties);
}

std::vector<Assimp3D> loadAssimpObj(const char * path)
{
	std::vector<Assimp3D> retObj;

	Assimp::Importer importer;
	std::vector<float> xValues;
	std::vector<float> yValues;
	std::vector<float> zValues;

	const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::error(importer.GetErrorString());
		return retObj;
	}

	// Fill vertices positions
	for (int j = 0; j < scene->mNumMeshes; ++j)
	{
		Assimp3D obj;

		obj.vertices.reserve(scene->mMeshes[j]->mNumVertices);
		obj.uvs.reserve(scene->mMeshes[j]->mNumVertices);
		obj.normals.reserve(scene->mMeshes[j]->mNumVertices);
		obj.tangents.reserve(scene->mMeshes[j]->mNumVertices);
		obj.bitangents.reserve(scene->mMeshes[j]->mNumVertices);
		for (unsigned int i = 0; i<scene->mMeshes[j]->mNumVertices; ++i) {
			aiVector3D pos = scene->mMeshes[j]->mVertices[i];
			obj.vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
			xValues.push_back(pos.x);
			yValues.push_back(pos.y);
			zValues.push_back(pos.z);
			// Fill vertices texture coordinates
			aiVector3D UVW = scene->mMeshes[j]->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
			obj.uvs.push_back(glm::vec2(UVW.x, UVW.y));
			// Fill vertices normals
			aiVector3D n = scene->mMeshes[j]->mNormals[i];
			obj.normals.push_back(glm::vec3(n.x, n.y, n.z));
			// Fill tangents
			obj.tangents.push_back(glm::normalize(glm::vec3(scene->mMeshes[j]->mTangents[i].x, scene->mMeshes[j]->mTangents[i].y, scene->mMeshes[j]->mTangents[i].z)));
			// Fill bitangents
			obj.bitangents.push_back(glm::normalize(glm::vec3(scene->mMeshes[j]->mBitangents[i].x, scene->mMeshes[j]->mBitangents[i].y, scene->mMeshes[j]->mBitangents[i].z)));
		}

		// Fill face indices
		obj.indices.reserve(3 * scene->mMeshes[j]->mNumFaces);
		for (unsigned int i = 0; i<scene->mMeshes[j]->mNumFaces; ++i) {
			// Assume the model has only triangles.
			obj.indices.push_back(scene->mMeshes[j]->mFaces[i].mIndices[0]);
			obj.indices.push_back(scene->mMeshes[j]->mFaces[i].mIndices[1]);
			obj.indices.push_back(scene->mMeshes[j]->mFaces[i].mIndices[2]);
		}

		std::sort(xValues.begin(), xValues.end());
		std::sort(yValues.begin(), yValues.end());
		std::sort(zValues.begin(), zValues.end());

		obj.width = abs(xValues.front()) + abs(xValues.back());
		obj.height = abs(yValues.front()) + abs(yValues.back());
		obj.depth = abs(zValues.front()) + abs(zValues.back());
		obj.objType = OBJECT_MESH;

		retObj.push_back(obj);
	}	

	// The "scene" pointer will be deleted automatically by "importer"

	return retObj;
}

std::vector<Assimp3D> loadAssimpCone(float height, float radius1, float radius2, unsigned int tess, uint16_t num_of_objs, bool bOpen)
{
	std::vector<Assimp3D> retObj;
	std::vector<aiVector3D> vertices;

	// generate the cone - it consists of simple triangles
	StandardShapes::MakeCone(height, radius1, radius2, integer_pow(4, tess), vertices);
	aiMesh* mesh = StandardShapes::MakeMesh(vertices, 3);
	
	Assimp3D obj;
	std::vector<float> xValues;
	std::vector<float> yValues;
	std::vector<float> zValues;

	
	obj.vertices.reserve(mesh->mNumVertices);
	obj.uvs.reserve(mesh->mNumVertices);
	obj.normals.reserve(mesh->mNumVertices);
	obj.tangents.reserve(mesh->mNumVertices);
	obj.bitangents.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i<mesh->mNumVertices; ++i) {
		aiVector3D pos = mesh->mVertices[i];
		obj.vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
		xValues.push_back(pos.x);
		yValues.push_back(pos.y);
		zValues.push_back(pos.z);
		// Fill vertices texture coordinates
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		obj.uvs.push_back(glm::vec2(UVW.x, UVW.y));
		// Fill vertices normals
		aiVector3D n = mesh->mNormals[i];
		obj.normals.push_back(glm::vec3(n.x, n.y, n.z));
		// Fill tangents
		obj.tangents.push_back(glm::normalize(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)));
		// Fill bitangents
		obj.bitangents.push_back(glm::normalize(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z)));
	}

	// Fill face indices
	obj.indices.reserve(3 * mesh->mNumFaces);
	for (unsigned int i = 0; i<mesh->mNumFaces; ++i) {
		// Assume the model has only triangles.
		obj.indices.push_back(mesh->mFaces[i].mIndices[0]);
		obj.indices.push_back(mesh->mFaces[i].mIndices[1]);
		obj.indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	std::sort(xValues.begin(), xValues.end());
	std::sort(yValues.begin(), yValues.end());
	std::sort(zValues.begin(), zValues.end());

	obj.width = abs(xValues.front()) + abs(xValues.back());
	obj.height = abs(yValues.front()) + abs(yValues.back());
	obj.depth = abs(zValues.front()) + abs(zValues.back());
	obj.objType = OBJECT_CONE;

	for (uint16_t i = 0; i < num_of_objs; ++i)
	{
		retObj.push_back(obj);
	}

	return retObj;
}

//std::vector<Assimp3D> loadAssimpSphere(unsigned int	tess)
//{
//
//}

std::vector<Assimp3D> loadAssimpCube()
{
	std::vector<Assimp3D> retObj;
	std::vector<aiVector3D> vertices;

	// generate the cone - it consists of simple triangles
	StandardShapes::MakeHexahedron(vertices);
	aiMesh* mesh = StandardShapes::MakeMesh(vertices, 3);

	Assimp3D obj;
	std::vector<float> xValues;
	std::vector<float> yValues;
	std::vector<float> zValues;


	obj.vertices.reserve(mesh->mNumVertices);
	obj.uvs.reserve(mesh->mNumVertices);
	obj.normals.reserve(mesh->mNumVertices);
	obj.tangents.reserve(mesh->mNumVertices);
	obj.bitangents.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i<mesh->mNumVertices; ++i) {
		aiVector3D pos = mesh->mVertices[i];
		obj.vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
		xValues.push_back(pos.x);
		yValues.push_back(pos.y);
		zValues.push_back(pos.z);
		// Fill vertices texture coordinates
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		obj.uvs.push_back(glm::vec2(UVW.x, UVW.y));
		// Fill vertices normals
		aiVector3D n = mesh->mNormals[i];
		obj.normals.push_back(glm::vec3(n.x, n.y, n.z));
		// Fill tangents
		obj.tangents.push_back(glm::normalize(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)));
		// Fill bitangents
		obj.bitangents.push_back(glm::normalize(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z)));
	}

	// Fill face indices
	obj.indices.reserve(3 * mesh->mNumFaces);
	for (unsigned int i = 0; i<mesh->mNumFaces; ++i) {
		// Assume the model has only triangles.
		obj.indices.push_back(mesh->mFaces[i].mIndices[0]);
		obj.indices.push_back(mesh->mFaces[i].mIndices[1]);
		obj.indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	std::sort(xValues.begin(), xValues.end());
	std::sort(yValues.begin(), yValues.end());
	std::sort(zValues.begin(), zValues.end());

	obj.width = abs(xValues.front()) + abs(xValues.back());
	obj.height = abs(yValues.front()) + abs(yValues.back());
	obj.depth = abs(zValues.front()) + abs(zValues.back());
	obj.objType = OBJECT_CUBE;

	return retObj;
}

#endif