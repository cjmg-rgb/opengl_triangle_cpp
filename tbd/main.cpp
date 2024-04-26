#include "libs.h"

void setFrameBufferResizaableCallback(GLFWwindow* window, int fbw, int fbh)
{
	glViewport(0, 0, fbw, fbh);
}


void initGLFW();
GLFWwindow* createWindow();
void initGLEW();
void glEnables();
bool loadShaders(GLuint&);
void mainLoop(GLFWwindow*, GLuint&, GLuint&);

Vertex vertices[] =
{
	glm::vec3(0.f, .5f, 0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 1.f),
	glm::vec3(-.5f, -.5f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, 0.f),
	glm::vec3(.5f, -.5f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 0.f)
};
unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[] =
{
	0, 1, 2
};
unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);


int main()
{

	// Initialize GLFW
	initGLFW();

	// Create Window
	GLFWwindow* window = createWindow();

	// Initialize Glew
	initGLEW();

	// Enables
	glEnables();

	// SHADERS
	GLuint core_program;
	if(!loadShaders(core_program))
		std::cout << "ERROR::MAIN.CPP::FAILED_TO_LOAD_SHADERS_CORE_PROGRAM\n";
	
	// VAO
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// LOOP
	mainLoop(window, core_program, VAO);

	glfwTerminate();
	glDeleteProgram(core_program);

	return 0;
}

void initGLFW()
{
	if (!glfwInit())
		std::cout << "ERROR::MAIN.CPP::FAILED_TO_INITIALIZE_GLWF\n";
}

GLFWwindow* createWindow()
{
	// 640x480

	const int WINDOW_WIDTH = 640;
	const int WINDOW_HEIGHT = 480;
	int frame_buffer_width = 0;
	int frame_buffer_height = 0;


	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Triangle", NULL, NULL);

	glfwSetFramebufferSizeCallback(window, setFrameBufferResizaableCallback);

	glfwMakeContextCurrent(window);

	return window;
}

void initGLEW()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR::MAIN.CPP::FAILED_TO_INITIALIZE_GLEW\n";
}

void glEnables()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool loadShaders(GLuint& program)
{
	std::string temp = "";
	std::string src = "";
	char infoLog[512];
	GLint success;

	std::ifstream file;

	// Vertex
	file.open("vertex_core.glsl");

	if (file.is_open())
	{
		while (std::getline(file, temp))
			src += temp + "\n";
	}
	else
	{
		std::cout << "ERROR::LOADSHADER::FAILED_TO_OPEN_FILE\n";
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertSrc = src.c_str();
	glShaderSource(vertexShader, 1, &vertSrc, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cout << "ERROR::LOADSHADER::FAILED_TO_COMPILE_VERTEX_SHADER\n";
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << infoLog << "\n";
	}

	file.close();

	temp = "";
	src = "";

	// Fragment
	file.open("fragment_core.glsl");
	if (file.is_open())
	{
		while (std::getline(file, temp))
			src += temp + "\n";
	}
	else
	{
		std::cout << "ERROR::LOADSHADER::FAILED_TO_OPEN_FILE\n";
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragSrc = src.c_str();
	glShaderSource(fragmentShader, 1, &fragSrc, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cout << "ERROR::LOADSHADER::FAILED_TO_COMPILE_FRAGMENT_SHADER\n";
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << infoLog << "\n";
	}

	// Program
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// END
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(0);

	return success;
}

void mainLoop(GLFWwindow* window, GLuint& program, GLuint& VAO)
{

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glUseProgram(program);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, nrOfIndices);

		glfwSwapBuffers(window);
		glFlush();
	}
}