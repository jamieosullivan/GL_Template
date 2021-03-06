#include <stdio.h>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "DirectionalLight.h"



DirectionalLight::DirectionalLight(const glm::vec3& worldPosition, const glm::vec3& color, const glm::mat4& projection) : Light(worldPosition, color, projection) {
	
	
}


void DirectionalLight::init(const std::map<std::string, GLuint>& textureIds){
	// Setup the framebuffer.
	_shadowPass = std::make_shared<Framebuffer>(512, 512, GL_RG,GL_FLOAT, GL_RG16F, GL_LINEAR,GL_CLAMP_TO_BORDER);
	_blurPass = std::make_shared<Framebuffer>(_shadowPass->_width, _shadowPass->_height, GL_RG,GL_FLOAT, GL_RG16F, GL_LINEAR,GL_CLAMP_TO_BORDER);
	_blurScreen.init(_shadowPass->textureId(), "ressources/shaders/screens/boxblur");
	
	std::map<std::string, GLuint> textures = textureIds;
	textures["shadowMap"] = _blurPass->textureId();
	_screenquad.init(textures, "ressources/shaders/lights/directional_light");
	
	glUseProgram(_screenquad.program());
	_vtolID  = glGetUniformLocation(_screenquad.program(), "viewToLight");
	_lightPosId = glGetUniformLocation(_screenquad.program(), "lightDirection");
	_lightColId = glGetUniformLocation(_screenquad.program(), "lightColor");
	_projId = glGetUniformLocation(_screenquad.program(), "projectionMatrix");
	glUseProgram(0);

}

void DirectionalLight::draw(const glm::vec2& invScreenSize, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix){
	
	
	glm::mat4 viewToLight = _mvp * glm::inverse(viewMatrix);
	// Store the four variable coefficients of the projection matrix.
	glm::vec4 projectionVector = glm::vec4(projectionMatrix[0][0], projectionMatrix[1][1], projectionMatrix[2][2], projectionMatrix[3][2]);
	glm::vec3 lightPositionViewSpace = glm::vec3(viewMatrix * glm::vec4(_local, 0.0));
	
	glUseProgram(_screenquad.program());
	
	glUniform3fv(_lightPosId, 1,  &lightPositionViewSpace[0]);
	glUniform3fv(_lightColId, 1,  &_color[0]);
	// Projection parameter for position reconstruction.
	glUniform4fv(_projId, 1, &(projectionVector[0]));
	glUniformMatrix4fv(_vtolID, 1, GL_FALSE, &viewToLight[0][0]);

	_screenquad.draw(invScreenSize);

}

void DirectionalLight::bind(){
	_shadowPass->bind();
	glViewport(0, 0, _shadowPass->_width, _shadowPass->_height);
	
	// Set the clear color to white.
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DirectionalLight::blurAndUnbind(){
	// Unbind the shadow map framebuffer.
	_shadowPass->unbind();
	// ----------------------
	
	// --- Blur pass --------
	glDisable(GL_DEPTH_TEST);
	// Bind the post-processing framebuffer.
	_blurPass->bind();
	// Set screen viewport.
	glViewport(0,0,_blurPass->_width, _blurPass->_height);
	// Draw the fullscreen quad
	_blurScreen.draw( glm::vec2(0.0f) );
	 
	_blurPass->unbind();
	glEnable(GL_DEPTH_TEST);
}

void DirectionalLight::clean(){
	_blurPass->clean();
	_blurScreen.clean();
	_shadowPass->clean();
}


