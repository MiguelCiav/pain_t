#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <string>
#include <iostream>

struct color 
{
	float r, g, b;
	color() : r(0), g(0), b(0) {};
	color(float r, float g, float b) : r(r), g(g), b(b) {};
};

class engine_2d
{
private: 
	GLFWwindow* window;
	std::string title;
	std::vector<color> pixel_buffer;
	GLuint texture_id, vao,vbo, ebo, shader_program, vs, fs;
	void init();
	void setup_canvas();
	void upload_texture();
	//Manejo de eventos
	bool key_state[GLFW_KEY_LAST];
	bool mouse_button_state[GLFW_MOUSE_BUTTON_LAST];
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
protected:
	int width, height;
	void put_pixel(int x, int y, const color& c);
	void clear(const color& c);
	glm::vec2 get_mouse_position();
	bool is_key_pressed(int key) const;
	bool is_mouse_button_pressed(int button) const;
public:
	engine_2d(int width, int height, const std::string& title);
	~engine_2d();
	void run();
	//API
	virtual void on_key_down(int key) {};
	virtual void on_key_up(int key) {};
	virtual void on_mouse_button_down(int button, double x, double y) {};
	virtual void on_mouse_button_up(int button, double x, double y) {};
	virtual void on_mouse_move(double x, double y) {};
	virtual void setup() {};
	virtual void update(float deltaTime) {};
	virtual void draw_ui() {};
private:
	const char* vertex_shader_source = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoord;
		out vec2 TexCoord;
		void main() {
			gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
			// Invertimos el eje Y de la textura para que el (0,0) esté arriba a la izquierda
			TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y); 
		}
	)";
	const char* fragment_shader_source = R"(
		#version 330 core
		out vec4 Fragcolor;
		in vec2 TexCoord;
		uniform sampler2D screenTexture;
		void main() {
			Fragcolor = texture(screenTexture, TexCoord);
		}
	)";
};
