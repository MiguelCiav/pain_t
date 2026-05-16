#include "engine_2d.h"

engine_2d::engine_2d(int width, int height, const std::string& title) : width(width), height(height), title(title) {
	pixel_buffer.resize(width * height, color(0.0f, 0.0f, 0.0f));
	for (int i = 0; i < GLFW_KEY_LAST; ++i) key_state[i] = false;
	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) mouse_button_state[i] = false;
	init();
	setup_canvas();
}

engine_2d::~engine_2d() {
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(shader_program);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void engine_2d::init() {
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW\n";
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		std::cout << "Failed to create GLFW window\n";
		return;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return;
	}
	glViewport(0, 0, width, height);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	return;
};

void engine_2d::setup_canvas() {
	float vertex[] = {
		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};
	unsigned int index[] = { 0, 1, 3, 1, 2, 3 };

	glGenTextures(1, &texture_id);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	shader_program = glCreateProgram();

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Shaders
	glShaderSource(vs, 1, &vertex_shader_source, nullptr);
	glCompileShader(vs);
	glShaderSource(fs, 1, &fragment_shader_source, nullptr);
	glCompileShader(fs);
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	//Texturas
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	return;
}

void engine_2d::upload_texture() {
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, pixel_buffer.data());
	return;
}

void engine_2d::run() {
	setup();
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		static float lastFrame = 0.0f;
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();
		update(deltaTime);
		upload_texture();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		draw_ui();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	return;
}

void engine_2d::put_pixel(int x, int y, const color& c) {
	if (x < 0 || x >= width || y < 0 || y >= height) return;
	pixel_buffer[y * width + x] = c;
	return;
}

void engine_2d::clear(const color& c) {
	std::fill(pixel_buffer.begin(), pixel_buffer.end(), c);
	return;
}

glm::vec2 engine_2d::get_mouse_position() {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void engine_2d::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key < 0 || key >= GLFW_KEY_LAST) return;
	engine_2d* engine = static_cast<engine_2d*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS) {
		engine->key_state[key] = true;
		engine->on_key_down(key);
	}
	else if (action == GLFW_RELEASE) {
		engine->key_state[key] = false;
		engine->on_key_up(key);
	}
}

void engine_2d::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button < 0 || button >= GLFW_MOUSE_BUTTON_LAST) return;
	engine_2d* engine = static_cast<engine_2d*>(glfwGetWindowUserPointer(window));
	glm::vec2 mousePos = engine->get_mouse_position();
	if (action == GLFW_PRESS) {
		engine->mouse_button_state[button] = true;
		engine->on_mouse_button_down(button, mousePos.x, mousePos.y);
	}
	else if (action == GLFW_RELEASE) {
		engine->mouse_button_state[button] = false;
		engine->on_mouse_button_up(button, mousePos.x, mousePos.y);
	}
}

void engine_2d::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	engine_2d* engine = static_cast<engine_2d*>(glfwGetWindowUserPointer(window));
	if (engine) {
		engine->on_mouse_move(xpos, ypos);
	}
}

bool engine_2d::is_key_pressed(int key) const {
	if (key < 0 || key >= GLFW_KEY_LAST) return false;
	return key_state[key];
}

bool engine_2d::is_mouse_button_pressed(int button) const {
	if (button < 0 || button >= GLFW_MOUSE_BUTTON_LAST) return false;
	return mouse_button_state[button];
}