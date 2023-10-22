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
	GLfloat x, y , sizel;		// �߽� ��ǥ, ���콺 Ŭ��
	GLfloat allvertex[4][3];	// ��ġ
	GLfloat allcolor[4][3];		// ����
	GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
}shape;
shape rect[10];					//�ִ� 10��
int n = 0;
//-------------------------------------------------------------------------------------------------------------
//------------InitBuffer---------------------------------------------------------------------------------------
void InitBuffer(shape *s) {
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
void setpoint(int i) {

	rect[i].allvertex[0][0] = rect[i].x;
	rect[i].allvertex[0][1] = rect[i].y;
	rect[i].allvertex[0][2] = 0.0f;

}
void setline(int i) {

	rect[i].allvertex[0][0] = rect[i].x - rect[i].sizel;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].sizel;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].x + rect[i].sizel;
	rect[i].allvertex[1][1] = rect[i].y + rect[i].sizel;
	rect[i].allvertex[1][2] = 0.0f;

}
void settriangle(int i) {
	/*rect[i].allvertex[0][0] = rect[i].x + sqrt(3) * rect[i].sizel;*/
	rect[i].allvertex[0][0] = rect[i].x - rect[i].sizel;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].sizel / 2;
	rect[i].allvertex[0][2] = 0.0f;

	/*rect[i].allvertex[1][0] = rect[i].x - sqrt(3) * rect[i].sizel;*/
	rect[i].allvertex[1][0] = rect[i].x + rect[i].sizel;
	rect[i].allvertex[1][1] = rect[i].y - rect[i].sizel / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = rect[i].x;
	rect[i].allvertex[2][1] = rect[i].y + rect[i].sizel ;
	rect[i].allvertex[2][2] = 0.0f;


}
void setsquare(int i) {

	rect[i].allvertex[0][0] = rect[i].x - rect[i].sizel / 2;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].sizel;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].x + rect[i].sizel / 2;
	rect[i].allvertex[1][1] = rect[i].y - rect[i].sizel ;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = rect[i].x - rect[i].sizel / 2;
	rect[i].allvertex[2][1] = rect[i].y + rect[i].sizel;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = rect[i].x + rect[i].sizel / 2;
	rect[i].allvertex[3][1] = rect[i].y + rect[i].sizel;
	rect[i].allvertex[3][2] = 0.0f;
}

void addshape(int x, int y) {
	if (n == 10) {
		
	}
	rect[n].x = (x - 800 / 2.0f) / (800 / 2.0f);
	rect[n].y = (800/ 2.0f - y ) / (800 / 2.0f);
	rect[n].sizel = 0.2f;
	//rect[n].choice = 0;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j) {
			rect[n].allcolor[i][j] = randomnum(0.0, 1.0);
		}
	}

	if (rect[n].choice == 0) {		// ��
		setpoint(n);
	}
	else if (rect[n].choice == 1) {	// ��
		setline(n);
	}
	else if (rect[n].choice == 2) {	//�ﰢ��
		settriangle(n);
	}
	else if (rect[n].choice == 3) {	//�簢��
		setsquare(n);
	}
	n++;
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		addshape(x, y); //�簢�� �߰�
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

bool w = false;
bool a = false;
bool s = false;
bool d = false;

void TimerW(int value) {
	for (int i = 0; i < n; ++i) {
		rect[i].y += 0.01f;
		if (rect[i].choice == 0) {		// ��
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// ��
			setline(i);
		}
		else if (rect[i].choice == 2) {	//�ﰢ��
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//�簢��
			setsquare(i);
		}
	}
	glutPostRedisplay();
	if (w == true)
		glutTimerFunc(60, TimerW, 1);
}
void TimerA(int value) {
	for (int i = 0; i < n; ++i) {
		rect[i].x -= 0.01f;
		if (rect[i].choice == 0) {		// ��
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// ��
			setline(i);
		}
		else if (rect[i].choice == 2) {	//�ﰢ��
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//�簢��
			setsquare(i);
		}
	
	}
	glutPostRedisplay();
	if (a == true)
		glutTimerFunc(60, TimerA, 1);
}
void TimerS(int value) {
	for (int i = 0; i < n; ++i) {
		rect[i].y -= 0.01f;
		if (rect[i].choice == 0) {		// ��
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// ��
			setline(i);
		}
		else if (rect[i].choice == 2) {	//�ﰢ��
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//�簢��
			setsquare(i);
		}
	}
	glutPostRedisplay();
	if (s == true)
		glutTimerFunc(60, TimerS, 1);
}
void TimerD(int value) {
	for (int i = 0; i < n; ++i) {
		rect[i].x += 0.01f;
		if (rect[i].choice == 0) {		// ��
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// ��
			setline(i);
		}
		else if (rect[i].choice == 2) {	//�ﰢ��
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//�簢��
			setsquare(i);
		}
	}
	glutPostRedisplay();
	if (d == true)
		glutTimerFunc(60, TimerD, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	int i = n ;
	switch (key) {
	case 'p':case 'P':	//p: �� �׸���
		rect[i].choice = 0;
		break;
	case 'i':case 'I':	//l: �� �׸���
		rect[i].choice = 1;
		break;
	case 't':case 'T':	//t: �ﰢ�� �׸���
		rect[i].choice = 2;
		break;
	case 'r':case 'R':	//r: �簢�� �׸���
		
		rect[i].choice = 3;
		break;
	case 'w':case 'W':	//w: ��
		w = true;
		a = false;
		s = false;
		d = false;
		if (w == true)
			glutTimerFunc(60, TimerW, 1);
		break;
	case 'a':case 'A':	//a: ����
		w = false;
		a = true;
		s = false;
		d = false;
		if (a == true)
			glutTimerFunc(60, TimerA, 1);
		break;
	case 's':case 'S':	//s: �Ʒ�
		w = false;
		a = false;
		s = true;
		d = false;
		if (s == true)
			glutTimerFunc(60, TimerS, 1);
		break;
	case 'd':case 'D':	//d: �����U
		w = false;
		a = false;
		s = false;
		d = true;
		if (d == true)
			glutTimerFunc(60, TimerD, 1);
		break;
	case 'c':case 'C':	//c: ��� ������ �����Ѵ�.
		n = 0;
		break;
	case 'q':case 'Q':	//q: ����
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

	for (int i = 0; i < n; ++i) {
		// std::cout << rect[i].x;
		InitBuffer(&rect[i]);
		glBindVertexArray(vao);								//--- ����� VAO �ҷ�����

		if (rect[i].choice == 0) {
			glPointSize(10);
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else if (rect[i].choice == 1) {
			glLineWidth(2);
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (rect[i].choice == 2) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else if (rect[i].choice == 3) {
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
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
	glutCreateWindow("Example7");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է�
	glutMouseFunc(Mouse); // ���콺 �Է�
	glutMainLoop();
}