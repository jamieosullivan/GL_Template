#ifndef Object_h
#define Object_h
#include <gl3w/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



class Object {

public:

	Object();

	~Object();

	/// Init function
	void init(const std::string& meshPath, const std::vector<std::string>& texturesPaths);

	/// Draw function
	void draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const size_t pingpong);
	
	/// Draw depth function
	//void drawDepth(float elapsed, const glm::mat4& vp);
	
	/// Clean function
	void clean();


private:
	
	GLuint _programId;
	//GLuint _programDepthId;
	GLuint _vao;
	GLuint _ebo;
	GLuint _texColor;
	GLuint _texNormal;
	GLuint _texEffects;
	
	GLuint _lightUniformId;
	
	GLsizei _count;
	
	glm::mat4 _lightMVP;
	
};

#endif
