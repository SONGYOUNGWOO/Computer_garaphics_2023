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

//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
typedef struct allshape {
	GLfloat x, y, size;			// �߽� ��ǥ, ���콺 Ŭ��
	float time,dx,dy;
	int pointnum;
	int indexnum; //index.size()
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;	
	bool canuse;				// ������ ��밡�� ���� (��������)
	std::vector<float> color;
	GLint motions ,down;

	float chy;
	//glm::vec3 mat_size;
	//GLfloat allvertex[4][3];	// ��ġ
	//GLfloat allcolor[4][3];		// ����
	//GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
	//int dx, dy, touchnum;		// ���� ���� , ���� ���� Ƚ��				// 0 �̶� 180�� ���Ʒ�
}shape;

//--- �������� ----
shape rect;
shape tetrahedron;
int choice(0);		//�׸� ���� ���
bool start = false; // Ÿ�̸� ���� ����
int n = 0;			// ����� ���� ����
GLuint vao, vbo[2];
GLfloat degree{ 0.0f };
int target{ 0 };  //������ ����
int target2{ 0 };  //������ ����
bool left_button = false;

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
void TimerB(int value);

GLvoid Reshape(int w, int h);
//void InitBuffer();
void InitBuffer_cube(shape& s);

float randomnum(float a, float b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(a, b);
	return dis(gen);
}

int randomnum(int a, int b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution dis(a, b);
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

//--------------------InitBuffer_cube---------------------------------------
void InitBuffer_cube(shape& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;
	std::vector<unsigned int> index;

	
	//for(int i = 0; i < 8; ++i)
	//{
	//	if (s.pointnum == 1) {
	//		vertex.push_back(0.0f);		//x - cos
	//		vertex.push_back(0.0f);		//y - sin
	//		vertex.push_back(0.0f);		//z
	//	}
	//	
	//	color.push_back(s.color.at(i * 3));		//r
	//	color.push_back(s.color.at(i * 3 + 1));	//g
	//	color.push_back(s.color.at(i * 3 + 2));	//b

	//	//�����
	//	{
	//		std::cout << std::format("���� ���� [{}]: vertex - ({},{},{}),\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2));
	//	}		
	//}
	for (int i = 0; i < 8; ++i) {
		vertex.push_back((i % 4) % 3 == 0 ? -1.0f : 1.0f);	//x - cos
		vertex.push_back(i / 4 == 0 ? 1.0f: -1.0f);		//y - sin
		vertex.push_back((i % 4) < 2 ? 1.0f : -1.0f);		//z

		color.push_back(randomnum(0.0f, 1.0f));
		color.push_back(randomnum(0.0f, 1.0f));
		color.push_back(randomnum(0.0f, 1.0f));
		std::cout << std::format("���� ���� [{}]: vertex - ({},{},{}),\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2));
	}
	//1
	index.push_back(0);
	index.push_back(1);
	index.push_back(2);

	index.push_back(0);
	index.push_back(2);
	index.push_back(3);
	//----2
	index.push_back(0);
	index.push_back(3);
	index.push_back(4);

	index.push_back(3);
	index.push_back(7);
	index.push_back(4);
	//---3
	index.push_back(7);
	index.push_back(6);
	index.push_back(4);

	index.push_back(6);
	index.push_back(5);
	index.push_back(4);
	//--4
	index.push_back(5);
	index.push_back(2);
	index.push_back(1);

	index.push_back(5);
	index.push_back(6);
	index.push_back(2);

	//--5
	index.push_back(4);
	index.push_back(1);
	index.push_back(0);

	index.push_back(4);
	index.push_back(5);
	index.push_back(1);

	//--6
	index.push_back(7);
	index.push_back(3);
	index.push_back(2);

	index.push_back(6);
	index.push_back(7);
	index.push_back(2);



	s.indexnum = index.size();

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

		glGenBuffers(1, &s.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), index.data(), GL_STATIC_DRAW);

	}
	

	vertex.clear();
	color.clear();
	index.clear();
}

//--------------------InitBuffer_tetrahedron---------------------------------------
void InitBuffer_tetrahedron(shape& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;
	std::vector<unsigned int> index;
	
	for (int i = 0; i < 4; ++i) {
		if (i == 0) {
			vertex.push_back(0);	//x - cos
			vertex.push_back(sqrt(6)/3);		//y - sin
			vertex.push_back(0.0f);		//z
		} 
		else{
			vertex.push_back(cos(glm::radians(120.0f*i))*(1/sqrt(3)));	//x - cos
			vertex.push_back(0.0f);		//y - sin
			vertex.push_back(sin(glm::radians(120.0f * i)) * (1 / sqrt(3)));		//z
		}
		color.push_back(randomnum(0.0f, 1.0f));
		color.push_back(randomnum(0.0f, 1.0f));
		color.push_back(randomnum(0.0f, 1.0f));

		
		std::cout << std::format("���� ���� [{}]: vertex - ({},{},{}),\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2));
	}
	//1-
	index.push_back(0);
	index.push_back(1);
	index.push_back(2);
	//----2
	index.push_back(0);
	index.push_back(2);
	index.push_back(3);
	//---3
	index.push_back(0);
	index.push_back(3);
	index.push_back(1);
	//---4--�ǾƷ�
	index.push_back(1);
	index.push_back(2);
	index.push_back(3);



	s.indexnum = index.size();

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

		glGenBuffers(1, &s.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), index.data(), GL_STATIC_DRAW);

	}


	vertex.clear();
	color.clear();
	index.clear();


}
//�浹����------------------------------------------------------------------------------------------
bool IsCollision(const shape& rect1, const shape& rect2) {

	// �� �簢���� �� ����
	float rect1_left = rect1.x - rect1.size;
	float rect1_right = rect1.x + rect1.size;
	float rect1_top = rect1.y + rect1.size;
	float rect1_bottom = rect1.y - rect1.size;

	float rect2_left = rect2.x - rect2.size;
	float rect2_right = rect2.x + rect2.size;
	float rect2_top = rect2.y + rect2.size;
	float rect2_bottom = rect2.y - rect2.size;

	// �浹 ����
	if (rect1_left < rect2_right &&
		rect1_right > rect2_left &&
		rect1_top > rect2_bottom &&
		rect1_bottom < rect2_top) {
		return true; // �浹 �߻�
	}

	return false; // �浹 ����

}

//----------------Mouse----------------------------------
void Mouse(int button, int state, int x, int y) {

	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		 degree += 10.0f;

		left_button = true;
		//InitBuffer(rect[target]);
		//glutTimerFunc(10, TimerA, target);
	
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		
		left_button = false;
	}

	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

//----------Motion----------------------------------------
void Motion(int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (left_button == true) {

	}

	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

//--------keyboard----------------------------------------

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key >= '0' and key <= '9') {
		int num = key - '0';
		target = num;
		target2 = -1;
		std::cout << "������ ����: " << target << '\n';

	}
	else {
		switch (key)
		{
		case'C' :case'c':
			target = randomnum(1, 6);	//1~6
			target2 = target + randomnum(1, 5);	//+ 1~5
			if (target2 >= 7) target2 -= 6;
			break;
		case'T':case't':
			target = randomnum(7, 10) % 10;	//7,8,9,0
			target2 = (target + randomnum(1, 3)) % 10;
			if (target2 > 0 and target2 < 7) { target2 += 6; }
			break;
		case'Q':case'q':
			exit(0);
		default:
			break;
		}
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

// ---- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- ����� ���� ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- ������ ���������ο� ���̴� �ҷ�����

	glEnable(GL_DEPTH_TEST);
	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
		transformMatrix = glm::rotate(transformMatrix,glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix,glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix,glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

	}
	//�����ü
	if (target > 6 or target == 0) {	
		int i = target % 6;
		glBindVertexArray(tetrahedron.vao);								//--- ����� VAO �ҷ�����
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));	//ť�� 1�� ���
		if (target2 != -1) {
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(target2 % 6 * 3 * sizeof(unsigned int)));	//ť�� 1�� ���
		}
		//glDrawElements(GL_TRIANGLES, tetrahedron.indexnum, GL_UNSIGNED_INT, 0);	//ť�� 1�� ���
		//for(int j = 0; j < 4; j++){
		//	glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(j * 3 * sizeof(unsigned int)));	//ť�� 1�� ���
		//}
	}
	//������ü
	else {
		//glDrawElements(GL_TRIANGLES, rect.indexnum, GL_UNSIGNED_INT, 0);	//ť�� ���
		glBindVertexArray(rect.vao);								//--- ����� VAO �ҷ�����
		int i = target - 1;
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));	//ť�� 1�� ���
		if (target2 != -1) {
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((target2 - 1) * 6 * sizeof(unsigned int)));	//ť�� 1�� ���
		}
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

	

	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
}


void reset() {
	InitBuffer_cube(rect);
	InitBuffer_tetrahedron(tetrahedron);
	

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
	//--- GLEW �ʱ�ȭ�ϱ�e
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
	glutMotionFunc(Motion); // ���콺 �Է�

	glutMainLoop();
}