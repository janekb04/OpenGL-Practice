//// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
//
//// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
//#include "External.h"
//#include "defines.h"
//#include "window.h"
//#include "mesh.h"
//#include "texture.h"
//#include "material.h"
//#include "model.h"
//#include "Transform.h"
//#include "GameObject.h"
//
//struct MyContext
//{
//private:
//	int selected_idx = 0;
//private:
//	//Clock
//	Texture2D clock_diffuse;
//	Texture2D clock_specular;
//	Material clock_material;
//	Mesh clock_mesh;
//	Model clock_model;
//
//	//Table
//	Texture2D table_diffuse;
//	Texture2D table_specular;
//	Material table_material;
//	Mesh table_mesh;
//	Model table_model;
//
//	//Container
//	Texture2D container_diffuse;
//	Texture2D container_specular;
//	Material container_material;
//	Mesh container_mesh;
//	Model container_model;
//
//	//Data
//	glm::vec3 light_pos = { 0.0f, 6.0f, 5.0f };
//	glm::vec3 light_color = { 10.0f, 10.0f, 10.0f };
//	glm::vec3 ambient_color = { 0.03f, 0.03f, 0.03f };
//
//public:
//	std::vector<GameObject> objects =
//	{
//		{clock_model},
//		{clock_model},
//		{table_model},
//		{container_model},
//		{container_model},
//		{container_model}
//	};
//
//public:
//	MyContext(int width_, int height_) :
//		clock_diffuse(load_texture("res/clock/albedo.jpg", true)),
//		clock_specular(load_texture("res/clock/metalness.jpg", true)),
//		clock_material{ &clock_diffuse, &clock_specular, 32.0f },
//		clock_mesh(std::move(load_meshes("res/clock/clock.obj")[0])),
//		clock_model{ &clock_material, &clock_mesh },
//
//		table_diffuse(load_texture("res/table/diffuse.png", true)),
//		table_specular(load_texture("res/table/specular.png", true)),
//		table_material{ &table_diffuse, &table_specular, 50.0f },
//		table_mesh(std::move(load_meshes("res/table/Table.obj")[0])),
//		table_model{ &table_material, &table_mesh },
//
//		container_diffuse(load_texture("res/container/diffuse.png", true)),
//		container_specular(load_texture("res/container/specular.png", true)),
//		container_material{ &container_diffuse, &container_specular, 50.0f },
//		container_mesh(std::move(load_meshes("res/container/container.obj")[0])),
//		container_model{ &container_material, &container_mesh }
//	{
//		glEnable(GL_DEPTH_TEST);
//		glCullFace(GL_BACK);
//		glEnable(GL_FRAMEBUFFER_SRGB);
//
//		BlinnPhong::init(ambient_color, light_color);
//
//	}
//
//	void next_frame()
//	{
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		BlinnPhong::use();
//
//		glm::mat4 view = MainCamera::transform.to_mat4();
//		BlinnPhong::set_light_position(view * glm::vec4(light_pos, 1.0f));
//		BlinnPhong::set_light_color(light_color);
//		BlinnPhong::set_ambient_color(ambient_color);
//
//		for (const auto& object : objects)
//			object.draw();
//
//		{
//			ImGui_ImplOpenGL3_NewFrame();
//			ImGui_ImplGlfw_NewFrame();
//			ImGui::NewFrame();
//
//			ImGui::Begin("Settings");
//			{
//				ImGui::DragInt("Object index", &selected_idx, 0.2, 0, objects.size() - 1);
//				ImGui::DragFloat3("Position", &objects[selected_idx].transform.position[0], 0.8, -100, 100);
//				ImGui::DragFloat3("Orientation", &objects[selected_idx].transform.orientation[0], 0.8, 0, 360);
//				ImGui::DragFloat3("Scale", &objects[selected_idx].transform.scale[0], 0.3, -100, 100);
//
//				ImGui::NewLine();
//
//				ImGui::DragFloat3("Light Position", &light_pos[0], 0.8, -100, 100);
//				ImGui::ColorPicker3("Light Color", &light_color[0]);
//				ImGui::ColorPicker3("Ambient Color", &ambient_color[0]);
//			}
//			ImGui::End();
//
//			ImGui::Render();
//			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//		}
//	}
//};
//
//Window<MyContext> make_window()
//{
//	return Window<MyContext>{
//		std::tuple{ SCREEN_WIDTH, SCREEN_HEIGHT },
//			Window<MyContext>::WindowOptions{
//				false,
//				true,
//				false,
//				true
//		},
//			Window<MyContext>::WindowProperties{
//				SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", nullptr
//		},
//			Window<MyContext>::WindowCallbacks{
//			//[](Window<MyContext>& wnd, int width, int height) {
//			//	auto context = wnd.get_context();
//			//	context.resize(width, height);
//			//	context.next_frame();
//			//	wnd.swap_buffers();
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd) {
//			//	std::cout << "refresh\n";
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd, bool val) {
//			//	std::cout << "focus: " << val << '\n';
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd, bool val) {
//			//	std::cout << "minimize: " << val << '\n';
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd, bool val) {
//			//	std::cout << "maximize: " << val << '\n';
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd) {
//			//	std::cout << "close\n";
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd, int x, int y) {
//			//	std::cout << "position: " << x << ' ' << y << '\n';
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd, int width, int height) {
//			//	std::cout << "size: " << width << ' ' << height << '\n';
//			//},
//			nullptr,
//			//[](Window<MyContext>& wnd, float x, float y) {
//			//	std::cout << "scale: " << x << ' ' << y << '\n';
//			//},
//			nullptr
//		}
//	};
//}
//
//int main()
//{
//	WindowManager::init();
//
//	Window<MyContext> main_window = make_window();
//
//	main_window.run_in_context([](Window<MyContext>& wnd, MyContext& context) {
//		float speed = 0.05f;
//		while (!wnd.should_close())
//		{
//			if (wnd.is_key_pressed(GLFW_KEY_A))
//				MainCamera::transform.position.x -= speed;
//			if (wnd.is_key_pressed(GLFW_KEY_D))
//				MainCamera::transform.position.x += speed;
//			if (wnd.is_key_pressed(GLFW_KEY_W))
//				MainCamera::transform.position.z -= speed;
//			if (wnd.is_key_pressed(GLFW_KEY_S))
//				MainCamera::transform.position.z += speed;
//			if (wnd.is_key_pressed(GLFW_KEY_R))
//				MainCamera::transform.position.y += speed;
//			if (wnd.is_key_pressed(GLFW_KEY_F))
//				MainCamera::transform.position.y -= speed;
//			if (wnd.is_key_pressed(GLFW_KEY_Q))
//				MainCamera::transform.pitch += speed * 10;
//			if (wnd.is_key_pressed(GLFW_KEY_E))
//				MainCamera::transform.pitch -= speed * 10;
//			if (wnd.is_key_pressed(GLFW_KEY_Z))
//				MainCamera::transform.yaw += speed * 10;
//			if (wnd.is_key_pressed(GLFW_KEY_C))
//				MainCamera::transform.yaw -= speed * 10;
//			if (wnd.is_key_pressed(GLFW_KEY_UP))
//				speed *= 1.01;
//			if (wnd.is_key_pressed(GLFW_KEY_DOWN))
//				speed /= 1.01;
//
//			context.next_frame();
//			wnd.swap_buffers();
//			WindowManager::poll_events();
//		}
//		});
//
//	return 0;
//}