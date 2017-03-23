#ifndef Cube_h
#define Cube_h
#include <gl3w/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



class Skybox {

public:

	Skybox();

	~Skybox();

	/// Init function
	void init();

	/// Draw function
	void draw(float elapsed, const glm::mat4& view, const glm::mat4& projection);

	/// Clean function
	void clean();


private:
	
	GLuint _programId;
	GLuint _vao;
	GLuint _ebo;
	GLuint _texCubeMap;
	

};

#endif
