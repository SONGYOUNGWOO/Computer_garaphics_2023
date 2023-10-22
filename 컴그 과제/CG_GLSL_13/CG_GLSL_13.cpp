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
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
class shape {
private:
	std::string name;
public:
	glm::vec3 p[4]; //�簢���� �� ���� ��ǥ
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;	
	std::vector<float> color;
	GLint motions;
	float mouse_check_size;
	int* points;
	//������
	shape() {
		points = new int;
		this->name = "None";
		this->vao = 0;
		this->vbo[0] = 0;
		this->vbo[1] = 0;
		this->ebo = 0;
		this->color.clear();
		this->motions = 0;
		this->mouse_check_size = 0.05f;

		for (int i = 0; i < 4; ++i) {
			this->p[i].x = i % 3 == 0 ? -0.5f : 0.5f;
			this->p[i].y = i / 2 == 0 ? -0.5f : 0.5f;
			this->p[i].z = 0.0f;

			this->color.push_back(i == 0?1.0f:0.0f);
			this->color.push_back(i == 1?1.0f:0.0f);
			this->color.push_back(i == 2?1.0f:0.0f);
		}
	}

	//�Ҹ���
	~shape() {
		delete points;
	}
	//��� �Լ�(method)
	void reset() {
		for (int i = 0; i < 4; ++i) {
			this->p[i].x = i % 3 == 0 ? -0.5f : 0.5f;
			this->p[i].y = i / 2 == 0 ? -0.5f : 0.5f;
			this->p[i].z = 0.0f;
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
			std::cout << "p[" << i << "] = ("<< this->p[i].x<<", " << this->p[i].y << ", " << this->p[i].z << ")" << '\n';
		}
		std::cout << "����" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << this->color.at(i* 3) << ", " << this->color.at(i * 3 + 1) << ", " << this->color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}
	//glm::vec3 mat_size;
	//GLfloat allvertex[4][3];	// ��ġ
	//GLfloat allcolor[4][3];		// ����
	//GLint choice;				// � ���� 0�� 1�� 2�ﰢ�� 3�簢��
	//int dx, dy, touchnum;		// ���� ���� , ���� ���� Ƚ��				// 0 �̶� 180�� ���Ʒ�
};

//--- �������� -------------------------------------------------------------------------------------------------------
shape axis;
shape rect;
int choice(0);		//�׸� ���� ���
bool start = false; // Ÿ�̸� ���� ����
int n = 0;			// ����� ���� ����
GLuint vao, vbo[2];
int target{ 0 };  //������ ����
bool Point_Inside_Rectangle = false; // �簢�� ����
bool left_button = false;
float mouse[2]{0, 0};	//0: x, 1: y
//----------------------------------------------------------------------------------------------------------------------
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
void InitBuffer(shape& s);

//-----------------------------randomnum---------------------------------------------------------------------------------------------
float randomnum(float a, float b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(a, b);
	return dis(gen);
}

//-----------------------------------------------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------------------------------------------

//----------------------------void InitBuffer(shape& s)------------------------------------------------------------------------------
void InitBuffer(shape& s) {
	//���� ����
	std::vector<float> vertex;
	std::vector<float> color;

	for(int i = 0; i < 4; ++i)
	{
	
		vertex.push_back(s.p[i].x);		//x - cos
		vertex.push_back(s.p[i].y);		//y - sin
		vertex.push_back(s.p[i].z);											//z
	
		//if (s.pointnum < 5) {
		//	l_vertex.push_back(s.x + cos(glm::radians(i * l_rad)) * s.size);	//x
		//	l_vertex.push_back(s.y + sin(glm::radians(i * l_rad)) * s.size);	//y
		//	l_vertex.push_back(0.0);	//z
		//}
		//else {
		//	l_vertex.push_back(s.x);	//x
		//	l_vertex.push_back(s.y);	//y
		//	l_vertex.push_back(0.0);	//z
		//}
		//vertex.push_back((1 - s.time) * s_vertex.at(i * 3) + s.time * l_vertex.at(i * 3));			//x
		//vertex.push_back((1 - s.time) * s_vertex.at(i * 3 + 1) + s.time * l_vertex.at(i * 3 + 1));	//y
		//vertex.push_back(0.0);	//z
		
		color.push_back(s.color.at(i * 3));		//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b

		//�����
/*		{
			std::cout << std::format("���� ���� [{}]: vertex - ({},{},{}), time : {}\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2), s.time);
		}	*/	
	}

	//if(s.pointnum + 1 > 0)
	//	index.push_back(0);		//���� 1���ϋ�
	//if (s.pointnum + 1 > 1)
	//	index.push_back(1);		//���� 2���ϋ�
	//if (s.pointnum + 1 > 2)
	//	index.push_back(2);		//���� 3���ϋ�
	//if (s.pointnum + 1 > 3){
	//	index.push_back(0);
	//	index.push_back(2);
	//	index.push_back(3);		//���� 4���ϋ�
	//}
	//if (s.pointnum + 1 > 4) {
	//	index.push_back(0);
	//	index.push_back(3);
	//	index.push_back(4);		//���� 5���ϋ�
	//}
	//if(s.pointnum + 1 > 5) {
	//	index.push_back(0);
	//	index.push_back(4);
	//	index.push_back(5);		//���� 8���ϋ�
	//}
	//s.indexnum = index.size();

	//gpu ���ۿ� �����ϱ�
	{
		glGenVertexArrays(1, &s.vao); //--- VAO �� �����ϰ� gpu�Ҵ��ϱ�
		glBindVertexArray(s.vao);		//--- VAO�� ���ε��ϱ�
		glGenBuffers(2, s.vbo);		//--- 2���� VBO�� �����ϰ� gpu�Ҵ��ϱ�

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
	
	//�̹� gpu�� ����� ������ cpu���� ����	
	vertex.clear();
	color.clear();
}

//----------------�簢�� ���� ���� -----------------------------------------------------------------------------------------------
bool IsPointInsideRectangle(const glm::vec2& point, const shape& rect) {
	int x_count = 0; // ������ ������ 0���� �ʱ�ȭ
	int y_count = 0; // ������ ������ 0���� �ʱ�ȭ

	// ���簢�� �������� �����κ�ȯ
	glm::vec2 rectSegments[4][2];
	for (int i = 0; i < 4; i++) {
		rectSegments[i][0] = glm::vec2(rect.p[i].x, rect.p[i].y);
		rectSegments[i][1] = glm::vec2(rect.p[(i + 1) % 4].x, rect.p[(i + 1) % 4].y); //���� ������(3->0)
	}

	//���Ʒ� 2�� ���� - ����x , y ��
	for (int i = 0; i < 4; i += 2) {
		glm::vec2 p1 = rectSegments[i][0];
		glm::vec2 p2 = rectSegments[i][1];

		if (p1.x != p2.x) { // ���� ����			
			float intersectionY = (point.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x) + p1.y;	//dy * m(������ ����) + x = Px
			if ((p1.y == p2.y and point.y < p1.y) || point.y <= intersectionY) {
				std::cout << i + 1 << "���� count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}
		}
		else { //���Ⱑ 0�� ��
			if (point.y < p1.y) {
				std::cout << i + 1 << "���� count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}

		}		
	}
	//�翷���� - ����y, x ��
	for (int i = 1; i < 4; i += 2) {
		glm::vec2 p1 = rectSegments[i][0];
		glm::vec2 p2 = rectSegments[i][1];

		if (p1.y != p2.y) { // ���� ����			
			float intersectionX = (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;	//dy * m(������ ����) + x = Px			
			if ((p1.x == p2.x and point.x < p1.x) || point.x <= intersectionX) {
				std::cout << i + 1 << "���� count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}

		}
		else { //���Ⱑ 0�� ��
			if (point.x < p1.x) {
				std::cout << i + 1 << "���� count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}

		}
	}
	
	std::cout << "x_count = " << x_count << '\n';
	std::cout << "y_count = " << y_count << '\n';
	// Ȧ�A ��Ģ�� ����Ͽ� ���� ���簢�� �ȿ� �ִ��� Ȯ��, 1�̸� ���� 0�̸� �ܺ�
	if ((y_count % 2) == 1 and (x_count % 2) == 1) {
		std::cout << "�簢�� ����" << '\n';
		return true;
	}
	else {
		std::cout << "�簢�� �ܺ�" << '\n';
		return false;
	}
}

//----------------Mouse--------------------------------------------------------------------------------------------------------------
void Mouse(int button, int state, int x, int y) {

	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (600 / 2.0f - y) / (600 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		for (int i = 0; i < 4; ++i) {//�浹����
			if (rect.p[i].x - rect.mouse_check_size < mx and  mx < rect.p[i].x + rect.mouse_check_size and	 
				rect.p[i].y - rect.mouse_check_size < my and my < rect.p[i].y + rect.mouse_check_size) {
				target = i;
				left_button = true;
				//std::cout << "rect[:" << target << "].x =" << rect.p[target].x << '\n';
			}
		}
		//�簢�� ����
		if (IsPointInsideRectangle(glm::vec2(mx, my), rect)) {
			Point_Inside_Rectangle = true;
		}
	
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		target = -1; // target �ʱ�ȭ
		left_button = false;
		Point_Inside_Rectangle = false;
	}


	mouse[0] = mx;
	mouse[1] = my;
	glutPostRedisplay();// ȭ�� �ٽ� �׸��� ��û
}
//----------Motion-------------------------------------------------------------------------------------------------------------------
void Motion(int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (600 / 2.0f - y) / (600 / 2.0f);
	if (left_button == true) {
		rect.p[target].x = mx;
		rect.p[target].y = my;
		InitBuffer(rect);
		//std::cout << "------------------------------------------------------------------" << '\n';
		//std::cout << "rect[:" << target << "].x =" << rect.p[target].x << '\n';
		//std::cout << "rect[:" << target << "].y =" << rect.p[target].y << '\n' << '\n';
		//std::cout << "------------------------------------------------------------------" << '\n';
	}
	else if (Point_Inside_Rectangle) {
		for (int i = 0; i < 4; ++i) {
			rect.p[i].x -= mouse[0] - mx;
			rect.p[i].y -= mouse[1] - my;

		/*	std::cout << "------------------------------------------------------------------" << '\n';
			std::cout << "rect[:" << i << "].x =" << rect.p[i].x << '\n';
			std::cout << "rect[:" << i << "].y =" << rect.p[i].y << '\n' << '\n';
			std::cout << "------------------------------------------------------------------" << '\n';*/
			
		}
		InitBuffer(rect);
	}
	mouse[0] = mx;
	mouse[1] = my;
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

// --------- GLvoid Keyboard(unsigned char key, int x, int y)------------------------------------------------------------------------
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r': case 'R':
		rect.reset();
		InitBuffer(rect);
		break;
	case 'q':case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

// --------- GLvoid drawScene()------------------------------------------------------------------------------------------------------
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);//--- ����� ���� ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);//--- ������ ���������ο� ���̴� �ҷ�����
	{
		glm::mat4 transformMatrix(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));//--- modelTransform ������ ��ȯ �� �����ϱ�
	}
	// x, y �� axis �׸���
	glBindVertexArray(axis.vao);//--- ����� VAO �ҷ�����
	//std::cout << axis.Getname() << '\n';
	//axis.Print_member();
	glLineWidth(3);
	glDrawArrays(GL_LINES, 0, 4);

	//�簢�� ���
	glBindVertexArray(rect.vao);//--- ����� VAO �ҷ�����
	//std::cout << rect.Getname() << '\n';
	//rect.Print_member();
	//�� ���
	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, 4);

	//�������� ����
	glLineWidth(3);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	//{
	//	std::cout << "------------------------------------------------------------------" << '\n';
	//	std::cout << "rect[:" << target << "].x =" << rect.p[target].x << '\n';
	//	std::cout << "rect[:" << target << "].y =" << rect.p[target].y << '\n' << '\n';
	//	std::cout << "------------------------------------------------------------------" << '\n';
	//}

	//for (int i = 0; i < 1; ++i) {
	//
	//	glBindVertexArray(rect[i].vao);								//--- ����� VAO �ҷ�����
	//	//{
	//	//	glm::mat4 transformMatrix(1.0f);
	//	//	transformMatrix = glm::translate(transformMatrix, glm::vec3(rect[i].x, rect[i].y, 0.0f));
	//	//	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	//	//	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�
	//
	//	//}
	//
	//	//if (choice == 0) {
	//	//	//glDrawArrays(GL_TRIANGLE_STRIP, 0, rect[i].pointnum + 1);
	//	//	if (rect[i].pointnum == 1) {
	//	//		glPointSize(5);
	//	//		glDrawElements(GL_POINTS, rect[i].indexnum, GL_UNSIGNED_INT, 0);
	//	//	}
	//	//	else if (rect[i].pointnum == 2 ) {
	//	//		glLineWidth(5);
	//	//		glDrawElements(GL_LINES, rect[i].indexnum , GL_UNSIGNED_INT, 0);
	//
	//	//	}
	//	//	else
	//	//		glDrawElements(GL_TRIANGLES, rect[i].indexnum, GL_UNSIGNED_INT, 0);
	//	//}
	//	//else {
	//	//	glLineWidth(5);
	//	//	glDrawArrays(GL_LINE_LOOP, 0, rect[i].pointnum + 1);
	//
	//	//}
	//	//std::cout << "��� �� rect[" << i << "]: (" << rect[i].x << ", " << rect[i].y << ")" << std::endl;
	//	//{
	//	//	GLint bufferSize = 0;
	//	//	glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[0]);
	//	//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	//	//	// ���� ���ε��� VBO�� ũ�⸦ ������
	//	//	GLfloat* data = new GLfloat[bufferSize / sizeof(GLfloat)];
	//	//	glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, data);
	//	//	// VBO�� �����͸� �о��
	//	//	for (int i = 0; i < bufferSize / sizeof(GLfloat); i++) {
	//	//		std::cout << "Data[" << i << "]: " << data[i] << std::endl;
	//	//	}
	//	//	delete[] data;
	//	//	// ������ ��� �� �޸𸮸� ����
	//	//}
	//
	//}

	glutSwapBuffers();//--- ȭ�鿡 ����ϱ�
}

// ---------void reset()-------------------------------------------------------------------------------------------------------------
void reset() {	
	shape& s = rect;
	std::cout << "InitBuffer ����" << '\n';
	s.Setname("Rectangle");
	InitBuffer(s);
	std::cout << "InitBuffer ����" << '\n';


	axis.color.clear();
	for (int i = 0; i < 4; i++) {
		if (i / 2 == 0) {
			axis.p[i].x = i % 2 == 0 ? -1.0f : 1.0f;
		}
		else axis.p[i].x = 0.0f;

		if (i / 2 == 1) {
			axis.p[i].y = i % 2 == 0 ? -1.0f : 1.0f;
		}
		else axis.p[i].y = 0.0f;

		axis.p[i].z = 0.0f;

		axis.color.push_back(0.0f);
		axis.color.push_back(0.0f);
		axis.color.push_back(0.0f);
	}
	axis.Setname("Axis");
	InitBuffer(axis);
	
	
}

//--- �ٽñ׸��� �ݹ� �Լ�-----------------------------------------------------------------------------------------------------------
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//---------------------mian�Լ�--------------------------------------------------------------------------
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 0);
	glutInitWindowSize(800, 600);
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