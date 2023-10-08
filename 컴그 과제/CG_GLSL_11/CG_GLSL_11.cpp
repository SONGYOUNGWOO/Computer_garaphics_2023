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
	float time;
	int pointnum;
	int indexnum;
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;

	std::vector<float> color;

	//float chy;
	//glm::vec3 mat_size;
	//GLfloat allvertex[4][3];	// ��ġ
	//GLfloat allcolor[4][3];		// ����
	//GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
	//int dx, dy, touchnum;		// ���� ���� , ���� ���� Ƚ��				// 0 �̶� 180�� ���Ʒ�
}shape;

//--- �������� ----
shape rect[4];
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
	std::vector<float> s_vertex;
	std::vector<float> l_vertex;
	std::vector<float> vertex;

	std::vector<float> color;


	std::vector<unsigned int> index;



	float s_rad = 360.0/ s.pointnum;
	float l_rad = 360.0/ (s.pointnum + 1);
	for(int i = 0; i < s.pointnum + 1; ++i)
	{
		s_vertex.push_back(s.x + cos(glm::radians( i * s_rad)) *  s.size);	//x
		s_vertex.push_back(s.y + sin(glm::radians( i * s_rad)) * s.size);	//y
		s_vertex.push_back(0.0);	//z

		if (s.pointnum < 5) {
			l_vertex.push_back(s.x + cos(glm::radians(i * l_rad)) * s.size);	//x
			l_vertex.push_back(s.y + sin(glm::radians(i * l_rad)) * s.size);	//y
			l_vertex.push_back(0.0);	//z
		}
		else {
			l_vertex.push_back(s.x);	//x
			l_vertex.push_back(s.y);	//y
			l_vertex.push_back(0.0);	//z
		}
		vertex.push_back((1 - s.time) * s_vertex.at(i * 3) + s.time * l_vertex.at(i * 3));	//x
		vertex.push_back((1 - s.time) * s_vertex.at(i * 3 + 1) + s.time * l_vertex.at(i * 3 + 1));	//y
		vertex.push_back(0.0);	//z
		
		color.push_back(s.color.at(i * 3));	//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b

		//�����
		{
			std::cout << std::format("���� ���� [{}]: vertex - ({},{},{}), time : {}\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2), s.time);
		}		
	}

	if(s.pointnum + 1 > 0)
		index.push_back(0);		//���� 1���ϋ�
	if (s.pointnum + 1 > 1)
		index.push_back(1);		//���� 2���ϋ�
	if (s.pointnum + 1 > 2)
		index.push_back(2);		//���� 3���ϋ�
	if (s.pointnum + 1 > 3){
		index.push_back(0);
		index.push_back(2);
		index.push_back(3);		//���� 4���ϋ�
	}
	if (s.pointnum + 1 > 4) {
		index.push_back(0);
		index.push_back(3);
		index.push_back(4);		//���� 5���ϋ�
	}
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
	
	

	s_vertex.clear();
	l_vertex.clear();
	vertex.clear();
	color.clear();
	index.clear();
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
		int target{ 0 };
		mx > 0.0f ? target++ : 0;
		my < 0.0f ? target += 2 : 0;
		//InitBuffer(rect[target]);
		std::cout << "���콺 Ŭ�� Ÿ�̸� ���۵� ���� ��ȣ[:" << target  <<"]" << '\n';
		glutTimerFunc(10, TimerA, target);
		
		
		//changet(mx, my); // �߰�
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}



//Ÿ�̸� �Լ�
void TimerA(int value) {
	if (rect[value].time > 1.0) {
		/*rect[value].pointnum++;
		if (rect[value].pointnum > 5) {
			rect[value].pointnum = 1;
		}*/
		rect[value].time = 0.0;
	}
	else {
		rect[value].time+= 0.01f;
	}
	//std::cout << "Ÿ�̸� ȣ��: rect[" << value << "].pointnum =" << rect[value].pointnum << '\n';
	InitBuffer(rect[value]);

	glutPostRedisplay();	
	if(rect[value].pointnum < 6)
		glutTimerFunc(10, TimerA, value);
}



GLvoid Keyboard(unsigned char key, int x, int y) {

	
	switch (key) {
	//��
	case 'l': case '��':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 2;
			InitBuffer(rect[i]);
		}
		break;
	//�ﰢ��
	case 't':case 'T':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 3;
			InitBuffer(rect[i]);
		}
		break;
	// �簢��
	case 'r':case 'R':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 4;
			InitBuffer(rect[i]);
		}
		break;
	//������
	case 'p':case 'P':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 5;
			InitBuffer(rect[i]);
		}
		break;
	// �ٽ�
	case 'a':case 'A':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = i+ 2;
			InitBuffer(rect[i]);
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

	for (int i = 0; i < n; ++i) {

		glBindVertexArray(rect[i].vao);								//--- ����� VAO �ҷ�����
		{
			glm::mat4 transformMatrix(1.0f);
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

		}

		if (choice == 0) {
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, rect[i].pointnum + 1);
			if (rect[i].pointnum == 0) {
				glPointSize(5);
				glDrawElements(GL_POINTS, rect[i].indexnum, GL_UNSIGNED_INT, 0);
			}
			else if (rect[i].pointnum == 1 || (rect[i].pointnum == 2 and rect[i].time == 0.0f)) {
				glLineWidth(5);
				glDrawElements(GL_LINES, rect[i].indexnum , GL_UNSIGNED_INT, 0);

			}
			else
				glDrawElements(GL_TRIANGLES, rect[i].indexnum, GL_UNSIGNED_INT, 0);
		}
		else {
			glLineWidth(5);
			glDrawArrays(GL_LINE_LOOP, 0, rect[i].pointnum + 1);

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
	n = 4;
	for (int i = 0; i < 4; i++) {
		rect[i].x = i % 2 == 1 ? 0.5 : -0.5;
		rect[i].y = i < 2 ? 0.5 : -0.5;
		rect[i].time = 0.0f;
		rect[i].pointnum = i + 2;
		rect[i].size = 0.3f;

		for (int j = 0; j < 6; j++) {
			rect[i].color.push_back(randomnum(0.0, 1.0));		//r
			rect[i].color.push_back(randomnum(0.0, 1.0));		//g
			rect[i].color.push_back(randomnum(0.0, 1.0));		//b
		}
		InitBuffer(rect[i]);
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