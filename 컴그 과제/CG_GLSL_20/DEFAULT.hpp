#pragma once

#ifndef DEFAULT_HPP
#define DEFAULT_HPP

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <format>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

const glm::vec3 x_axis{ 1.0f,0.0f,0.0f }; //x축
const glm::vec3 y_axis{ 0.0f,1.0f,0.0f }; //y축
const glm::vec3 z_axis{ 0.0f,0.0f,1.0f }; //z축
const glm::vec3 zero{ 0.0f,0.0f,0.0f }; // 원점

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid handleMouseWheel(int wheel, int direction, int x, int y);
GLvoid specialkeyborad(int key, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Keyboardup(unsigned char key, int x, int y);
#endif