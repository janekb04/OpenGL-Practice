#pragma once

#include "External.h"

class Mesh
{	
private:
	enum { VBO = 0, EBO = 1, BUF_COUNT = 2};
	GLuint buffers[BUF_COUNT];
	GLuint VAO;
	std::size_t indices_count;

public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec2 uv;

		Vertex(const glm::vec3& position_, const glm::vec3& normal_, const glm::vec3& tangent_, const glm::vec2& uv_) :
			position(position_),
			normal(normal_),
			tangent(tangent_),
			uv(uv_)
		{
		}
	};
	
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned short>& indices):
		indices_count(indices.size())
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(BUF_COUNT, &buffers[0]);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[VBO]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[EBO]);

		glNamedBufferData(buffers[VBO], vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glNamedBufferData(buffers[EBO], indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(3);
	}

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept:
		VAO(other.VAO),
		indices_count(other.indices_count)
	{
		buffers[VBO] = other.buffers[VBO];
		other.buffers[VBO] = NULL;
		buffers[EBO] = other.buffers[EBO];
		other.buffers[EBO] = NULL;
		other.VAO = NULL;
	}

	~Mesh()
	{
		if (VAO != NULL)
		{
			glDeleteBuffers(BUF_COUNT, &buffers[0]);
			glDeleteVertexArrays(1, &VAO);
		}
	}

	inline void bind() const
	{
		glBindVertexArray(VAO);
	}

	inline std::size_t get_indices_count() const
	{
		return indices_count;
	}
};

std::vector<Mesh> load_meshes(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error(importer.GetErrorString());
	}

	const std::size_t mesh_count = scene->mNumMeshes;
	std::vector<Mesh> meshes;
	meshes.reserve(mesh_count);
	for (int i = 0; i < mesh_count; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		
		std::vector<Mesh::Vertex> vertices;
		std::vector<unsigned short> indices;

		for (int j = 0; j < mesh->mNumVertices; j++)
		{
			glm::vec3 position; 
			position.x = mesh->mVertices[j].x;
			position.y = mesh->mVertices[j].y;
			position.z = mesh->mVertices[j].z;

			glm::vec3 normal;
			normal.x = mesh->mNormals[j].x;
			normal.y = mesh->mNormals[j].y;
			normal.z = mesh->mNormals[j].z;

			glm::vec3 tangent;
			tangent.x = mesh->mTangents[j].x;
			tangent.y = mesh->mTangents[j].y;
			tangent.z = mesh->mTangents[j].z;

			glm::vec3 bitangent;
			bitangent.x = mesh->mBitangents[j].x;
			bitangent.y = mesh->mBitangents[j].y;
			bitangent.z = mesh->mBitangents[j].z;

			if (glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f)
			{
				tangent *= -1.0f;
			}

			glm::vec2 uv;
			if (mesh->mTextureCoords[0])
			{
				uv.x = mesh->mTextureCoords[0][j].x;
				uv.y = mesh->mTextureCoords[0][j].y;
			}
			else
				uv = glm::vec2(0.0f, 0.0f);
			
			vertices.emplace_back(position, normal, tangent, uv);
		}

		for (int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];

			for (int k = 0; k < face.mNumIndices; k++)
				indices.push_back(face.mIndices[k]);
		}

		meshes.emplace_back(vertices, indices);
	}

	return meshes;
}