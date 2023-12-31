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
using namespace std;

const glm::vec3 z_axis{ 0.0f,0.0f,1.0f }; //z��

void reset();
void timer(int value);

GLvoid drawScene();
void dddddddddddddddddddddd();
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
void make_shaderProgram();
void basket_timer(int value);

#endif

