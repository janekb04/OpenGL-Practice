// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "External.h"
#include "defines.h"
#include "window.h"
#include "mesh.h"
#include "texture.h"
#include "material.h"
#include "model.h"
#include "Transform.h"
#include "GameObject.h"	
#include "Shader.h"
#include "Framebuffer.h"
#include "Cubemap.h"

std::string read_file(const char* filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (file)
	{
		std::string contents;
		file.seekg(0, std::ios::end);
		contents.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(contents.data(), contents.size());
		file.close();
		return contents;
	}
	return std::string();
}

static class MainCamera
{
	inline static glm::mat4 projection{ glm::perspective(glm::radians(45.0f), float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 100.0f) };

public:
	struct Transform
	{
		glm::vec3 position = { 0, 0, 0 };
		float pitch = 0, yaw = 0, roll = 0;

		glm::mat4 to_mat4() const
		{
			glm::mat4 transformation(1.0f);

			transformation = glm::rotate(transformation, -glm::radians(pitch), glm::vec3(1, 0, 0));
			transformation = glm::rotate(transformation, -glm::radians(yaw), glm::vec3(0, 1, 0));
			transformation = glm::rotate(transformation, -glm::radians(roll), glm::vec3(0, 0, 1));
			transformation = glm::translate(transformation, -position);

			return transformation;
		}
	};
	inline static Transform transform;

	static const glm::mat4& get_projection_matrix()
	{
		return projection;
	}
};

struct MyContext
{
private:
	//Scene
	Texture2D textures[9]
	{
		load_texture("res/clock/albedo.jpg", true),
		load_texture("res/clock/metalness.jpg", true),
		load_texture("res/clock/normal.jpg", false),
		load_texture("res/table/diffuse.png", true),
		load_texture("res/table/specular.png", true),
		load_texture("res/table/normal.png", false),
		load_texture("res/container/diffuse.png", true),
		load_texture("res/container/specular.png", true),
		load_texture("res/container/normal.png", false)
	};
	Material materials[3]
	{
		Material{
			&textures[0],
			&textures[1],
			&textures[2],
			32.0f
		},
		Material{
			&textures[3],
			&textures[4],
			&textures[5],
			256.0f
		},
		Material{
			&textures[6],
			&textures[7],
			&textures[8],
			32.0f
		}
	};
	Mesh meshes[3]
	{
		std::move(load_meshes("res/clock/clock.obj")[0]),
		std::move(load_meshes("res/table/Table.obj")[0]),
		std::move(load_meshes("res/container/container.obj")[0]),
	};
	Model models[3]
	{
		Model{
			&materials[0],
			&meshes[0]
		},
		Model{
			&materials[1],
			&meshes[1]
		},
		Model{
			&materials[2],
			&meshes[2]
		}
	};

	GameObject objects[4]
	{
		GameObject{
			&models[0],
			Transform {
				{ 1.5f, 0.2f, 2.5f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.3f, 0.3f, 0.3f },
			}
		},
		GameObject{
			&models[0],
			Transform {
				{-2.0f, 0.2f, 3.5f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.3f, 0.3f, 0.3f },
			}
		},
		GameObject{
			&models[1],
			Transform {
				{ 0.0f,-5.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f },
			}
		},
		GameObject{
			&models[2],
			Transform {
				{ 0.0f, 1.2f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f },
			}
		}
	};

	//Settings
	glm::vec3 light_direction = { -0.4f, -1.0f, -0.8f };
	glm::vec3 light_color = { 1.0f, 1.0f, 1.0f };
	glm::vec3 ambient_color = { 80 / (float)255, 110 / (float)255, 148 / (float)255 };

	//Shadow
	Texture2D shadow_map{ NULL, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, false };
	Framebuffer shadow_fbo{ std::vector<std::monostate>{}, shadow_map, std::monostate{} };
	Shader depth_only{ read_file("shader/depth_only.vert"), read_file("shader/depth_only.frag") };
	GLint MVP_loc{ glGetUniformLocation(depth_only.id, "MVP_") };

	//Skybox
	const char* skybox_faces_paths[6]
	{
		"res/skybox/right.jpg",
		"res/skybox/left.jpg",
		"res/skybox/top.jpg",
		"res/skybox/bottom.jpg",
		"res/skybox/front.jpg",
		"res/skybox/back.jpg"
	};
	Cubemap skybox{ load_cubemap(&skybox_faces_paths[0], false) };
	Shader skybox_shader{ read_file("shader/skybox.vert"), read_file("shader/skybox.frag") };
	GLint VP_loc{ glGetUniformLocation(skybox_shader.id, "VP_") };
	GLint skybox_loc{ glGetUniformLocation(skybox_shader.id, "skybox_") };
	const GLuint skybox_unit = 0;

private:
	struct BlinnPhong
	{
	public:
		const Shader shader;
		const GLuint ambient_color_loc;
		const GLuint light_color_loc;
		const GLuint light_direction_loc;
		const GLuint diffuse_loc;
		const GLuint specular_loc;
		const GLuint normal_map_loc;
		const GLuint shininess_loc;
		const GLuint shadow_map_loc;
		const GLuint MVP_loc;
		const GLuint MV_loc;
		const GLuint N_loc;
		const GLuint light_mat_loc;
		const inline static GLuint diffuse_unit = 0;
		const inline static GLuint specular_unit = 1;
		const inline static GLuint normal_map_unit = 2;
		const inline static GLuint shadow_map_unit = 3;

		BlinnPhong(glm::vec3 ambient_color_, glm::vec3 light_color_) :
			shader(read_file("shader/vertex.vert"), read_file("shader/fragment.frag")),
			ambient_color_loc(glGetUniformLocation(shader.id, "ambient_color_")),
			light_color_loc(glGetUniformLocation(shader.id, "light_color_")),
			light_direction_loc(glGetUniformLocation(shader.id, "light_direction_")),
			diffuse_loc(glGetUniformLocation(shader.id, "diffuse_")),
			specular_loc(glGetUniformLocation(shader.id, "specular_")),
			normal_map_loc(glGetUniformLocation(shader.id, "normal_map_")),
			shadow_map_loc(glGetUniformLocation(shader.id, "shadow_map_")),
			shininess_loc(glGetUniformLocation(shader.id, "shininess_")),
			MVP_loc(glGetUniformLocation(shader.id, "MVP_")),
			MV_loc(glGetUniformLocation(shader.id, "MV_")),
			N_loc(glGetUniformLocation(shader.id, "N_")),
			light_mat_loc(glGetUniformLocation(shader.id, "light_mat_"))
		{
			shader.use();

			glUniform1i(diffuse_loc, diffuse_unit);
			glUniform1i(specular_loc, specular_unit);
			glUniform1i(normal_map_loc, normal_map_unit);
			glUniform1i(shadow_map_loc, shadow_map_unit);

			set_ambient_color(ambient_color_);
			set_light_color(light_color_);
		}

		void set_ambient_color(glm::vec3 ambient_color_)
		{
			glUniform3fv(ambient_color_loc, 1, &ambient_color_[0]);
			glClearColor(ambient_color_.x, ambient_color_.y, ambient_color_.z, 0.0f);
		}

		void set_light_color(glm::vec3 light_color_)
		{
			glUniform3fv(light_color_loc, 1, &light_color_[0]);
		}

		void set_light_direction(glm::vec3 light_direction_)
		{
			glUniform3fv(light_direction_loc, 1, &light_direction_[0]);
		}

		void set_MVP(const glm::mat4& MVP)
		{
			glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
		}

		void set_MV(const glm::mat4& MV)
		{
			glUniformMatrix4fv(MV_loc, 1, GL_FALSE, glm::value_ptr(MV));
		}

		void set_N(const glm::mat4& N)
		{
			glUniformMatrix4fv(N_loc, 1, GL_FALSE, glm::value_ptr(N));
		}

		void set_light_mat(const glm::mat4& light_mat)
		{
			glUniformMatrix4fv(light_mat_loc, 1, GL_FALSE, glm::value_ptr(light_mat));
		}

		inline void use()
		{
			shader.use();
		}
	};
	BlinnPhong blinn_phong{ glm::pow(ambient_color, glm::vec3(1.0f / 2.2f)), light_color };

public:
	MyContext(int width_, int height_)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glDepthFunc(GL_LEQUAL);


		glActiveTexture(GL_TEXTURE0 + blinn_phong.shadow_map_unit);
		glBindTexture(GL_TEXTURE_2D, shadow_map.get_id());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);


		skybox_shader.use();
		glUniform1i(skybox_loc, skybox_unit);
		glActiveTexture(GL_TEXTURE0 + skybox_unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get_id());
	}

	void next_frame()
	{
		glm::mat4 shadow_mat{ glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f) * glm::lookAt(glm::normalize(-light_direction) * 3.0f + MainCamera::transform.position, MainCamera::transform.position, glm::vec3(0.0f, 1.0f, 0.0f)) };
		
		depth_only.use();
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
		shadow_fbo.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		for (const auto& object : objects)
		{
			auto MVP = shadow_mat * object.transform.to_mat4();
			glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
			object.model->mesh->bind();
			glDrawElements(GL_TRIANGLES, object.model->mesh->get_indices_count(), GL_UNSIGNED_SHORT, 0);
		}
		
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
		blinn_phong.use();

		glm::mat4 V = MainCamera::transform.to_mat4();
		blinn_phong.set_light_direction(glm::normalize(glm::vec4(light_direction, 0.0f)));
		blinn_phong.set_light_color(light_color);
		blinn_phong.set_ambient_color(ambient_color);

		for (const auto& object : objects)
		{
			auto model = object.model;
			auto material = model->material;
			auto mesh = model->mesh;
			
			glActiveTexture(GL_TEXTURE0 + blinn_phong.diffuse_unit);
			glBindTexture(GL_TEXTURE_2D, material->diffuse->get_id());
			glActiveTexture(GL_TEXTURE0 + blinn_phong.specular_unit);
			glBindTexture(GL_TEXTURE_2D, material->specular->get_id());
			glActiveTexture(GL_TEXTURE0 + blinn_phong.normal_map_unit);
			glBindTexture(GL_TEXTURE_2D, material->normal->get_id());
			glUniform1f(blinn_phong.shininess_loc, material->shininess);

			mesh->bind();

			glm::mat4 M = object.transform.to_mat4();
			glm::mat4 MV = V * M;
			glm::mat4 light_mat = shadow_mat * M;
			blinn_phong.set_MV(MV);
			blinn_phong.set_MVP(MainCamera::get_projection_matrix() * MV);
			blinn_phong.set_N(glm::mat3(glm::transpose(glm::inverse(M))));
			blinn_phong.set_light_mat(light_mat);

			glDrawElements(GL_TRIANGLES, mesh->get_indices_count(), GL_UNSIGNED_SHORT, 0);
		}

		skybox_shader.use();
		glm::mat4 VP = MainCamera::get_projection_matrix() * glm::mat4(glm::mat3(V));
		glUniformMatrix4fv(VP_loc, 1, GL_FALSE, glm::value_ptr(VP));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (ImGui::Begin("Settings"))
			{
				static glm::vec3 pos{ 0, 0, 0 };
				ImGui::DragFloat3("Position", &pos.x, 0.05f);

				static glm::vec3 rot{ 0, 0, 0 };
				ImGui::DragFloat3("Rotation", &rot.x, 0.5f);

				static glm::vec3 scl{ 1, 1, 1 };
				ImGui::DragFloat3("Scale", &scl.x, 0.1f);

				ImGui::Separator();

				static int idx;
				ImGui::DragInt("Object index", &idx, 0.1, 0, 3);

				static bool auto_apply = false;
				ImGui::Checkbox("Auto-apply", &auto_apply);

				if (ImGui::Button("Apply") || auto_apply)
				{
					auto& t = objects[idx].transform;
					t.set_position(pos);
					t.set_orientation(glm::radians(rot));
					t.set_scale(scl);
				}
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}
};

class ModelViewer : public Window<MyContext>
{
	virtual void close_callback() override
	{
		set_should_close(false);
	}
public:
	using Window<MyContext>::Window;
};

ModelViewer make_window()
{
	return ModelViewer{
		std::tuple{ SCREEN_WIDTH, SCREEN_HEIGHT },
		Window<MyContext>::WindowOptions{
			false,
			true,
			false,
			true
		},
		Window<MyContext>::WindowProperties{
			SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", 0
		}
	};
}

int main()
{	
	WindowManager::init();
	
	auto main_window = make_window();

	main_window.run_in_context([](Window<MyContext>& wnd, MyContext& context) {
		float speed = 0.05f;
		while (!wnd.should_close())
		{
			if (wnd.is_key_pressed(GLFW_KEY_A))
				MainCamera::transform.position.x -= speed;
			if (wnd.is_key_pressed(GLFW_KEY_D))
			MainCamera::transform.position.x += speed;
			if (wnd.is_key_pressed(GLFW_KEY_W))
				MainCamera::transform.position.z -= speed;
			if (wnd.is_key_pressed(GLFW_KEY_S))
				MainCamera::transform.position.z += speed;
			if (wnd.is_key_pressed(GLFW_KEY_R))
				MainCamera::transform.position.y += speed;
			if (wnd.is_key_pressed(GLFW_KEY_F))
				MainCamera::transform.position.y -= speed;
			if (wnd.is_key_pressed(GLFW_KEY_Q))
				MainCamera::transform.pitch += speed * 10;
			if (wnd.is_key_pressed(GLFW_KEY_E))
				MainCamera::transform.pitch -= speed * 10;
			if (wnd.is_key_pressed(GLFW_KEY_Z))
				MainCamera::transform.yaw += speed * 10;
			if (wnd.is_key_pressed(GLFW_KEY_C))
				MainCamera::transform.yaw -= speed * 10;
			if (wnd.is_key_pressed(GLFW_KEY_UP))
				speed *= 1.01;
			if (wnd.is_key_pressed(GLFW_KEY_DOWN))
				speed /= 1.01;
			
			context.next_frame();
			wnd.swap_buffers();
			WindowManager::poll_events();
		}
	});

	return 0;
}