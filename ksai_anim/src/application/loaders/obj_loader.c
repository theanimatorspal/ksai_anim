#include <assimp/aabb.h> 
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <engine/renderer/scene.h>

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

		if (in_mesh->mTangents != NULL)
		{
			out_obj->vertices[i].tangent[0] = in_mesh->mTangents[i].x;
			out_obj->vertices[i].tangent[1] = in_mesh->mTangents[i].y;
			out_obj->vertices[i].tangent[2] = in_mesh->mTangents[i].z;
		}

		if (in_mesh->mBitangents != NULL)
		{
			out_obj->vertices[i].bit_tangent[0] = in_mesh->mBitangents[i].x;
			out_obj->vertices[i].bit_tangent[1] = in_mesh->mBitangents[i].y;
			out_obj->vertices[i].bit_tangent[2] = in_mesh->mBitangents[i].z;
		}

		if (in_mesh->mTextureCoords[0] != NULL)
		{
			out_obj->vertices[i].tex_coord[0] = in_mesh->mTextureCoords[0][i].x;
			out_obj->vertices[i].tex_coord[1] = in_mesh->mTextureCoords[0][i].y;
		}
	}
}

static void read_and_add_kie_Objects(kie_Scene *scene, const struct aiScene *aiscene, struct aiNode *node)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		kie_Object obj;
		kie_Object_init(&obj);
		write_to_meshes(aiscene->mMeshes[node->mMeshes[i]], &obj);
		kie_Scene_add_object(scene, 1, &obj);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		read_and_add_kie_Objects(scene, aiscene, node->mChildren[i]);
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

void read_add_auto_objs(kie_Scene *scene, const char path[KSAI_SMALL_STRING_LENGTH])
{
	const struct aiScene *aiscene = aiImportFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	read_and_add_kie_Objects(scene, aiscene, aiscene->mRootNode);

}
