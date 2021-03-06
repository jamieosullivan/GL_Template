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
	void init(const std::string& meshPath, const std::vector<std::string>& texturesPaths, int materialId, bool centerAndUnit = false);
	
	/// Update function
	void update(const glm::mat4& model);
	
	/// Draw function
	void draw(const glm::mat4& view, const glm::mat4& projection);
	
	/// Draw depth function
	void drawDepth(const glm::mat4& lightVP);
	
	/// Clean function
	void clean();


private:
	
	GLuint _programId;
	GLuint _programDepthId;
	GLuint _vao;
	GLuint _ebo;
	GLuint _texColor;
	GLuint _texNormal;
	GLuint _texEffects;
	GLuint _mvpId;
	GLuint _mvpDepthId;
	GLuint _mvId;
	GLuint _normalMatrixId;
	GLuint _pId;
	
	GLsizei _count;
	
	glm::mat4 _model;
	
};

#endif
