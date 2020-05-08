//
// Created by root on 10/3/20.
//

#ifndef WXRAYTRACER_GLSL_HPP
#define WXRAYTRACER_GLSL_HPP

#include <stdio.h>
#include <glad/glad.h>

#include "GLFW/glfw3.h"

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <Instance.h>
#include <GlossySpecular.h>
#include <Image.h>
#include <ImageTexture.hpp>
#include <SVAttributeBasedMapping.hpp>
#include <fstream>
#include <Matte.h>

#include "renderThread.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h> 
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/colorpicker.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui\formhelper.h>
#include <MarchingRayTracer.hpp>
#include <iomanip>
#include <Emissive.h>
#include "PointLight.h"


struct Log {
	template<typename... Args>
	static void info(Args... args) {
		std::cout << "\t[log]\t";
		(std::cout << ... << args);
		std::cout << std::endl;
	}

	template<typename... Args>
	static void error(Args... args) {
		std::cerr << "\t[log]\t";
		(std::cerr << ... << args);
		std::cerr << std::endl;
	}
} debug_log;

template<typename...Args>
struct EventDelegate {
	std::vector<std::function<void(Args...)>> handlers;

	void operator()(Args... args) {
		for (const auto& handler : handlers) {
			handler(args...);
		}
	}

	void operator+=(std::function<void(Args...)> handler) {
		handlers.push_back(handler);
	}

};

nanogui::Screen* screen = nullptr;
struct program* that = nullptr;

struct program : public nanogui::Screen {

	EventDelegate<int> onKeyPressEvent{};
	EventDelegate<int> onFinished{};

	GLFWwindow* window;
	unsigned int texture;
	unsigned int framebuffer;
	std::shared_ptr<RenderThread> renderThread;
	std::shared_ptr<World> world;
	std::shared_ptr<Instance> subject;
	std::shared_ptr<Instance> box;
	std::shared_ptr<ImageTexture> image;
	std::shared_ptr<Material> active_material;
	std::shared_ptr<Grid> reactor_grid;
	std::shared_ptr<Grid> box_grid;

	Point3D camera_position = { 0, -10, 26 };

	bool redraw = false;
	bool show_params = true;
	bool auto_save = false;

	int width = 640;
	int height = 480;
	float scale_factor = 1.0f;

	nanogui::Color glow_color;
	nanogui::Vector3f light0;
	nanogui::Vector3f light1;

	void onKeyPress(int key) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			show_params = !show_params;
			return;

		default:
			onKeyPressEvent(key);
		}
		//redraw = true;
	}

	void load_attribute_image(std::string filename) {
		debug_log.info("Loading attribute image: '", filename, "'");
		image = std::make_shared<ImageTexture>(std::make_shared<Image>(filename));
	}

	std::shared_ptr<Grid> load_mesh_data(std::string filename) {
		debug_log.info("Loading mesh: '", filename, "'");
		auto grid = std::make_shared<Grid>(new Mesh());
		grid->read_smooth_triangles(const_cast<char*>(filename.c_str()));
		grid->setup_cells();
		return grid;
	}

	void load_emissive_material() {
		debug_log.info("Loading emissive material");
		auto material = std::make_shared<Emissive>(reactor_grid->mesh_ptr);
		active_material = material;
		subject->set_material(active_material.get());

		onKeyPressEvent.handlers.clear();
		onKeyPressEvent += [material](int key) {
			if (key == GLFW_KEY_EQUAL) {
				// material->r(material->r() * 1.20);
			}
			if (key == GLFW_KEY_MINUS) {
				//material->r(material->r() * 0.80f);
			}
		};
	}

	void load_highlights_material() {
		debug_log.info("Loading highlights material");
		auto material = std::make_shared<svHighlightsMaterial>();
		material->set_attribute_image(image);
		active_material = material;
		subject->set_material(active_material.get());

		onKeyPressEvent.handlers.clear();
		onKeyPressEvent += [material](int key) {
			if (key == GLFW_KEY_EQUAL) {
				material->s(material->s() + 1.0f);
			}
			if (key == GLFW_KEY_MINUS) {
				material->s(material->s() - 1.0f);
			}
		};
	}

	void focus_subject() {
		debug_log.info("Focusing subject");
		subject->compute_bounding_box();
		auto bbox = subject->get_bounding_box();

		// find half-extends
		double dx = std::abs(bbox.x1 - bbox.x0) / 2.0;
		double dy = std::abs(bbox.y1 - bbox.y0) / 2.0;
		double dz = std::abs(bbox.z1 - bbox.z0) / 2.0;

		scale_factor = std::max({ dx, dy, dz }) / 10.0f;

		auto z_min = std::min(bbox.z0, bbox.z1);

		// move the center of the subject to origin
		double ox = -bbox.x0 - dx;
		double oy = -bbox.y0 - dy;
		double oz = -bbox.z0 - dz;
		//subject->translate(ox, 0.0f, oz);

		camera_position = params.camera_position;
		look_at_subject();
	}

	void look_at_subject() const {
		debug_log.info(camera_position);
		world->camera_ptr->set_eye(camera_position);
		world->camera_ptr->set_lookat(params.target);
		world->camera_ptr->compute_uvw();
	}

	std::string create_filename(std::chrono::milliseconds render_time = 0ms) {

		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d.%H.%M.%S");
		ss << ".png";

		return ss.str();
	}

	void save_as_png() {
		auto filename = create_filename();
		auto pixels = renderThread->pixel_data(false);
		int stride = 3;
		stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), width * stride);

		std::fstream fh;

		auto data_filename = filename + ".txt";
		fh.open(data_filename.c_str(), std::fstream::out);
		if (fh.is_open())
		{
			fh << params;
		}

	}

	void reload_render_params() {
		std::fstream fh;
		fh.open(".settings", std::fstream::in | std::fstream::binary);
		if (fh.is_open())
		{
			fh.read((char*)&params, sizeof(params));
		}
	}

	void save_render_params() {
		std::fstream fh;
		fh.open(".settings", std::fstream::out | std::fstream::binary);
		if (fh.is_open())
		{
			fh.write((char*)&params, sizeof(params));
		}
	}

	program(const char* mesh_filename, const char* mesh2_filename) {

		that = this; //todo hack

		// GLFW init

		if (!glfwInit()) {

			debug_log.error("GLFW Initialization failed");
			exit(1);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		window = glfwCreateWindow(width, height, "", NULL, NULL);
		if (!window) {
			debug_log.error("Window or OpenGL context creation failed");
			exit(1);
		}
		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, this);
		gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		glfwSwapInterval(1);

		glfwGetFramebufferSize(window, &width, &height);

		initialize(window, false);

		// Ray tracing framework init

		reactor_grid = load_mesh_data(mesh_filename);
		box_grid = load_mesh_data(mesh2_filename);

		world = std::make_shared<World>();
		world->build();
		subject = std::make_shared<Instance>(reactor_grid.get());
		box = std::make_shared<Instance>(box_grid.get());
		world->objects.clear();
		world->vp.hres = width;
		world->vp.vres = height;
		world->add_object(subject.get());

		for (auto&& light : params.emitter_locations)
		{
			auto directional = new ParticleEmitter(light, params.glow_colour);
			directional->set_intensity(params.intensity);
			world->add_light(directional);
		}

		world->add_object(box.get());

		focus_subject();

		auto material = std::make_shared<Matte>();
		box->set_material(material.get());
		subject->set_material(material.get());

		load_emissive_material();

		onFinished += [&](auto status) {
			if (auto_save) { save_as_png(); }
			if (params.batch) { exit(0); }
		};

		renderThread = std::make_shared<RenderThread>(world, [&]() {onFinished(0); });

		// gui init

		using namespace nanogui;
		bool enabled = true;
		FormHelper* gui = new FormHelper(this);
		nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Render Settings");

		gui->addGroup("Parameters");
		gui->addVariable("absorption", params.a);
		gui->addVariable("scattering", params.s);
		gui->addVariable("transmittance", params.extinction);
		auto intensity_widget = gui->addVariable("brightness", params.intensity);
		intensity_widget->setCallback([&](float value)
			{
				params.intensity = value;
				for (auto&& light : world->lights)
				{
					auto emitter = dynamic_cast<ParticleEmitter*>(light);
					if (emitter) {
						emitter->set_intensity(params.intensity);
					}
				}
			});

		auto falloff_widget = gui->addVariable("falloff", params.falloff);
		falloff_widget->setCallback([&](float value)
			{
				params.falloff = value;
				for (auto&& light : world->lights)
				{
					auto emitter = dynamic_cast<ParticleEmitter*>(light);
					if (emitter) {
						emitter->set_falloff(params.falloff);
					}
				}
			});

		glow_color = nanogui::Color{ params.glow_colour.r, params.glow_colour.g, params.glow_colour.b, 1.0f };
		auto colour_widget = gui->addVariable("colour", glow_color);
		colour_widget->setFinalCallback([&](auto changed) {
			params.glow_colour = RGBColor{ glow_color.x(), glow_color.y(), glow_color.z() };
			for (auto&& light : world->lights)
			{
				auto emitter = dynamic_cast<ParticleEmitter*>(light);
				if (emitter) {
					emitter->set_color(params.glow_colour);
				}
			}
			});

		gui->addGroup("Samples");
		gui->addVariable("steps:0", params.parameters[0].primary_samples);
		gui->addVariable("scattering:0", params.parameters[0].secondary_samples);
		gui->addVariable("irradiance:0", params.parameters[0].irradiance_samples);

		gui->addVariable("steps:1", params.parameters[1].primary_samples);
		gui->addVariable("scattering:1", params.parameters[1].secondary_samples);
		gui->addVariable("irradiance:1", params.parameters[1].irradiance_samples);

		gui->addVariable("steps:2", params.parameters[2].primary_samples);
		gui->addVariable("scattering:2", params.parameters[2].secondary_samples);


		FormHelper* gui3 = new FormHelper(this);
		nanogui::ref<Window> nanoguiWindow3 = gui3->addWindow(Eigen::Vector2i(10, 10), "Light Settings");

		gui3->addGroup("Gather");
		gui3->addVariable("surface:0", params.gather_surface);
		gui3->addVariable("absorption:0", params.parameters[0].gather_absorption);
		gui3->addVariable("scattering:0", params.parameters[0].gather_scattering);

		gui3->addVariable("absorption:1", params.parameters[1].gather_absorption);
		gui3->addVariable("scattering:1", params.parameters[1].gather_scattering);

		gui3->addVariable("absorption:2", params.parameters[2].gather_absorption);
		gui3->addVariable("scattering:2", params.parameters[2].gather_scattering);

		gui3->addGroup("Commands");
		gui3->addButton("Refresh", [&]() { redraw = true; });
		gui3->addVariable("Auto Save", auto_save);
		gui3->addButton("Save", [&]() { save_as_png(); });
		gui3->addButton("Reset Params", [&]() { params = {}; gui3->refresh(); gui->refresh(); });

		this->setVisible(true);
		this->performLayout();

		nanoguiWindow->setPosition(Eigen::Vector2i(10, 10));
		nanoguiWindow3->setPosition(Eigen::Vector2i(width - 10 - nanoguiWindow3->width(), 10));

		::screen = this; // todo: hack around limitation

		glfwSetCursorPosCallback(window,
			[](GLFWwindow*, double x, double y) {
				::screen->cursorPosCallbackEvent(x, y);
			}
		);

		glfwSetMouseButtonCallback(window,
			[](GLFWwindow*, int button, int action, int modifiers) {
				::screen->mouseButtonCallbackEvent(button, action, modifiers);
			}
		);

		glfwSetKeyCallback(window,
			[](GLFWwindow*, int key, int scancode, int action, int mods) {
				::screen->keyCallbackEvent(key, scancode, action, mods);
				if (action == GLFW_PRESS)that->onKeyPress(key);
			}
		);

		glfwSetCharCallback(window,
			[](GLFWwindow*, unsigned int codepoint) {
				::screen->charCallbackEvent(codepoint);
			}
		);

		glfwSetDropCallback(window,
			[](GLFWwindow*, int count, const char** filenames) {
				::screen->dropCallbackEvent(count, filenames);
			}
		);

		glfwSetScrollCallback(window,
			[](GLFWwindow*, double x, double y) {
				::screen->scrollCallbackEvent(x, y);
			}
		);

		glfwSetFramebufferSizeCallback(window,
			[](GLFWwindow*, int width, int height) {
				::screen->resizeCallbackEvent(width, height);
			}
		);

		// setup render target & texture

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr
		);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		while (!glfwWindowShouldClose(window)) {
			update();
			render();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void update(float time = 0) {
		if (redraw) {
			save_render_params();
			glfwGetFramebufferSize(window, &width, &height);
			world->vp.hres = width;
			world->vp.vres = height;

			world->camera_ptr->CANCEL_THREAD.clear();
			if (renderThread)renderThread->join(); // ignore current solution...
			world->camera_ptr->CANCEL_THREAD.test_and_set();

			renderThread = std::make_shared<RenderThread>(world, [&]() {onFinished(0); });
			redraw = false;
		}
	}

	void render() {

		if (renderThread) {
			auto pixels = renderThread->pixel_data(false);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB8,
				renderThread->width,
				renderThread->height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				pixels.data());
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, height, width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		}

		if (show_params) {
			drawContents();
			drawWidgets();
		}
	}

	~program() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

#endif //WXRAYTRACER_GLSL_HPP
