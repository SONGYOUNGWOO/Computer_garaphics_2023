#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
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
GLvoid Reshape(int w, int h);
void InitBuffer();

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

const GLfloat triShape[3][3] = {							//--- �ﰢ�� ��ġ ��
	{ -0.5, -0.5, 0.0 },
	{ 0.5, -0.5, 0.0 },
	{ 0.0, 0.5, 0.0}
};
const GLfloat colors[3][3] = {								//--- �ﰢ�� ������ ����
	{ 1.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{0.0, 0.0, 1.0 }
};
GLuint vao, vbo[2];
void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao);		//--- VAO�� ���ε��ϱ�
	glGenBuffers(2, vbo);		//--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);											//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);	//--- triShape �迭�� ������: 9 * float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glEnableVertexAttribArray(0);													//--- attribute �ε��� 0���� ��밡���ϰ� ��

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  										//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW); 	//--- colors �迭�� ������: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glEnableVertexAttribArray(1);													//--- attribute �ε��� 1���� ��� �����ϰ� ��.

}
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

typedef struct allshape {
	GLfloat x, y, size;		// �߽� ��ǥ, ���콺 Ŭ��
	GLfloat allvertex[4][3];	// ��ġ
	GLfloat allcolor[4][3];		// ����
	GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
}shape;
shape rect[4];					//�ִ� 10��
int n = 0;
bool a = false;
bool b = false;

void InitBuffer(shape* s) {
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao);		//--- VAO�� ���ε��ϱ�
	glGenBuffers(2, vbo);		//--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);											//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allvertex, GL_STATIC_DRAW);	//--- triShape �迭�� ������: 9 * float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glEnableVertexAttribArray(0);													//--- attribute �ε��� 0���� ��밡���ϰ� ��

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  										//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allcolor, GL_STATIC_DRAW); 	//--- colors �迭�� ������: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glEnableVertexAttribArray(1);
}
void setline(int i) {
	rect[i].allvertex[0][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].x + rect[i].size;
	rect[i].allvertex[1][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = rect[i].x;
	rect[i].allvertex[2][1] = rect[i].y + rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[3][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;


}
void settriangle(int i) {
	rect[i].allvertex[0][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].x + rect[i].size;
	rect[i].allvertex[1][1] = rect[i].y - rect[i].size/ 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = rect[i].x;
	rect[i].allvertex[2][1] = rect[i].y + rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[3][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;
}

void quadrant(float mx, float my) {
	int aa = 0;
	if (mx > 0)  aa += 1;
	if (my < 0)  aa += 2;

	rect[aa].x = mx;
	rect[aa].y = my;
	rect[aa].size = randomnum(0.1, 0.3);
	settriangle(aa);

}
void changet(float mx, float my) {
	int ct = 0;
	if (mx > 0)  ct += 1;
	if (my < 0)  ct += 2;

	rect[ct].x = mx;
	rect[ct].y = my;
	rect[ct].size = randomnum(0.1, 0.3);

	

	if (rect[ct].choice == 0) {

		settriangle(ct);
	}
	else {
	
		setline(ct);
	}


}

void Mouse(int button, int state, int x, int y){
	float mx =  (x - 800 / 2.0f) / (800 / 2.0f);
	float my =  (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		changet(mx, my); // �߰�

	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':case 'A':
		a = true;
		b = false;
		if (a) {
			for (int i = 0; i < 4; ++i) {
				rect[i].choice = 0;
			}
			
		}
		break;
	case 'b':case 'B':
		a = false;
		b = true;
		if (b) {
			for (int i = 0; i < 4; ++i) {
				rect[i].choice = 1;
			}
		}
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
		InitBuffer(&rect[i]);
		glBindVertexArray(vao);								//--- ����� VAO �ҷ�����

		std::cout << rect[i].choice;

		if (rect[i].choice == 0) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else  {
			glLineWidth(2);
			glDrawArrays(GL_LINE_STRIP, 0, 4);
			
		}
	}

	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
}
void reset() {

	for (int i = 0; i < 4; ++i) {

		float x = i % 2 == 0 ? -0.5 : 0.5;
		float y = i / 2 == 0 ? -0.5 : 0.5;
		quadrant(x, y);
		GLfloat randomColor[3] = { randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) };

		for (int a = 0; a < 4; ++a) {
			
			for (int b = 0; b < 3; ++b) {
				rect[i].allcolor[a][b] = randomColor[b];
			}

		}

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
	glutCreateWindow("Example8");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	make_shaderProgram();

	InitBuffer();
	reset();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է�
	glutMouseFunc(Mouse); // ���콺 �Է�
	glutMainLoop();
}