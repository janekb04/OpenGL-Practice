#pragma once

#include "External.h"
#include "texture.h"

class Renderbuffer
{
private:
	GLuint id;

public:
	Renderbuffer(GLenum format, int width, int height)
	{
		glGenRenderbuffers(1, &id);

		glNamedRenderbufferStorage(id, format, width, height);
	}

	GLuint get_id() const
	{
		return id;
	}

	~Renderbuffer()
	{
		glDeleteRenderbuffers(1, &id);
	}
};

class Framebuffer
{
private:
	inline void set_no_color_buffer()
	{
		glNamedFramebufferDrawBuffer(id, GL_NONE);
		glNamedFramebufferReadBuffer(id, GL_NONE);
	}

private:
	GLuint id;

public:
	template <typename CBT>
	Framebuffer(const std::vector<CBT>& color_buffers)
	{
		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		if constexpr (std::is_same_v<CBT, Texture2D>)
		{
			if (color_buffers.size() > 0)
			{
				int i = 0;
				for (const auto& texture : color_buffers)
				{
					glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0 + i, texture.get_id(), 0);
					++i;
				}
			}
			else
				set_no_color_buffer();
		}
		else if constexpr (std::is_same_v<CBT, Renderbuffer>)
		{
			if (color_buffers.size() > 0)
			{
				int i = 0;
				for (const auto& renderbuffer : color_buffers)
				{
					glNamedFramebufferRenderbuffer(id, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, renderbuffer.get_id());
					++i;
				}
			}
			else
				set_no_color_buffer();
		}
		else if constexpr (std::is_same_v<CBT, std::monostate>)
		{
			set_no_color_buffer();
		}
		else
			throw "Invalid type";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	template <typename CBT, typename DSBT>
	Framebuffer(const std::vector<CBT>& color_buffers, const DSBT& depth_stencil_buffer) :
		Framebuffer(color_buffers)
	{
		if constexpr (std::is_same_v<DSBT, Texture2D>)
		{
			glNamedFramebufferTexture(id, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_buffer.get_id(), 0);
		}
		else if constexpr (std::is_same_v<DSBT, Renderbuffer>)
		{
			glNamedFramebufferRenderbuffer(id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer.get_id());
		}
		else
			throw "Invalid type";
	}

	template <typename CBT, typename DBT, typename SBT>
	Framebuffer(const std::vector<CBT>& color_buffers, const DBT& depth_buffer, const SBT& stencil_buffer) :
		Framebuffer(color_buffers)
	{
		if constexpr (std::is_same_v<DBT, Texture2D>)
		{
			glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, depth_buffer.get_id(), 0);
		}
		else if constexpr (std::is_same_v<DBT, Renderbuffer>)
		{
			glNamedFramebufferRenderbuffer(id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer.get_id());
		}
		else if constexpr (!std::is_same_v<DBT, std::monostate>)
			throw "Invalid type";

		if constexpr (std::is_same_v<SBT, Texture2D>)
		{
			glNamedFramebufferTexture(id, GL_STENCIL_ATTACHMENT, stencil_buffer.get_id(), 0);
		}
		else if constexpr (std::is_same_v<SBT, Renderbuffer>)
		{
			glNamedFramebufferRenderbuffer(id, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_buffer.get_id());
		}
		else if constexpr (!std::is_same_v<SBT, std::monostate>)
			throw "Invalid type";
	}

	void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id);
	}

	~Framebuffer()
	{
		glDeleteFramebuffers(1, &id);
	}
};