#include <iostream>
#include <fstream>
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
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
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
class shapecube {
private:
	std::string name;
public:
	//------------------------------
	glm::vec3 p[8]; //사각형의 각 점의 좌표
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	float dx, dy;
	int indexnum;	
	std::vector<float> color;
	//------------------------------
	int* points;
	//생성자
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
		
		//for (int i = 0; i < 4; ++i) {
		//	this->p[i].x = i % 3 == 0 ? -0.5f : 0.5f;
		//	this->p[i].y = i / 2 == 0 ? -0.5f : 0.5f;
		//	this->p[i].z = 0.0f;
		//
		//	this->color.push_back(i == 0 ? 1.0f : 0.0f);
		//	this->color.push_back(i == 1 ? 1.0f : 0.0f);
		//	this->color.push_back(i == 2 ? 1.0f : 0.0f);
		//}
		for (int i = 0; i < 8; ++i) {
			this->p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f) + this->dx);	
			this->p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f) + this->dy);	
			this->p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);	

			this->color.push_back(randomnum(0.0f, 1.0f));
			this->color.push_back(randomnum(0.0f, 1.0f));
			this->color.push_back(randomnum(0.0f, 1.0f));
		}
	}

	//소멸자
	~shapecube() {
		delete points;
	}
	//멤버 함수(method)
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
		std::cout << "정점 위치들" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = (" << this->p[i].x << ", " << this->p[i].y << ", " << this->p[i].z << ")" << '\n';
		}
		std::cout << "색상" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << this->color.at(i * 3) << ", " << this->color.at(i * 3 + 1) << ", " << this->color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}

};
//------------------------------------------------------------------------------------------------------------
class shapep {
private:
	std::string name;
public:
	glm::vec3 p[5]; //사각형의 각 점의 좌표
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	int indexnum;
	std::vector<float> color;
	//------------------
	float dx, dy;
	int* points;
	//생성자
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

		//for (int i = 0; i < 4; ++i) {
		//	this->p[i].x = i % 3 == 0 ? -0.5f : 0.5f;
		//	this->p[i].y = i / 2 == 0 ? -0.5f : 0.5f;
		//	this->p[i].z = 0.0f;
		//
		//	this->color.push_back(i == 0 ? 1.0f : 0.0f);
		//	this->color.push_back(i == 1 ? 1.0f : 0.0f);
		//	this->color.push_back(i == 2 ? 1.0f : 0.0f);
		//}
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

	//소멸자
	~shapep () {
		delete points;
	}
	//멤버 함수(method)
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
		std::cout << "정점 위치들" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = (" << this->p[i].x << ", " << this->p[i].y << ", " << this->p[i].z << ")" << '\n';
		}
		std::cout << "색상" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << this->color.at(i * 3) << ", " << this->color.at(i * 3 + 1) << ", " << this->color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}

};

//--- 전역변수 -------------------------------------------------------------------------------------------------------
shapecube cube;
shapep square_horn;
int target{ 0 };  //선택한 도형
bool DEPTH_T{ true }; // 은면제거
bool t_or_l{ false };//면 또는 선
bool rotateXY{ false };// 회전
bool left_button{ false }; //좌클릭
GLfloat degree{ 0.0f }; // 좌클릭시 회전각
float rotateX{ 1.0f };//X 축 회전
float rotateY{ 1.0f };//Y 축 회전
//float alldx{ 0.0f };//x축이동변수
//float alldy{ 0.0f };//y축이동변수
glm::vec3 trans{ 0.0f, 0.0f, 0.0f };

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
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
//-------------------------------------------------------------------------------------------------------
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");					//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);			//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);								//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
//--- 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");					//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);			//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);		//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);								//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
//--- 버퍼 생성하고 데이터 받아오기
void make_shaderProgram()
{

	make_vertexShaders();
	make_fragmentShaders();

	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
	//return shaderProgramID;

}
//-------------------------------------------------------------------------------------------------------

//--------------------InitBuffer_cube---------------------------------------
void InitBuffer_cube(shapecube& s) {
	//버퍼 생성
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

	//gpu 버퍼에 저장하기
	{
		glGenVertexArrays(1, &s.vao); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(s.vao);		//--- VAO를 바인드하기
		glGenBuffers(2, s.vbo);		//--- 2개의 VBO를 지정하고 할당하기

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
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
	//버퍼 생성
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
	//---5-바닥
	index.push_back(1);
	index.push_back(4);
	index.push_back(2);
	//---5-바닥
	index.push_back(2);
	index.push_back(4);
	index.push_back(3);

	s.indexnum = index.size();

	//gpu 버퍼에 저장하기
	{
		glGenVertexArrays(1, &s.vao); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(s.vao);		//--- VAO를 바인드하기
		glGenBuffers(2, s.vbo);		//--- 2개의 VBO를 지정하고 할당하기

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &s.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), index.data(), GL_STATIC_DRAW);

	}


	vertex.clear();
	color.clear();
	index.clear();


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

	glutPostRedisplay(); // 화면 다시 그리기 요청
}

//----------Motion----------------------------------------
void Motion(int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (left_button == true) {

	}

	glutPostRedisplay(); // 화면 다시 그리기 요청
}
//----------TimerrotateX-------------------------------------------------------------------------------------
void TimerrotateX(int value) {
	rotateX += 1.0f;
	if (rotateX > 360.0f) {
		rotateX -= 360.0f;
	}
	glutPostRedisplay();
	if (rotateXY)
		glutTimerFunc(10, TimerrotateX, 0);
}
//----------TimerrotateY-------------------------------------------------------------------------------------
void TimerrotateY(int value) {
	rotateY += 1.0f;
	if (rotateY > 360.0f) {
		rotateY -= 360.0f;
	}
	glutPostRedisplay();
	if (rotateXY)
		glutTimerFunc(10, TimerrotateY, 0);
}
//--------keyboard----------------------------------------
GLvoid Keyboard(unsigned char key, int x, int y) {
	std::vector<float> vertex;
	switch (key)
	{
		//c: 육면체 
	case'C':case'c':
		target = 1;
		break;
		//p: 사각 뿔 
	case'T':case't':
		target = 0;
		break;
		//h: 은면제거 적용/해제
	case'H':case'h':
	
		DEPTH_T = DEPTH_T == true ? false : true;
		//std::cout << "DEPTH_T = " << DEPTH_T << "\n";
		break;
		//w/W: 와이어 객체/솔리드 객체
	case'W':case'w':
		t_or_l = t_or_l == true ? false : true;
		/*std::cout << "t_or_l = " << t_or_l << "\n";*/
		break;
		//x / X: x축을 기준으로 양 / 음 방향으로 회전 애니메이션(자전)
	case'X':case'x':
		rotateXY = rotateXY == true ? false : true;
		//if (rotateXY) {
		//	rotateXY = false;
		//}
		//else {
		//	rotateXY = true;
		//}
		glutTimerFunc(10, TimerrotateX, 0);

		break;
		//y / Y : y축을 기준으로 양 / 음 방향으로 회전 애니메이션(자전)
	case'Y':case'y':
		
		rotateXY = rotateXY == true ? false : true;
		
		glutTimerFunc(10, TimerrotateY, 0);
		break;
		//s : 초기 위치로 리셋(자전 애니메이션도 멈추기
	case'S':case's':
		rotateXY = false;
		rotateX = 0.0f;
		rotateY = 0.0f;
		trans = { 0.0f,0.0f ,0.0f };

		InitBuffer_cube(cube);
		InitBuffer_square_horn(square_horn);
		break;
	case'Q':case'q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}
//← / → / ↑ / ↓ : 좌 / 우 / 상 / 하로 객체를 이동한다. (x축, y축으로 이동)
GLvoid specialkeyborad(int key, int x, int y) {
	std::cout << key << "입력받음." << '\n';
	switch (key)
	{
	case GLUT_KEY_LEFT:// ←
		trans.x -= 0.01f;
		//std::cout << "왼쪽" << "\n";
		break;

	case GLUT_KEY_RIGHT:// →
		trans.x += 0.01f;
		break;

	case GLUT_KEY_UP:// ↑
		trans.y += 0.01f;
		break;

	case GLUT_KEY_DOWN:// ↓
		trans.y -= 0.01f;
		break;
	}
	/*InitBuffer_cube(cube);
	InitBuffer_square_horn(square_horn);*/
	glutPostRedisplay();
}

// ---- 그리기 콜백 함수---------------------------------------------------------------------------------------------------
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- 변경된 배경색 설정 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- 렌더링 파이프라인에 세이더 불러오기

	if (DEPTH_T) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	{
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, trans);

		transformMatrix = glm::scale(transformMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));//마우스
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

	}
	//피라미드
	if (target == 0) {
		glBindVertexArray(square_horn.vao);								//--- 사용할 VAO 불러오기
		//면으로 출력
		if (t_or_l)
		{
			glDrawElements(GL_TRIANGLES, square_horn.indexnum, GL_UNSIGNED_INT, 0);	//큐브 1면 출력
		}
		else
			//선으로 출력
		{
			glLineWidth(6);
			for (int j = 0; j < 4; j++) {
				glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(j * 3 * sizeof(unsigned int)));	//큐브 1면 출력
			}
		}
	}
	//정육면체
	else {
		glBindVertexArray(cube.vao);								//--- 사용할 VAO 불러오기
		//면으로 출력
		if (t_or_l)
		{
			glDrawElements(GL_TRIANGLES, cube.indexnum, GL_UNSIGNED_INT, 0);	//큐브 출력
		}
		//선으로 출력
		else
		{
			for (int j = 0; j < 12; j++) {
				glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(j * 3 * sizeof(unsigned int)));	//큐브 1면 출력
			}
		}
	}






	//std::cout << "출력 후 rect[" << i << "]: (" << rect[i].x << ", " << rect[i].y << ")" << std::endl;
	//{
	//	GLint bufferSize = 0;
	//	glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[0]);
	//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	//	// 현재 바인딩된 VBO의 크기를 가져옴
	//	GLfloat* data = new GLfloat[bufferSize / sizeof(GLfloat)];
	//	glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, data);
	//	// VBO의 데이터를 읽어옴
	//	for (int i = 0; i < bufferSize / sizeof(GLfloat); i++) {
	//		std::cout << "Data[" << i << "]: " << data[i] << std::endl;
	//	}
	//	delete[] data;
	//	// 데이터 출력 후 메모리를 해제
	//}



	glutSwapBuffers();									//--- 화면에 출력하기
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
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Example9");
	//--- GLEW 초기화하기e
	glewExperimental = GL_TRUE;
	glewInit();
	std::cout << " GLEW 초기화 완료" << '\n';

	make_shaderProgram();

	{
		std::ifstream in{ "CG_GLSL_15_1.cpp" };

		if (!in) {
			std::cout << "파일 읽기 실패" << '\n';
		}
		else {
			std::cout << "파일 읽기 성공" << '\n';

		}
		std::ofstream out{ "TEST.txt"};
		// 대문자를 소문자로 바꾸어서 TEST.txt 파일에 저장하라.
		char ch;
		in >> std::noskipws;
		while (in >> ch) {
			std::cout << ch;

			
			out << ch;
		}
	}

	reset();
	std::cout << " reset 초기화 완료" << '\n';

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력
	glutSpecialFunc(specialkeyborad);
	glutMouseFunc(Mouse); // 마우스 입력
	glutMotionFunc(Motion); // 마우스 입력

	glutMainLoop();
}