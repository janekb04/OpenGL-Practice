//This is an independent project of an individual developer.Dear PVS - Studio, please check it.
//PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma once

#include "External.h"
#include "opengl_context.h"


static class WindowManager
{
private:
	inline static std::thread::id main_thread_id = std::this_thread::get_id();;

	template<typename>
	friend class Window;
public:
	class context_guard
	{
		mutable GLFWwindow* previous;
	private:
		explicit context_guard(const GLFWwindow* new_context) : previous(glfwGetCurrentContext())
		{
			glfwMakeContextCurrent(const_cast<GLFWwindow*>(new_context));
		}
	public:
		context_guard() = delete;
		context_guard(const context_guard&) = delete;
		context_guard& operator=(const context_guard&) = delete;

		~context_guard()
		{
			glfwMakeContextCurrent(previous);
		}
		
		template<typename>
		friend class Window;
	};
	static inline void force_main_thread(const std::string& operation_name)
	{
		if (std::this_thread::get_id() != main_thread_id)
		{
			throw std::logic_error("Operation " + operation_name + " can only be performed on the main thread");
		}
	}
	static void init()
	{
		force_main_thread("WindowManager::init");
		glfwInit();
	}
	static void poll_events()
	{
		force_main_thread("WindowManager::poll_events");
		glfwPollEvents();
	}
	static void wait_events()
	{
		force_main_thread("WindowManager::wait_events");
		glfwWaitEvents();
	}
	static void terminate()
	{
		force_main_thread("WindowManager::terminate");
		glfwTerminate();
	}
};


template <typename Context>
class Window
{
public:
	struct WindowOptions
	{
		bool tranparent_framebuffer = false;
		bool decorated = true;
		bool resizable = false;
		bool use_imgui = false;
	};
	struct WindowProperties
	{
		int width = 400, height = 300;
		std::string title;
		GLFWmonitor* monitor = nullptr;
	};
	struct context_wrapper
	{
		Context context;
		mutable std::recursive_mutex mutex;
	
		template <typename... Args>
		context_wrapper(Args&&... args) :
			context(args...),
			mutex()
		{
		}
	};
private:
	WindowProperties properties;
	bool owner;
	GLFWwindow* handle;
	context_wrapper* wrapped_context;
private:
	template <typename... Args>
	static inline context_wrapper* make_context(Args&&... args, const GLFWwindow* handle)
	{
		WindowManager::context_guard guard(handle);
		return new context_wrapper(std::forward<Args>(args)...);
	}

	GLFWwindow* create_window(GLFWwindow* share_, const WindowOptions& options_) const
	{
		WindowManager::force_main_thread("Window::Window");
		GLFWwindow* handle;

		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, options_.tranparent_framebuffer);
		glfwWindowHint(GLFW_DECORATED, options_.decorated);
		glfwWindowHint(GLFW_RESIZABLE, options_.resizable);

#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // DEBUG
		
		handle = glfwCreateWindow(
			properties.width,
			properties.height,
			properties.title.c_str(),
			properties.monitor,
			share_
		);
		if (handle == NULL)
		{
			throw std::runtime_error("Failed to create Window");
		}

		glfwSetWindowUserPointer(handle, const_cast<Window*>(this));
		if (owner)
		{
			WindowManager::context_guard guard(handle);
			glewInit();
			glViewport(0, 0, properties.width, properties.height);
#ifdef DEBUG
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(opengl_error_callback, NULL);
#endif // DEBUG

			if (options_.use_imgui)
			{
				IMGUI_CHECKVERSION();
				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO(); (void)io;
				//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
				//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

				ImGui_ImplGlfw_InitForOpenGL(handle, true);
				ImGui_ImplOpenGL3_Init();
			}
		}
		
		//Window
		glfwSetFramebufferSizeCallback(handle, framebuffer_size_handler);
		glfwSetWindowRefreshCallback(handle, refresh_handler);
		glfwSetWindowFocusCallback(handle, focus_handler);
		glfwSetWindowIconifyCallback(handle, minimize_handler);
		glfwSetWindowMaximizeCallback(handle, maximize_handler);
		glfwSetWindowCloseCallback(handle, close_handler);
		glfwSetWindowPosCallback(handle, position_handler);
		glfwSetWindowSizeCallback(handle, size_handler);
		glfwSetWindowContentScaleCallback(handle, content_scale_handler);
		//Input
		glfwSetDropCallback(handle, drop_handler);

		return handle;
	}
private:
	static void APIENTRY opengl_error_callback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* msg,
		const void* data
	) {
		char message[BUFSIZ];

		const char* _source;
		const char* _type;
		const char* _severity;

		switch (source) {
		case GL_DEBUG_SOURCE_API:
			_source = "API";
			break;

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			_source = "WINDOW_SYSTEM";
			break;

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			_source = "SHADER_COMPILER";
			break;

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			_source = "THIRD_PARTY";
			break;

		case GL_DEBUG_SOURCE_APPLICATION:
			_source = "APPLICATION";
			break;

		case GL_DEBUG_SOURCE_OTHER:
			_source = "OTHER";
			break;

		default:
			_source = "UNKNOWN";
			break;
		}

		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			_type = "ERROR";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "DEPRECATED_BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			_type = "UDEFINED_BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "PORTABILITY";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			_type = "PERFORMANCE";
			break;

		case GL_DEBUG_TYPE_OTHER:
			_type = "OTHER";
			break;

		case GL_DEBUG_TYPE_MARKER:
			_type = "MARKER";
			break;

		default:
			_type = "UNKNOWN";
			break;
		}

		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "HIGH";
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "MEDIUM";
			break;

		case GL_DEBUG_SEVERITY_LOW:
			_severity = "LOW";
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			_severity = "NOTIFICATION";
			break;

		default:
			_severity = "UNKNOWN";
			break;
		}

		std::snprintf(message, BUFSIZ, "%s (%d) of %s severity, raised from %s: %s\n", _type, id, _severity, _source, msg);

		std::cout << message;
	}

	static inline Window* get_instance(GLFWwindow* HWND) noexcept
	{
		Window* instance = (Window*)(glfwGetWindowUserPointer(HWND));
		assert(instance != nullptr);
		return instance;
	}

	//Window callback handlers
	static void framebuffer_size_handler(GLFWwindow* HWND, int x, int y)
	{
		auto instance = get_instance(HWND);
		instance->framebuffer_size_callback(x, y);
	}
	static void refresh_handler(GLFWwindow* HWND)
	{
		auto instance = get_instance(HWND);
		instance->refresh_callback();
	}
	static void focus_handler(GLFWwindow* HWND, int state)
	{
		auto instance = get_instance(HWND);
		instance->focus_callback(state);
	}
	static void minimize_handler(GLFWwindow* HWND, int state)
	{
		auto instance = get_instance(HWND);
		instance->minimize_callback(state);
	}
	static void maximize_handler(GLFWwindow* HWND, int state)
	{
		auto instance = get_instance(HWND);
		instance->maximize_callback(state);
	}
	static void close_handler(GLFWwindow* HWND)
	{
		auto instance = get_instance(HWND);
		instance->close_callback();
	}
	static void position_handler(GLFWwindow* HWND, int x, int y)
	{
		auto instance = get_instance(HWND);
		instance->position_callback(x, y);
	}
	static void size_handler(GLFWwindow* HWND, int x, int y)
	{
		auto instance = get_instance(HWND);
		instance->size_callback(x, y);
	}
	static void content_scale_handler(GLFWwindow* HWND, float x, float y)
	{
		auto instance = get_instance(HWND);
		instance->content_scale_callback(x, y);
	}
	//Input callback handlers
	static void drop_handler(GLFWwindow* HWND, int amount, const char** paths)
	{
		auto instance = get_instance(HWND);
		instance->drop_callback(amount, paths);
	}
protected:
	//Window callbacks
	virtual void framebuffer_size_callback(int width, int height) {}
	virtual void refresh_callback() {}
	virtual void focus_callback(bool focused) {}
	virtual void minimize_callback(bool minimized) {}
	virtual void maximize_callback(bool maximized) {}
	virtual void close_callback() {}
	virtual void position_callback(int x, int y) {}
	virtual void size_callback(int width, int height) {}
	virtual void content_scale_callback(float x, float y) {}
	//Input callbacks
	virtual void drop_callback(int count, const char** paths) {}

public:	
	Window(
		const Window& share_,
		const WindowOptions& options_ = WindowOptions(),
		const WindowProperties& properties_ = WindowProperties()
	) : properties(properties_),
		owner(false),
		handle(create_window(share_.handle, options_)),
		wrapped_context(share_.wrapped_context)
	{
	}

	template <typename... Args>
	Window(
		const std::tuple<Args...>& args_ = std::tuple(),
		const WindowOptions& options_ = WindowOptions(),
		const WindowProperties& properties_ = WindowProperties()
	) : properties(properties_),
		owner(true),
		handle(create_window(NULL, options_)),
		wrapped_context(std::apply(make_context<Args...>, std::tuple_cat(args_, std::make_tuple(handle))))
	{
	}

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;

	virtual ~Window() 
	{
		WindowManager::force_main_thread("Window::~Window");
		if (owner)
		{
			delete wrapped_context;
		}
		glfwDestroyWindow(handle);
	}
public:
	bool should_close() const noexcept
	{
		return glfwWindowShouldClose(handle);
	}

	void swap_buffers() const noexcept
	{
		glfwSwapBuffers(handle);
	}

	void set_should_close(bool value) const noexcept
	{
		glfwSetWindowShouldClose(handle, value);
	}

public:
	template<typename... Args>
	using context_func = void(*)(Window&, Context&, Args&&...);
	
	template<typename... Args>
	using const_context_func = void(*)(const Window&, const Context&, Args&&...);

	template<typename... Args>
	void run_in_context(context_func<Args&&...> func, Args&&... args)
	{
		std::lock_guard lock(wrapped_context->mutex);
		WindowManager::context_guard guard(handle);
		func(*this, wrapped_context->context, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void run_in_context(const_context_func<Args&&...> func, Args&&... args) const
	{
		std::lock_guard lock(wrapped_context->mutex);
		WindowManager::context_guard guard(handle);
		func(*this, wrapped_context->context, std::forward<Args>(args)...);
	}

	void run_in_context(context_func<> func)
	{
		std::lock_guard lock(wrapped_context->mutex);
		WindowManager::context_guard guard(handle);
		func(*this, wrapped_context->context);
	}

	void run_in_context(const_context_func<> func) const
	{
		std::lock_guard lock(wrapped_context->mutex);
		WindowManager::context_guard guard(handle);
		func(*this, wrapped_context->context);
	}

public: //Input
	bool is_key_pressed(int key_code)
	{
		return glfwGetKey(handle, key_code) == GLFW_PRESS;
	}

public:	//Low level
	Context& get_context()
	{
		return wrapped_context->context;
	}

	const Context& get_context() const
	{
		return wrapped_context->context;
	}

	static bool share_context(const Window& a, const Window& b)
	{
		return a.wrapped_context == b.wrapped_context;
	}

	bool is_owner() const
	{
		return owner;
	}

	static void swap(Window& first, Window& second)
	{
		std::swap(first.handle, second.handle);
		std::swap(first.wrapped_context, second.wrapped_context);
		std::swap(first.owner, second.owner);
		std::swap(first.properties, second.properties);
	}

	void transfer_context_ownership(Window& other)
	{
		assert(owner == true);
		assert(share_context(*this, other));
		other.owner = true;
		owner = false;
	}

	WindowManager::context_guard get_context_guard() const
	{
		return WindowManager::context_guard(handle);
	}
};