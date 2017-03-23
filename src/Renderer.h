#ifndef Renderer_h
#define Renderer_h
#include <gl3w/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Framebuffer.h"
#include "camera/Camera.h"
#include "Object.h"
#include "Skybox.h"
#include "ScreenQuad.h"
#include "Light.h"



struct Material
{
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
};


class Renderer {

public:

	~Renderer();

	/// Init function
	Renderer(int width, int height);

	/// Draw function
	void draw();

	void physics(float elapsedTime);

	/// Clean function
	void clean();

	/// Handle screen resizing
	void resize(int width, int height);

	/// Handle keyboard inputs
	void keyPressed(int key, int action);

	/// Handle mouse inputs
	void buttonPressed(int button, int action, double x, double y);

	void mousePosition(int x, int y, bool leftPress, bool rightPress);


private:
	
	float _timer;

	GLuint _ubo;
	
	Camera _camera;

	Light _light;
	
	Object _object;
	Skybox _skybox;
	
	std::shared_ptr<Framebuffer> _sceneFramebuffer;
	ScreenQuad _finalScreen;
	
	GLuint _pingpong;
	GLuint _padding;
	
};

#endif
