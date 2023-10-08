#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
typedef struct allshape {
	GLfloat x, y, size;			// �߽� ��ǥ, ���콺 Ŭ��
	GLfloat allvertex[4][3];	// ��ġ
	GLfloat allcolor[4][3];		// ����
	GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
	int dx, dy, touchnum;		// ���� ���� , ���� ���� Ƚ��
	float rad;					// 0 �̶� 180�� ���Ʒ�
	GLuint vao;
	GLuint vbo[2];
	float chy;
	//glm::vec3 mat_size;
}shape;
shape rect[4];

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID; //--- ���̴� ���α׷�
void make_shaderProgram();
GLvoid drawScene();
void bounce();
void zizzag();
void quadric_spiral();
void circle_spiral();
GLvoid Reshape(int w, int h);
//void InitBuffer();
void InitBuffer(shape* s);

float randomnum(float a, float b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(a, b);
	return dis(gen);
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");					//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);			//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);								//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
//--- �����׸�Ʈ ���̴� ��ü �����
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");					//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);			//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);		//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);								//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
//--- ���� �����ϰ� ������ �޾ƿ���
GLuint vao, vbo[2];

void make_shaderProgram()
{

	make_vertexShaders();
	make_fragmentShaders();

	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
	//return shaderProgramID;

}
int n = 0;
bool a = false;
bool b = false;

void InitBuffer(shape* s) {
	glGenVertexArrays(1, &s->vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(s->vao);		//--- VAO�� ���ε��ϱ�
	glGenBuffers(2, s->vbo);		//--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, s->vbo[0]);											//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allvertex, GL_STATIC_DRAW);	//--- triShape �迭�� ������: 9 * float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glEnableVertexAttribArray(0);													//--- attribute �ε��� 0���� ��밡���ϰ� ��

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, s->vbo[1]);  										//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allcolor, GL_STATIC_DRAW); 	//--- colors �迭�� ������: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glEnableVertexAttribArray(1);
}
void setline(int i) {
	rect[i].allvertex[0][0] =  - rect[i].size;
	rect[i].allvertex[0][1] =  - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] =  rect[i].size;
	rect[i].allvertex[1][1] =  - rect[i].size / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = 0.0f;
	rect[i].allvertex[2][1] = rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = - rect[i].size;
	rect[i].allvertex[3][1] = - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;

}
void settriangle(int i) {
	rect[i].allvertex[0][0] =  - rect[i].size / 2;
	rect[i].allvertex[0][1] =  - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].size / 2;
	rect[i].allvertex[1][1] =  - rect[i].size / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = 0.0f;
	rect[i].allvertex[2][1] =  rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] =  - rect[i].size;
	rect[i].allvertex[3][1] = - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;

}

void changet(float mx, float my) {
	int ct = 0;
	if (mx > 0)  ct += 1;
	if (my < 0)  ct += 2;

	rect[ct].x = mx;
	rect[ct].y = my;

	//std::cout << "rect["<< ct << "]: (" << rect[ct].x << ", " << rect[ct].y << ")"<< std::endl;

	rect[ct].size = randomnum(0.1, 0.2);
	rect[ct].dx = rect[ct].dx = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;
	rect[ct].dy = rect[ct].dy = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;

	GLfloat randomColor[3] = { randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) };
	for (int a = 0; a < 4; ++a) {

		for (int b = 0; b < 3; ++b) {
			rect[ct].allcolor[a][b] = randomColor[b];
		}

	}

	if (rect[ct].choice == 0) {
		settriangle(ct);
	}
	else {
		setline(ct);
	}
	InitBuffer(&rect[ct]);
}

void Mouse(int button, int state, int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		changet(mx, my); // �߰�
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}
bool start = false;
bool boolai = false;
int caset{0};

void bounce()
{
	for (int i = 0; i < 4; ++i) {
		rect[i].x += rect[i].dx * 0.01f;
		rect[i].y += rect[i].dy * 0.01f;
		std::cout << "timer[" << i << "]: (" << rect[i].dx << ", " << rect[i].x << ")" << std::endl;

		if ((rect[i].dx < 0 and rect[i].x - rect[i].size <= -1) || (rect[i].dx > 0 and rect[i].x + rect[i].size >= 1)) {
			rect[i].dx *= -1;
			rect[i].x += rect[i].dx * 0.01f;
		}
		if ((rect[i].dy < 0 and rect[i].y - rect[i].size <= -1) || (rect[i].dy > 0 and rect[i].y + rect[i].size >= 1)) {
			rect[i].dy *= -1;
			rect[i].y += rect[i].dy * 0.01f;
		}
		std::cout << rect[i].x << '\n';
		//settriangle(i);
	}
}
void zizzag()
{
	for (int i = 0; i < 4; ++i) {

		if (((int)rect[i].rad / 90) % 2 == 0) { // 0 �̶� 180�� ���Ʒ�

			rect[i].y += rect[i].dy * 0.01f;
			rect[i].chy += 0.01f;

			if (rect[i].chy > rect[i].size * 2) {

				rect[i].rad = 180.0f + rect[i].dx * 90.0f;
			}
			else if ((rect[i].dy < 0 and rect[i].y - rect[i].size <= -1) || (rect[i].dy > 0 and rect[i].y + rect[i].size >= 1)) {
				rect[i].rad = 180.0f + rect[i].dx * 90.0f;
				rect[i].dy *= -1;
			}
		}
		else {

			rect[i].x += rect[i].dx * 0.01f;

			if ((rect[i].dx < 0 and rect[i].x - rect[i].size <= -1) || (rect[i].dx > 0 and rect[i].x + rect[i].size >= 1)) {
				rect[i].dx *= -1;
				rect[i].rad = 90.0f - rect[i].dy * 90.0f; // �Ʒ��� �ٶ󺸵��� ���� ����
				rect[i].chy = 0.0f;
			}

		}
		std::cout << i << "���� ��ǥ : " << rect[i].x << ", " << rect[i].y << '\n';


	}
}
void quadric_spiral()
{
	for (int i = 0; i < 4; ++i) {

		if (((int)rect[i].rad / 90) % 2 == 0) { // 0 �̶� 180�� ���Ʒ�

			rect[i].y += rect[i].dy * 0.01f;

			// y�� ���� ������ 90, 270 ��, ��� ���� ����
			if ((rect[i].dy < 0 and rect[i].y - rect[i].size <= -1) || (rect[i].dy > 0 and rect[i].y + rect[i].size >= 1)) {
				rect[i].touchnum += 1;
				rect[i].rad = 180.0f + rect[i].dx * 90.0f; //�¿�
				rect[i].dy *= -1;
			}

			else if (rect[i].touchnum > 3) {
				if ((rect[i].dy < 0 and rect[i].y < (-1 + rect[i].size * rect[i].touchnum / 2)) || (rect[i].dy > 0 and rect[i].y >(1 - rect[i].size * rect[i].touchnum / 2))) {
					rect[i].touchnum += 1;
					rect[i].rad = 180.0f + rect[i].dx * 90.0f; //�¿�
					rect[i].dy *= -1;
				}
				if ((-1 + rect[i].size * rect[i].touchnum / 2) > 0.99f) {
					rect[i].touchnum = 0;
				}
			}
		}
		else { // �� �� ��

			rect[i].x += rect[i].dx * 0.01f;

			// x�� ���� ������ 0, 180 ��, �Ϸ�  ���� ����
			if ((rect[i].dx < 0 and rect[i].x - rect[i].size <= -1) || (rect[i].dx > 0 and rect[i].x + rect[i].size >= 1)) {
				rect[i].touchnum += 1;						// ���� ����
				rect[i].rad = 90.0f - rect[i].dy * 90.0f;	// �� ��
				rect[i].dx *= -1;
			}

			else if (rect[i].touchnum > 4) {
				if ((rect[i].dx < 0 and rect[i].x < (-1 + rect[i].size * rect[i].touchnum / 2)) || (rect[i].dx > 0 and rect[i].x >(1 - rect[i].size * rect[i].touchnum / 2))) {
					rect[i].touchnum += 1;
					rect[i].rad = 90.0f - rect[i].dy * 90.0f; //�� ��
					rect[i].dx *= -1;
				}
				if ((-1 + rect[i].size * rect[i].touchnum / 2) > 0.99f) {
					rect[i].touchnum = 0;
				}
			}

		}
	}
}
void circle_spiral()
{
	for (int i = 0; i < 4; ++i) {

		rect[i].x = cos(glm::radians(rect[i].rad)) * rect[i].chy;
		rect[i].y = sin(glm::radians(rect[i].rad)) * rect[i].chy;

		rect[i].chy += 0.01f * (i + 1);
		rect[i].rad += 7.0f * (i + 1);
		if (rect[i].chy > 1) {
			rect[i].chy = 0;
		}
		if (rect[i].rad > 360) {
			rect[i].rad -= 360;
		}

	}
}
void TimerA(int value) {

	if (start) {
		if (caset == 1) {
			bounce();
		}
		else if (caset == 2) {
			zizzag();
		}
		else if (caset == 3) {
			quadric_spiral();
		}
		else if (caset == 4) {
			circle_spiral();
		}
	}
	glutPostRedisplay();
	glutTimerFunc(30, TimerA, 1);
}



GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
		//1. ƨ���
	case 'a':case 'A':
		//caset = caset == 2 ? 0 : 2;
		//start = caset == 2 ? true : false;
		if (caset != 1) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			caset = 1;
		}
		else {
			caset = 0;
			start = false;
		}
		break;

		// 2.�¿�� �������
	case 'b':case 'B':
		//caset = caset == 2 ? 0 : 2;
		//start = caset == 2 ? true : false;
		if (caset != 2) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			for (int i = 0; i < 4; ++i) {
				rect[i].rad = 180.0f + rect[i].dx * 90.0f; //90 or 270
			}
			caset = 2;
		}
		else {
			caset = 0;
			start = false;
		}
		break;

		// 3. �簢 �����̷�
	case 'c':case 'C':
		//caset = caset == 3 ? 0 : 3;			
		//start = caset == 3 ? true : false;	
		if (caset != 3) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			for (int i = 0; i < 4; ++i) {
				rect[i].y = 1.0f;
				rect[i].rad = 0.0f + rect[i].dx * 180.0f; // 0 or 180
			}
			caset = 3;
		}
		else {
			caset = 0;
			start = false;
		}
	
		break;
		// �� �����̷�
	case 'd':case 'D':
		//caset = caset == 4 ? 0 : 4;			// �������ΰ� ���(4��)�� �ƴҽ� ������� �ٲ�
		//start = caset == 4 ? true : false;	// ���� ����� ���� �ϸ� start�� ����
		if (caset != 4) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			for (int i = 0; i < 4; ++i) {
				rect[i].chy = (pow(rect[i].x, 2) + pow(rect[i].y, 2)); // r��
			}
			caset = 4;
		}
		else {
			caset = 0;
			start = false;
		}
		break;

	case 'q':case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

// -- - �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- ����� ���� ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- ������ ���������ο� ���̴� �ҷ�����
	
	for (int i = 0; i < 4; ++i) {
		/*InitBuffer(&rect[i]);*/
		glBindVertexArray(rect[i].vao);								//--- ����� VAO �ҷ�����
		{
			glm::mat4 transformMatrix(1.0f);
			//std::cout << "��� �� rect[" << i << "]: (" << rect[i].x << ", " << rect[i].y << ")" << std::endl;
			//transformMatrix = glm::scale(transformMatrix, rect[i].mat_size);
			// 
			//srt ����
			transformMatrix = glm::translate(transformMatrix, glm::vec3(rect[i].x, rect[i].y, 0.0));
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rect[i].rad), glm::vec3(0.0, 0.0, 1.0));

			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

		}
		if (rect[i].choice == 0) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else {
			glLineWidth(2);
			glDrawArrays(GL_LINE_STRIP, 0, 4);

		}
		//std::cout << "��� �� rect[" << i << "]: (" << rect[i].x << ", " << rect[i].y << ")" << std::endl;
		//{
		//	GLint bufferSize = 0;
		//	glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[0]);
		//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		//	// ���� ���ε��� VBO�� ũ�⸦ ������
		//	GLfloat* data = new GLfloat[bufferSize / sizeof(GLfloat)];
		//	glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, data);
		//	// VBO�� �����͸� �о��
		//	for (int i = 0; i < bufferSize / sizeof(GLfloat); i++) {
		//		std::cout << "Data[" << i << "]: " << data[i] << std::endl;
		//	}
		//	delete[] data;
		//	// ������ ��� �� �޸𸮸� ����
		//}
		
	}
	{
		glm::mat4 transformMatrix(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�
	}
	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
}
void reset() {

	for (int i = 0; i < 4; ++i) {

		float x = i % 2 == 0 ? -0.5 : 0.5;
		float y = i / 2 == 0 ? -0.5 : 0.5;
		
		//���λ��� ���� �ʱ�ȭ
		//std::cout << rect[i].x << rect[i].y << std::endl;
		//rect[i].mat_size.x= 0.5f; 
		//rect[i].mat_size.y= 1.0f;
		//rect[i].mat_size.z= 0.0f;

		rect[i].rad = 0.0f;
		rect[i].chy = 0.0f; // ��ȭ�� ���
		rect[i].touchnum = 0;

		changet(x, y);
		std::cout << "rect[" << i << "]: (" << rect[i].dx << ", " << rect[i].dy << ")" << std::endl;

	}
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Example9");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	std::cout << " GLEW �ʱ�ȭ �Ϸ�" << '\n';

	make_shaderProgram();

	
	reset();
	std::cout << " reset �ʱ�ȭ �Ϸ�" << '\n';

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է�
	glutMouseFunc(Mouse); // ���콺 �Է�

	glutMainLoop();
}