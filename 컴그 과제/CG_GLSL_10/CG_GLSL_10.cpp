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
	GLfloat x, y, drawr;			// �߽� ��ǥ, ���콺 Ŭ��
	int drawpoint;
	int pointnum;
	GLuint vao;
	GLuint vbo[2];

	//float chy;
	//glm::vec3 mat_size;
	//GLfloat allvertex[4][3];	// ��ġ
	//GLfloat allcolor[4][3];		// ����
	//GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
	//int dx, dy, touchnum;		// ���� ���� , ���� ���� Ƚ��				// 0 �̶� 180�� ���Ʒ�
}shape;

//--- �������� ----
shape rect[5];
int choice(0);		//�׸� ���� ���
bool start = false; // Ÿ�̸� ���� ����
int n = 0;			// ����� ���� ����


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
void TimerA(int value);

GLvoid Reshape(int w, int h);
//void InitBuffer();
void InitBuffer(shape& s);

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

void InitBuffer(shape& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;
	float drawr = 0;
	float rad = 0;
	while(rad < 360.0 * 4)
	{
		vertex.push_back(s.x + cos(glm::radians(rad)) *  drawr);	//x
		vertex.push_back(s.y + sin(glm::radians(rad)) *  drawr);	//y
		vertex.push_back(0.0);	//z

		color.push_back(randomnum(0.0, 1.0));		//r
		color.push_back(randomnum(0.0, 1.0));		//g
		color.push_back(randomnum(0.0, 1.0));		//b

		drawr += 0.002;
		rad += 15.0;
	}
	float max_r = drawr;
	rad += 180.0f;
	while (rad > 180.0)
	{
		vertex.push_back(s.x + 2 * max_r + cos(glm::radians(rad)) * drawr);	//x
		vertex.push_back(s.y + sin(glm::radians(rad)) * drawr);	//y
		vertex.push_back(0.0);	//z

		color.push_back(randomnum(0.0, 1.0));		//r
		color.push_back(randomnum(0.0, 1.0));		//g
		color.push_back(randomnum(0.0, 1.0));		//b

		drawr -= 0.002;
		rad -= 15.0;
	}


	//gpu ���ۿ� �����ϱ�
	{
		glGenVertexArrays(1, &s.vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(s.vao);		//--- VAO�� ���ε��ϱ�
		glGenBuffers(2, s.vbo);		//--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[0]);											//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//--- triShape �迭�� ������: 9 * float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
		glEnableVertexAttribArray(0);													//--- attribute �ε��� 0���� ��밡���ϰ� ��

		//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[1]);  										//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW); 	//--- colors �迭�� ������: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
		glEnableVertexAttribArray(1);
	}
	s.drawpoint = 1;
	s.pointnum = vertex.size() / 3 ;

	vertex.clear();
	color.clear();
}
/*
void setline(int i) {
	rect[i].allvertex[0][0] = -rect[i].drawr;
	rect[i].allvertex[0][1] = -rect[i].drawr / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].drawr;
	rect[i].allvertex[1][1] = -rect[i].drawr / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = 0.0f;
	rect[i].allvertex[2][1] = rect[i].drawr;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = -rect[i].drawr;
	rect[i].allvertex[3][1] = -rect[i].drawr / 2;
	rect[i].allvertex[3][2] = 0.0f;

}
void settriangle(int i) {
	rect[i].allvertex[0][0] = -rect[i].drawr / 2;
	rect[i].allvertex[0][1] = -rect[i].drawr / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].drawr / 2;
	rect[i].allvertex[1][1] = -rect[i].drawr / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = 0.0f;
	rect[i].allvertex[2][1] = rect[i].drawr;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = -rect[i].drawr;
	rect[i].allvertex[3][1] = -rect[i].drawr / 2;
	rect[i].allvertex[3][2] = 0.0f;

}*/

//void changet(float mx, float my) {
//	int ct = 0;
//	if (mx > 0)  ct += 1;
//	if (my < 0)  ct += 2;
//
//	rect[ct].x = mx;
//	rect[ct].y = my;
//
//	//std::cout << "rect["<< ct << "]: (" << rect[ct].x << ", " << rect[ct].y << ")"<< std::endl;
//
//	rect[ct].drawr = randomnum(0.1, 0.2);
//	rect[ct].dx = rect[ct].dx = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;
//	rect[ct].dy = rect[ct].dy = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;
//
//	GLfloat randomColor[3] = { randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) };
//	for (int a = 0; a < 4; ++a) {
//
//		for (int b = 0; b < 3; ++b) {
//			rect[ct].allcolor[a][b] = randomColor[b];
//		}
//
//	}
//
//	if (rect[ct].choice == 0) {
//		settriangle(ct);
//	}
//	else {
//		setline(ct);
//	}
//	InitBuffer(&rect[ct]);
////}

void Mouse(int button, int state, int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float r = randomnum(0.0f, 1.0f);
		float g = randomnum(0.0f, 1.0f);
		float b = randomnum(0.0f, 1.0f);

		glClearColor(r, g, b, 1.0f);  // Set random background color
		if (n < 5) {
			rect[n].x = mx;
			rect[n].y = my;
			InitBuffer(rect[n]);
			std::cout << "���콺 Ŭ�� ����, ���� ���� ����:" << n << '\n';
			glutTimerFunc(10, TimerA, n);
			n++;
		}
		//changet(mx, my); // �߰�
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

void TimerA(int value) {
	rect[value].drawpoint++;


	glutPostRedisplay();
	if(rect[value].drawpoint < rect[value].pointnum)
		glutTimerFunc(30, TimerA, value);
}



GLvoid Keyboard(unsigned char key, int x, int y) {

	if ('0' < key and key < '6') {	//���� 1~5
		for (int i = n; i < key - '0'; ++i) {
			rect[n].x = randomnum(-1.0 , 1.0);
			rect[n].y = randomnum(-1.0, 1.0);
			InitBuffer(rect[n]);
			std::cout << "Ű���� ���� Ŭ�� ����, ���� ���� ����:" << n << '\n';
			glutTimerFunc(10, TimerA, n);
			n++;
		}
		n = key - '0';
	}
	else {
		switch (key) {
		case 'p':case 'P':
			choice = 0;
			break;
		case 'l':case 'L':
			choice = 1;
			break;
		case 'q':case 'Q':
			exit(0);
			break;
		}
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

// -- - �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	//glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- ����� ���� ���� 
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);						//--- ������ ���������ο� ���̴� �ҷ�����

	for (int i = 0; i < n; ++i) {

		glBindVertexArray(rect[i].vao);								//--- ����� VAO �ҷ�����
		{
			glm::mat4 transformMatrix(1.0f);


			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

		}
		if (choice == 0) {
			glPointSize(3);
			glDrawArrays(GL_POINTS, 0, rect[i].drawpoint);
		}
		else {
			glLineWidth(2);
			glDrawArrays(GL_LINE_STRIP, 0, rect[i].drawpoint);

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

	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
}
void reset() {
	n = 0;
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

	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- ����� ���� ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);

	reset();
	std::cout << " reset �ʱ�ȭ �Ϸ�" << '\n';

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է�
	glutMouseFunc(Mouse); // ���콺 �Է�

	glutMainLoop();
}