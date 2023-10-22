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
//-------------------------------------------------------------------------------------------------------------------------
//glew32.lib freeglut.lib 
//-------------------------------------------------------------------------------------------------------------------------
const std::string Guide[]{
	"--------------------------------------------------------------------------------------------------"
	"x/X/y/Y: ��ü�� x/y �࿡ ���Ͽ� ���� �� �� �������� ȸ���ϱ� �� ��ü�� ��� ����",
	"1/2/3: 1 �� ������ ������ ��ü�� , 2 �� ������ ������ ��ü�� , 3 �� ������ ��� �����Ѵ�",
	"r/R: �� ��ü�� ȭ�� �߾��� y �࿡ ���Ͽ� �� �� �������� ȸ���ϱ� ����",
	"h/H: �������� ����/����",
	"w/W: ���̾� ��ü/�ָ��� ��ü",
	"c : �� ������ �ٸ� �������� �ٲ۴�",
	"s : �ʱ�ȭ �ϱ�",
	"q : ���α׷� ����",
	"--------------------------------------------------------------------------------------------------"
};

#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
//-------------------------------------------------------------------------------------------------------------------------
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

////-------------------------------------------------------------------------------------------------------------------------
class linexyz {
public:
	glm::vec3 p[6]; // �� ���� ��ǥ
	GLuint vao;
	GLuint vbo[2];
	std::vector<float> color;

	int* points;
	//������
	linexyz() {
		points = new int;
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		color.clear();

		for (int i = 0; i < 6; ++i) {
			p[i] = { 0.0f, 0.0f, 0.0f };
			p[i][i/2] = (i % 2 == 0 ? -1.0f : 1.0f);

			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
		}
	}
};
class shapecube {
private:
	std::string name;
public:
	//------------------------------
	glm::vec3 p[8]; //�簢���� �� ���� ��ǥ
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	float dx, dy;
	int indexnum;	
	std::vector<float> color;
	//------------------------------
	int* points;
	//������
	shapecube() {
		points = new int;
		this->name = "None";
		this->vao = 0;
		this->vbo[0] = 0;
		this->vbo[1] = 0;
		this->ebo = 0;
		this->color.clear();
		this->dx = 0.0f;
		this->dy = 0.0f;
		this->indexnum = 0;
		
		for (int i = 0; i < 8; ++i) {
			this->p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f) + this->dx);	
			this->p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f) + this->dy);	
			this->p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);	

			this->color.push_back(randomnum(0.0f, 1.0f));
			this->color.push_back(randomnum(0.0f, 1.0f));
			this->color.push_back(randomnum(0.0f, 1.0f));
		}
	}

	//�Ҹ���
	~shapecube() {
		delete points;
	}
	//��� �Լ�(method)
	void reset() {
		for (int i = 0; i < 8; ++i) {
			this->p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f));
			this->p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f));
			this->p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);
		}
	}
	void Setname(const std::string& name) {
		this->name = name;
	}

	std::string Getname() {
		return this->name;
	}

	void Print_member() {
		std::cout << "------------------------------------------------------------------" << '\n';
		std::cout << "���� ��ġ��" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = (" << this->p[i].x << ", " << this->p[i].y << ", " << this->p[i].z << ")" << '\n';
		}
		std::cout << "����" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << this->color.at(i * 3) << ", " << this->color.at(i * 3 + 1) << ", " << this->color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}

};
class shapep {
private:
	std::string name;
public:
	glm::vec3 p[5]; //�簢���� �� ���� ��ǥ
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	int indexnum;
	std::vector<float> color;
	//------------------
	float dx, dy;
	int* points;
	//������
	shapep () {
		points = new int;
		this->name = "None";
		this->vao = 0;
		this->vbo[0] = 0;
		this->vbo[1] = 0;
		this->ebo = 0;
		this->color.clear();
		this->dx = 0.0f;
		this->dy = 0.0f;
		this->indexnum = 0;

		for (int i = 0; i < 5; ++i) {
			if (i == 0) {
				this->p[i].x = (0.0f + this->dx);		//x - cos
				this->p[i].y = (1 + this->dy);		//y - sin
				this->p[i].z = (0.0f);		//z
			}
			else {
				this->p[i].x = ((i / 3 == 0 ? -1.0f : 1.0f) + this->dx);	//x - cos
				this->p[i].y = (-1.0f + this->dy);		//y - sin
				this->p[i].z = (i % 3 == 1 ? 1.0f : -1.0f);	//z
			}
			this->color.push_back(randomnum(0.0f, 1.0f));
			this->color.push_back(randomnum(0.0f, 1.0f));
			this->color.push_back(randomnum(0.0f, 1.0f));
		}
	}

	//�Ҹ���
	~shapep () {
		delete points;
	}
	//��� �Լ�(method)
	void reset() {
		for (int i = 0; i < 5; ++i) {
			if (i == 0) {
				this->p[i].x = (0.0f);		//x - cos
				this->p[i].y = (1);		//y - sin
				this->p[i].z = (0.0f);		//z
			}
			else {
				this->p[i].x = ((i / 3 == 0 ? -1.0f : 1.0f));	//x - cos
				this->p[i].y = (-1.0f);		//y - sin
				this->p[i].z = (i % 3 == 1 ? 1.0f : -1.0f);	//z
			}
		}
	}
	void Setname(const std::string& name) {
		this->name = name;
	}

	std::string Getname() {
		return this->name;
	}

	void Print_member() {
		std::cout << "------------------------------------------------------------------" << '\n';
		std::cout << "���� ��ġ��" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = (" << this->p[i].x << ", " << this->p[i].y << ", " << this->p[i].z << ")" << '\n';
		}
		std::cout << "����" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << this->color.at(i * 3) << ", " << this->color.at(i * 3 + 1) << ", " << this->color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}

};

//--- �������� -------------------------------------------------------------------------------------------------------
//--------------------xyz-----------------------------------------------------------
const glm::vec3 x_axis{ 1.0f,0.0f,0.0f }; //x��
const glm::vec3 y_axis{ 0.0f,1.0f,0.0f }; //y��
const glm::vec3 z_axis{ 0.0f,0.0f,1.0f }; //z��
shapecube cube;
shapep square_horn;
linexyz xyz;//xyz�� �׸���
int target{ 1 };  //������ ���� ó�� ������ ������ü
int targetglu{ 4 };//������ glu ���� ó�� ������ �� ��
bool DEPTH_T{ true }; // ��������
bool t_or_l{ false };//�� �Ǵ� ��
bool rotateXY{ false };// ȸ��
bool left_button{ false }; //��Ŭ��
int objglu{ 3 }; // 1 Ŭ���� glu�� ���� ����
GLfloat degree{ 0.0f }; // ��Ŭ���� ȸ����
glm::vec3 rotate{ 0.0f }; // ���� �������� ���� �Ǵ� ����
glm::vec3 rotateobjglu{ 0.0f }; // ���� �������� ���� �Ǵ� ����
glm::vec3 rotateobj{ 0.0f }; // ���� �������� ���� �Ǵ� ����
//-------------------------------------------------------------------------------------------------------

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
//-------------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------

//--------------------InitBuffer_cube---------------------------------------
void InitBuffer_cube(shapecube& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;
	std::vector<unsigned int> index;
	
	for (int i = 0; i < 8; ++i)
	{
		vertex.push_back(s.p[i].x);		
		vertex.push_back(s.p[i].y);		
		vertex.push_back(s.p[i].z);						

		color.push_back(s.color.at(i * 3));		//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b
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
//--------------------InitBuffer_square_horn---------------------------------------
void InitBuffer_square_horn(shapep& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;
	std::vector<unsigned int> index;


	for (int i = 0; i < 5; ++i) {
		vertex.push_back(s.p[i].x);
		vertex.push_back(s.p[i].y);
		vertex.push_back(s.p[i].z);

		color.push_back(s.color.at(i * 3));		//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b
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
	index.push_back(4);
	//---4
	index.push_back(0);
	index.push_back(4);
	index.push_back(1);
	//---5-�ٴ�
	index.push_back(1);
	index.push_back(4);
	index.push_back(2);
	//---5-�ٴ�
	index.push_back(2);
	index.push_back(4);
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
//--------------------InitBuffer_line_xyz(linexyz& s)---------------------------------------
void InitBuffer_line_xyz(linexyz& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;

	for (int i = 0; i < 6; ++i)
	{
		vertex.push_back(s.p[i].x);
		vertex.push_back(s.p[i].y);
		vertex.push_back(s.p[i].z);

		color.push_back(s.color.at(i * 3));		//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b
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

	vertex.clear();
	color.clear();
}
//----------------Mouse----------------------------------
void Mouse(int button, int state, int x, int y) {

	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		degree += 10.0f;

		left_button = true;

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
//----------Timer_rotateX-------------------------------------------------------------------------------------
void TimerrotateX(int value) {
	rotateobj.x += 1.0f;
	rotateobjglu.x += 1.0f;
	if (rotateobj.x > 360.0f or rotateobjglu.x > 360.0f) {
		rotateobj.x -= 360.0f;
		rotateobjglu .x -= 360.0f;
	}
	glutPostRedisplay();
	if (rotateXY)
		glutTimerFunc(10, TimerrotateX, 0);
}
//----------Timer_rotateY-------------------------------------------------------------------------------------
void TimerrotateY(int value) {
	if (objglu == 1) {
		rotateobjglu.y += 1.0f;
	}
	else if (objglu == 2) {
		rotateobj.y += 1.0f;
	}
	else if (objglu == 3) {
		rotateobj.y += 1.0f;
		rotateobjglu.y += 1.0f;
	}
	if (rotateobj.x > 360.0f or rotateobjglu.x > 360.0f) {
		rotateobj.x -= 360.0f;
		rotateobjglu.x -= 360.0f;
	}
	glutPostRedisplay();
	if (rotateXY)
		glutTimerFunc(10, TimerrotateY, 0);
}
//----------Timer_rotate-------------------------------------------------------------------------------------
void Timer_rotate(int value) {
	rotate.y += 1.0f;
	if (rotate.y > 360.0f) {
		rotate.y -= 360.0f;
	}

	glutPostRedisplay();
	if (rotateXY)
		glutTimerFunc(10, Timer_rotate, 0);
}

//--------keyboard----------------------------------------
GLvoid Keyboard(unsigned char key, int x, int y) {
	std::vector<float> vertex;
	switch (key)
	{

	//1 �� ������ ������ ��ü��(glu) , 2 �� ������ ������ ��ü�� , 3 �� ������ ��� �����Ѵ�
	case'1':
		objglu = 1;
		rotateXY = rotateXY == true ? false : true;
		glutTimerFunc(10, TimerrotateY, 0);
		break;
	case'2':
		objglu = 2;
		rotateXY = rotateXY == true ? false : true;
		glutTimerFunc(10, TimerrotateY, 0);
		break;
	case'3':
		objglu = 3;
		rotateXY = rotateXY == true ? false : true;
		glutTimerFunc(10, TimerrotateY, 0);
		break;
	case'C':case'c':
		target = randomnum(0, 1);
		targetglu = randomnum(3, 4);
		break;

	case'R':case'r': // y�� �������� ��ü ����
		rotateXY = rotateXY == true ? false : true;
		glutTimerFunc(10, Timer_rotate, 0);
		break;
	case'H':case'h'://h: �������� ����/����
		DEPTH_T = DEPTH_T == true ? false : true;
		break;
		
	case'W':case'w'://w/W: ���̾� ��ü/�ָ��� ��ü
		t_or_l = t_or_l == true ? false : true;
		break;
	case'X':case'x'://x / X: x���� �������� �� / �� �������� ȸ�� �ִϸ��̼�(����)
		rotateXY = rotateXY == true ? false : true;
		glutTimerFunc(10, TimerrotateX, 0);
		break;
	case'Y':case'y'://y / Y : y���� �������� �� / �� �������� ȸ�� �ִϸ��̼�(����)
		rotateXY = rotateXY == true ? false : true;
		glutTimerFunc(10, TimerrotateY, 0);
		break;
	case'S':case's'://s : �ʱ� ��ġ�� ����(���� �ִϸ��̼ǵ� ���߱�
		rotateXY = false;
		rotate.x = 0.0f;
		rotate.y = 0.0f;
		degree = 0.0f;
		rotateobj.x = 0.0f;
		rotateobj.y = 0.0f;
		rotateobjglu.x = 0.0f;
		rotateobjglu.y = 0.0f;

		objglu = 3; // �� ��
		target = 1; // ������ü
		targetglu = 4; // ����

		InitBuffer_cube(cube);
		InitBuffer_square_horn(square_horn);
		break;
	case'Q':case'q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}
//�� / �� / �� / �� : �� / �� / �� / �Ϸ� ��ü�� �̵��Ѵ�. (x��, y������ �̵�)
GLvoid specialkeyborad(int key, int x, int y) {
	std::cout << key << "�Է¹���." << '\n';
	switch (key)
	{
	case GLUT_KEY_LEFT:// ��
		cube.dx -= 0.01f;
		square_horn.dx -= 0.01f;
		break;

	case GLUT_KEY_RIGHT:// ��
		cube.dx += 0.01f;
		square_horn.dx += 0.01f;
		break;

	case GLUT_KEY_UP:// ��
		cube.dy += 0.01f;
		square_horn.dy += 0.01f;
		break;

	case GLUT_KEY_DOWN:// ��
		cube.dy -= 0.01f;
		square_horn.dy -= 0.01f;
		break;
	}
	InitBuffer_cube(cube);
	InitBuffer_square_horn(square_horn);
	glutPostRedisplay();
}

// ---- �׸��� �ݹ� �Լ�---------------------------------------------------------------------------------------------------
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- ����� ���� ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- ������ ���������ο� ���̴� �ҷ�����

	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

		glBindVertexArray(xyz.vao);
		glLineWidth(4);
		glDrawArrays(GL_LINES, 0, 6);
	}
	if (DEPTH_T) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	
	//glu---------------------------------- 
	{
		GLUquadricObj* qobj;
		glLineWidth(1);

		qobj = gluNewQuadric(); // ��ü �����ϱ�
		gluQuadricDrawStyle(qobj, GLU_LINE); // ���� ��Ÿ��
		gluQuadricNormals(qobj, GLU_SMOOTH); //? ���� ����
		gluQuadricOrientation(qobj, GLU_OUTSIDE); //? ���� ����


		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);

		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);

		transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0f, 0.0f, -0.8f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));//���콺
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobjglu.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobjglu.x), glm::vec3(1.0f, 0.0f, 0.0f));

	
		transformMatrix = glm::scale(transformMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

		if (targetglu == 3)
			gluSphere(qobj, 0.5, 50, 50); // �� ��ü �����

		else if (targetglu == 4)
			gluCylinder(qobj, 1.0, 0.0, 2.0, 20, 8);


	}

	{

		glm::mat4 transformMatrix(1.0f);


		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);

		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);

		transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0f, 0.0f, 0.8f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));//���콺
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobj.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobj.x), glm::vec3(1.0f, 0.0f, 0.0f));

		transformMatrix = glm::scale(transformMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

	}

	//�Ƕ�̵�
	if (target == 0) {
		glBindVertexArray(square_horn.vao);								//--- ����� VAO �ҷ�����
		//������ ���
		if (t_or_l)
		{
			glDrawElements(GL_TRIANGLES, square_horn.indexnum, GL_UNSIGNED_INT, 0);	//ť�� 1�� ���
		}
		else
			//������ ���
		{
			glLineWidth(6);
			for (int j = 0; j < 4; j++) {
				glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(j * 3 * sizeof(unsigned int)));	//ť�� 1�� ���
			}
		}
	}
	//������ü
	else if (target == 1) {
		glBindVertexArray(cube.vao);								//--- ����� VAO �ҷ�����
		//������ ���
		if (t_or_l)
		{
			glDrawElements(GL_TRIANGLES, cube.indexnum, GL_UNSIGNED_INT, 0);	//ť�� ���
		}
		//������ ���
		else
		{
			for (int j = 0; j < 12; j++) {
				glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(j * 3 * sizeof(unsigned int)));	//ť�� 1�� ���
			}
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
	cube.Setname("cube");
	square_horn.Setname("square_horn");
	cube.dx = 0.0f;
	cube.dy = 0.0f;
	square_horn.dx = 0.0f;
	square_horn.dy = 0.0f;
	InitBuffer_cube(cube);
	InitBuffer_square_horn(square_horn);

	InitBuffer_line_xyz(xyz);
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void ReadObj(FILE* path) {
	char count[128];
	int vertexnum = 0;
	int facenum = 0;
	int uvnum = 0;
	//--- 1. ��ü ���ؽ� ���� �� �ﰢ�� ���� ����
	while (!feof(path)) {
		fscanf(path, "%s", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexnum++;
		else if (count[0] == 'f' && count[1] == '\0')
			facenum++;
		else if (count[0] == 'v' && count[1] == 't' && count[3] == '\0')
			uvnum++;
		memset(count, '\0', sizeof(count));
	}
	rewind(path);
	int vertIndex = 0;
	int faceIndex = 0;
	int uvIndex = 0;
	//--- 2. �޸� �Ҵ�
	glm::vec3* vertex = new glm::vec3[vertexnum];
	glm::vec3* face = new glm::vec3[facenum];
	glm::vec3* uvdata = new glm::vec3[facenum];
	glm::vec3* uv = new glm::vec3[uvnum];
	char bind[128];
	//--- 3. �Ҵ�� �޸𸮿� �� ���ؽ�, ���̽�, uv ���� �Է�
	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(path, "%f %f %f\n",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);
			vertIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			unsigned int temp_face[3], temp_uv[3], temp_normal[3];
			fscanf(path, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&temp_face[0], &temp_uv[0], &temp_normal[0],
				&temp_face[1], &temp_uv[1], &temp_normal[1],
				&temp_face[2], &temp_uv[2], &temp_normal[2]);
			face[faceIndex].x = temp_face[0];
			face[faceIndex].y = temp_face[1];
			face[faceIndex].z = temp_face[2];
			uvdata[faceIndex].x = temp_uv[0];
			uvdata[faceIndex].y = temp_uv[1];
			uvdata[faceIndex].z = temp_uv[2];
			uv[faceIndex].x = temp_normal[0];
			uv[faceIndex].y = temp_normal[1];
			uv[faceIndex].z = temp_normal[2];
			faceIndex++;
		}
		else if (bind[0] == 'v' && bind[1] == 't' && bind[2] == '\0') {
			fscanf(path, "%f %f\n", &uv[uvIndex].x, &uv[uvIndex].y);
			uvIndex++;
		}
	}
	//--- �ʿ��� ��� �о�� ���� ���� ���� � ����
	delete[] vertex;
	delete[] face;
	delete[] uvdata;
	delete[] uv;
	vertex;
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

	for (std::string s : Guide) {
		std::cout << s << '\n';
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է�
	glutSpecialFunc(specialkeyborad);
	glutMouseFunc(Mouse); // ���콺 �Է�
	glutMotionFunc(Motion); // ���콺 �Է�

	glutMainLoop();
}