#include <assimp/aabb.h> 
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string.h>
#include "obj_loader.h"

void write_to_meshes(struct aiMesh *in_mesh, kie_Object *out_obj)
{
	out_obj->vertices_count = in_mesh->mNumVertices;
	out_obj->indices_count = in_mesh->mFaces->mNumIndices * in_mesh->mNumFaces;
	out_obj->indices = (unsigned int *) ksai_Arena_allocate(sizeof(uint32_t) * out_obj->indices_count, &global_object_arena);
	for (int i = 0; i < in_mesh->mNumFaces; i++)
	{
		for (int j = 0; j < in_mesh->mFaces[i].mNumIndices; j++)
		{
			int position = i * in_mesh->mFaces[i].mNumIndices + j;
			out_obj->indices[position] = in_mesh->mFaces[i].mIndices[j];
		}
	}

	out_obj->vertices = (kie_Vertex *) ksai_Arena_allocate(sizeof(kie_Vertex) * out_obj->vertices_count, &global_object_arena);
	for (int i = 0; i < out_obj->vertices_count; i++)
	{
		out_obj->vertices[i].position[0] = in_mesh->mVertices[i].x;
		out_obj->vertices[i].position[1] = in_mesh->mVertices[i].y;
		out_obj->vertices[i].position[2] = in_mesh->mVertices[i].z;

		out_obj->vertices[i].normal[0] = in_mesh->mNormals[i].x;
		out_obj->vertices[i].normal[1] = in_mesh->mNormals[i].y;
		out_obj->vertices[i].normal[2] = in_mesh->mNormals[i].z;

		out_obj->vertices[i].tangent[0] = in_mesh->mTangents[i].x;
		out_obj->vertices[i].tangent[1] = in_mesh->mTangents[i].y;
		out_obj->vertices[i].tangent[2] = in_mesh->mTangents[i].z;

		out_obj->vertices[i].bit_tangent[0] = in_mesh->mBitangents[i].x;
		out_obj->vertices[i].bit_tangent[1] = in_mesh->mBitangents[i].y;
		out_obj->vertices[i].bit_tangent[2] = in_mesh->mBitangents[i].z;

		out_obj->vertices[i].tex_coord[0] = in_mesh->mTextureCoords[0][i].x;
		out_obj->vertices[i].tex_coord[1] = in_mesh->mTextureCoords[0][i].y;
	}
}

void read_obj_to_kie_Object(const char *path, kie_Object *out_obj)
{
	const struct aiScene *scene = aiImportFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		__debugbreak();
	}

	write_to_meshes(scene->mMeshes[scene->mRootNode->mChildren[0]->mMeshes[0]], out_obj);
	aiReleaseImport(scene);
}
