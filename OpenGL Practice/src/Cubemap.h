#pragma once

#include "External.h"
#include "texture.h"

class Cubemap
{
private:
	GLuint id;

public:
	const int width, height;

public:
	Cubemap(const unsigned char** const data, int width_, int height_, int channel_count, bool uses_srgb) :
		id(NULL),
		width(width_),
		height(height_)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (GLuint i = 0; i < 6; i++)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				interal_format_from_channel_count(channel_count, uses_srgb),
				width,
				height,
				0,
				format_from_channel_count(channel_count),
				GL_UNSIGNED_BYTE,
				data[i]
			);
		}
	}

	Cubemap(const Cubemap&) = delete;
	Cubemap(Cubemap&& other) noexcept :
		id(other.id),
		width(other.width),
		height(other.height)
	{
		other.id = NULL;
	}

	inline GLuint get_id()
	{
		return id;
	}

	~Cubemap()
	{
		glDeleteTextures(1, &id);
	}
};

Cubemap load_cubemap(char const** paths, bool is_color_data)
{
	int width, height, channel_count;

	unsigned char* data[6];
	data[0] = stbi_load(paths[0], &width, &height, &channel_count, 0);
	if (data[0] == nullptr)
	{
		throw std::runtime_error("Failed to load cubemap");
	}

	for (int i = 1; i < 6; ++i)
	{
		int c_width, c_height, c_channel_count;
		
		data[i] = stbi_load(paths[i], &c_width, &c_height, &c_channel_count, 0);
		if (c_width != width || c_height != height || c_channel_count != channel_count || data[i] == nullptr)
		{
			for (int j = 0; j <= i; ++j)
				stbi_image_free(data[j]);
			throw std::runtime_error("Failed to load cubemap");
		}
	}

	Cubemap cubemap(const_cast<const unsigned char** const>(&data[0]), width, height, channel_count, is_color_data);
	for (int i = 0; i < 6; ++i)
		stbi_image_free(data[i]);
	return cubemap;
}