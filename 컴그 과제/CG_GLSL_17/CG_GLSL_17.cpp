﻿#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <format>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것r
//-------------------------------------------------------------------------------------------------------------------------
//glew32.lib freeglut.lib 
//-------------------------------------------------------------------------------------------------------------------------
const std::string Guide[]{
	"--------------------------------------------------------------------------------------------------"
	"각 도형의 이동 : x 축 , y 축 , z 축",
	"각 도형의 제자리에서 신축 : 확대 , 축소",
	"각 도형의 원점에 대하여 신축 : 확대 , 축소 원점에 대하여 신축하면 위치도 같이 이동된다",
	"전체 도형의 이동 : 키보드 명령에 따라 y 축을 제외한 x 축과 z 축 , 두 도형을 위 아래로 이동",
	"h/H: 은면제거 적용/해제",
	"w/W: 와이어 객체/솔리드 객체",
	"s/S: 원점에 대축",
	"---애니메이션 구현---",
	"키보드 r: xz 평면에 스파이럴을 그리고 , 그 스파이럴 위치에 따라 객체 이동 애니메이션",
	"키보드 t: 현재 자리에서 두 도형이 원점으로 이동 ->  제자리로 다시 이동하는 애니메이션",
	"키보드 1: 두 도형이 중심점을 통과하며 상대방의 자리로 이동하는 애니메이션",
	"키보드 2: 두 도형이 공전하면서 상대방의 자리로 이동하는 애니메이션",
	"키보드 3: 두 도형이 한 개는 위로 , 다른 도형은 아래로 이동하면서 상대방의 자리로 이동하는 애니메이션",
	"--------------------------------------------------------------------------------------------------",
	"p : 초기화 하기",
	"q : 프로그램 종료",
	"--------------------------------------------------------------------------------------------------"
};
//-------------------------------------------------------------------------------------------------------------------------


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
	glm::vec3 p[6]; // 각 점의 좌표
	GLuint vao;
	GLuint vbo[2];
	std::vector<float> color;

	int* points;
	//생성자
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
	glm::vec3 p[8]; //사각형의 각 점의 좌표
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	float dx, dy;//마우스
	int indexnum;	
	std::vector<float> color;
	//------------------------------
	int* points;
	//생성자
	shapecube() {
		points = new int;
		name = "None";
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		ebo = 0;
		color.clear();
		dx = 0.0f;
		dy = 0.0f;
		indexnum = 0;
		for (int i = 0; i < 8; ++i) {
			p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f) + dx);	
			p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f) + dy);	
			p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);	

			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
		}
	}

	//소멸자
	~shapecube() {
		delete points;
	}
	//멤버 함수(method)
	void reset() {
		for (int i = 0; i < 8; ++i) {
			p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f));
			p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f));
			p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);
		}
	}
	/*void Setname(const std::string& name) {
		name = name;
	}*/

	std::string Getname() {
		return name;
	}

	void Print_member() {
		std::cout << "------------------------------------------------------------------" << '\n';
		std::cout << "정점 위치들" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = (" << p[i].x << ", " << p[i].y << ", " << p[i].z << ")" << '\n';
		}
		std::cout << "색상" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << color.at(i * 3) << ", " << color.at(i * 3 + 1) << ", " << color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}

};
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
		name = "None";
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		ebo = 0;
		color.clear();
		dx = 0.0f;
		dy = 0.0f;
		indexnum = 0;

		for (int i = 0; i < 5; ++i) {
			if (i == 0) {
				p[i].x = (0.0f + dx);		//x - cos
				p[i].y = (1 + dy);		//y - sin
				p[i].z = (0.0f);		//z
			}
			else {
				p[i].x = ((i / 3 == 0 ? -1.0f : 1.0f) + dx);	//x - cos
				p[i].y = (-1.0f + dy);		//y - sin
				p[i].z = (i % 3 == 1 ? 1.0f : -1.0f);	//z
			}
			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
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
				p[i].x = (0.0f);		//x - cos
				p[i].y = (1);		//y - sin
				p[i].z = (0.0f);		//z
			}
			else {
				p[i].x = ((i / 3 == 0 ? -1.0f : 1.0f));	//x - cos
				p[i].y = (-1.0f);		//y - sin
				p[i].z = (i % 3 == 1 ? 1.0f : -1.0f);	//z
			}
		}
	}
	//void Setname(const std::string& name) {
	//	name = name;
	//}

	std::string Getname() {
		return name;
	}

	void Print_member() {
		std::cout << "------------------------------------------------------------------" << '\n';
		std::cout << "정점 위치들" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = (" << p[i].x << ", " << p[i].y << ", " << p[i].z << ")" << '\n';
		}
		std::cout << "색상" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << color.at(i * 3) << ", " << color.at(i * 3 + 1) << ", " << color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}

};
class  shap_circle_spiral {
public:
	glm::vec3 p[1];
	GLuint vao;
	GLuint vbo[2];
	std::vector<float> color;
	std::vector<float> vertex;
	//------------------
	float dx, dy;
	int drawpoint;
	int pointnum;
	int* points;
	
	//생성자
	shap_circle_spiral() {
		points = new int;
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		color.clear();
		drawpoint = 0;
		pointnum = 0;
		p->x = 0.0f;
		p->y = 0.0f;
		p->z = 0.0f;

		color.push_back(randomnum(0.0f, 1.0f));
		color.push_back(randomnum(0.0f, 1.0f));
		color.push_back(randomnum(0.0f, 1.0f));
		
	}
};


//--- 전역변수 -------------------------------------------------------------------------------------------------------
//--------------------xyz-----------------------------------------------------------
const glm::vec3 x_axis{ 1.0f,0.0f,0.0f }; //x축
const glm::vec3 y_axis{ 0.0f,1.0f,0.0f }; //y축
const glm::vec3 z_axis{ 0.0f,0.0f,1.0f }; //z축
glm::vec3 zero_scale{ 0.3f,0.3f,0.3f }; //원점에 대한 신축
shapecube cube;
shapep square_horn;
linexyz xyz;//xyz축 그리기
shap_circle_spiral s_circle_spiral; // 원 스파이럴 그리기
int target{ 1 };  //선택한 도형 처음 도형은 정육면체
int targetglu{ 3 };//선택한 glu 도형 처음 도형은 구
bool DEPTH_T{ true }; // 은면제거
bool t_or_l{ false };//면 또는 선
bool b_circle_spiral{ false };// 회전
bool left_button{ false }; //좌클릭
bool b_animation{ false };
int all_animation{ 0 }; // 애니메인션 0:x,1:1, 2:2, 3:3, 4:t, 5:r
float rad{ 0.0f };
float drawr_obj{ 0.0f };
float drawr_glu{ 0.0f };
float rad_obj{ 0.0f };
float rad_glu{ 0.0f };
float sz_syz = 1.0f;
float dz = 0.01f;
int dzz = 1;
//-----------------------------------------------------------
glm::vec3 translate_origin_glu{ 0.0f };//glu초기값,0.0f, 0.0f, -0.9f
glm::vec3 translate_origin_obj{ 0.0f };//obj초기값,0.0f, 0.0f, 0.9f
GLfloat degree{ 0.0f }; // 좌클릭시 회전각
glm::vec3 rotate{ 0.0f }; // 축을 기준으로 공전 또는 자전
glm::vec3 rotateobjglu{ 0.0f }; // 축을 기준으로 공전 또는 자전
glm::vec3 rotateobj{ 0.0f }; // 축을 기준으로 공전 또는 자전

//int objglu{ 3 }; // 1 클릭시 glu만 선택 자전
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
//--------------------InitBuffer_line_xyz---------------------------------------
void InitBuffer_line_xyz(linexyz& s) {
	//버퍼 생성
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

	}

	vertex.clear();
	color.clear();
}
//--------------------InitBuffer_s_circle_spiral---------------------------------------
void InitBuffer_s_circle_spiral(shap_circle_spiral& s) {
	//버퍼 생성
	std::vector<float> vertex;
	std::vector<float> color;
	float rad = 0;
	float drawr = 0;
	while (rad < 360.0f * 5 + 1)
	{
		vertex.push_back(s.p->x + cos(glm::radians(rad)) * drawr);	//x
		vertex.push_back(s.p->y = 0.0f);	//y
		vertex.push_back(s.p->z + sin(glm::radians(rad)) * drawr);	//z
		

		color.push_back(s.color.at(0));	//r
		color.push_back(s.color.at(1));	//g
		color.push_back(s.color.at(2));	//b
		drawr += 0.01f;
		rad += 15.0f;
		//static_cast<float>(rad);
	}
	

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

	}
	s.drawpoint = 1;
	s.pointnum = vertex.size() / 3 ;

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

//----------Timer_origin_loop-------------------------------------------------------------------------------------
void origin_loop(int value) {
	if (all_animation == 4) {
		static int dz = 1; // 방향 변경 변수
		translate_origin_obj.z -= 0.01f * dz;
		translate_origin_glu.z += 0.01f * dz;

		if (translate_origin_obj.z <= 0.0f || translate_origin_obj.z >= 0.9f) {
			dz *= -1; // 방향 변경
		}
	}
	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, origin_loop, 0);
}
//----------Timer_exchange-------------------------------------------------------------------------------------
void Timer_exchange(int value) {
	if (all_animation == 1) {
		static int dz = 1; // 방향 변경 변수
		translate_origin_obj.z -= 0.01f * dz;
		translate_origin_glu.z += 0.01f * dz;

		if (translate_origin_obj.z <= -0.9f || translate_origin_obj.z >= 0.9f) {
			dz *= -1; // 방향 변경
		}
	}
	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, Timer_exchange, 0);
}
//----------Timer_x_rotate-------------------------------------------------------------------------------------
void Timer_y_rotate(int value) {
	if (all_animation == 2) {
		rotate.y += 1.0f;

		// 현재 각도 값을 정수로 변환
		int currentAngle = (int)rotate.y;

		if (currentAngle == 180 || currentAngle == 360) {
			b_animation = !b_animation;
		}

		if (rotate.y >= 360.0f) {
			rotate.y -= 360.0f;
		}
	}

	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, Timer_y_rotate, 0);
}
//----------Timer_y_rotate-------------------------------------------------------------------------------------
void Timer_x_rotate(int value) {
	if (all_animation == 3) {
		rotate.x += 1.0f;

		// 현재 각도 값을 정수로 변환
		int currentAngle = (int)rotate.x;

		if (currentAngle == 180 || currentAngle == 360) {
			b_animation = !b_animation;
		}

		if (rotate.x >= 360.0f) {
			rotate.x -= 360.0f;
		}
	}
	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, Timer_x_rotate, 0);
}
//----------circle_spiral()------------------------------------------------------------------------------------
void Timer_circle_spiral(int value) {
	//translate_origin_glu{ 0.0f };//glu초기값,0.0f, 0.0f, -0.9f
	//translate_origin_obj{ 0.0f };//obj초기값,0.0f, 0.0f, 0.9f
	if (all_animation == 5) {

		if (rad_glu < 360.0f * 5) {
			std::cout << "translate_origin_obj = {" << translate_origin_obj.x  << ", " << translate_origin_obj.y << ", " << translate_origin_obj.z << "} " << "\n";
			drawr_obj += 0.01f;
			rad_obj += 15.0;

			drawr_glu += 0.01f;
			rad_glu += 15.0;

			translate_origin_obj.x = cos(glm::radians(rad_obj)) * drawr_obj;
			translate_origin_obj.z = sin(glm::radians(rad_obj)) * drawr_obj;
			
			translate_origin_glu.x = cos(glm::radians(rad_glu)) * drawr_glu;
			translate_origin_glu.z = sin(glm::radians(rad_glu)) * drawr_glu;
			if (rad_glu >= 360.0f * 5) {
				translate_origin_glu = glm::vec3{ 0.0f };
				drawr_glu = 0.0f;
				rad_glu = 0.0f;
			}
			if (rad_obj >= 360.0f * 5) {
				translate_origin_obj = glm::vec3{ 0.0f };
				drawr_obj = 0.0f;
				rad_obj = 0.0f;
			}
			//translate_origin_obj.x += 0.002f;
			//rotate.y += 15.0f;
		}
	

	}
	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(100, Timer_circle_spiral, 0);
}

void Timer_circle_spiral_pointnum(int value) {
	s_circle_spiral.drawpoint++;

	glutPostRedisplay();
	if (s_circle_spiral.drawpoint < s_circle_spiral.pointnum)
		glutTimerFunc(10, Timer_circle_spiral_pointnum, 0);
}

void Timer_zero_scale(int value) {
	//glm::vec3 zero_scale{ 0.3f,0.3f,0.3f }; //원점에 대한 신축
	//float sz_syz = 1.0f;
	//float dz = 0.01f;
	
	if (all_animation == 6) {
		
	
		zero_scale.x += dz;
		zero_scale.y += dz;
		zero_scale.z += dz;

		if (zero_scale.x > 1.0f) {
			dz *= -1;
		}
		else  if (zero_scale.x < 0.1f) {
			dz *= -1;
		}

		dz *= dzz;
	}
		

	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(50, Timer_zero_scale, 0);
}

//--------keyboard----------------------------------------
//"키보드 r: xz 평면에 스파이럴을 그리고 , 그 스파이럴 위치에 따라 객체 이동 애니메이션",
//"키보드 t: 현재 자리에서 두 도형이 원점으로 이동  제자리로 다시 이동하는 애니메이션",
//"키보드 1: 두 도형이 중심점을 통과하며 상대방의 자리로 이동하는 애니메이션",
//"키보드 2: 두 도형이 공전하면서 상대방의 자리로 이동하는 애니메이션",
//"키보드 3: 두 도형이 한 개는 위로 , 다른 도형은 아래로 이동하면서 상대방의 자리로 이동하는 애니메이션", x 축 회전
GLvoid Keyboard(unsigned char key, int x, int y) {
	std::vector<float> vertex;
	switch (key)
	{
	case'H':case'h'://h: 은면제거 적용/해제
		DEPTH_T = DEPTH_T == true ? false : true;
		break;

	case'W':case'w'://w/W: 와이어 객체/솔리드 객체
		t_or_l = t_or_l == true ? false : true;
		break;

	case'Q':case'q':
		exit(0);
		break;

	case'S':case's':
		all_animation = 6;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(50, Timer_zero_scale, 0);
		break;

	case'P':case'p'://p : 초기 위치로 리셋(자전 애니메이션도 멈추기
		cube.reset();
		b_circle_spiral = false;
		rotate.x = 0.0f;
		rotate.y = 0.0f;
		degree = 0.0f;
		rotateobj.x = 0.0f;
		rotateobj.y = 0.0f;
		rotateobjglu.x = 0.0f;
		rotateobjglu.y = 0.0f;
		all_animation = 0;
		translate_origin_glu = { 0.0f,0.0f,-0.9f };
		translate_origin_obj = { 0.0f,0.0f,0.9f };
		target = 1; // 정육면체
		targetglu = 3; // 원뿔

		InitBuffer_cube(cube);
		InitBuffer_square_horn(square_horn);
		break;

	case'R':case'r': //"키보드 r: xz 평면에 스파이럴을 그리고 , 그 스파이럴 위치에 따라 객체 이동 애니메이션",
		all_animation = 5;
		translate_origin_glu = { -0.12f,0.0f,0.0f };
		drawr_glu = 0.12f ;
		rad_glu = 180.0f;
		translate_origin_obj = { 0.0f,0.0f,0.0f };
		glutTimerFunc(10, Timer_circle_spiral_pointnum, 0);;

		b_animation = b_animation == true ? false : true;
		b_circle_spiral = b_circle_spiral == true ? false : true;
		glutTimerFunc(100, Timer_circle_spiral, 0);
		break;

	case'T':case't'://"키보드 t: 현재 자리에서 두 도형이 원점으로 이동 -> 제자리로 다시 이동하는 애니메이션",
		all_animation = 4;
		translate_origin_glu = { 0.0f,0.0f,-0.9f };
		translate_origin_obj = { 0.0f,0.0f,0.9f };
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, origin_loop, 0);
		break;
	case'1'://"키보드 1: 두 도형이 중심점을 통과하며 상대방의 자리로 이동하는 애니메이션"
		all_animation = 1;
		translate_origin_glu = { 0.0f,0.0f,-0.9f };
		translate_origin_obj = { 0.0f,0.0f,0.9f };
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_exchange, 0);
		break;
	case'2'://"키보드 2: 두 도형이 공전하면서 상대방의 자리로 이동하는 애니메이션",
		all_animation = 2;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_y_rotate, 0);
		break;
	case'3'://"키보드 3: 두 도형이 한 개는 위로 , 다른 도형은 아래로 이동하면서 상대방의 자리로 이동하는 애니메이션", x 축 회전
		all_animation = 3;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_x_rotate, 0);
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
		cube.dx -= 0.01f;
		square_horn.dx -= 0.01f;
		break;

	case GLUT_KEY_RIGHT:// →
		cube.dx += 0.01f;
		square_horn.dx += 0.01f;
		break;

	case GLUT_KEY_UP:// ↑
		cube.dy += 0.01f;
		square_horn.dy += 0.01f;
		break;

	case GLUT_KEY_DOWN:// ↓
		cube.dy -= 0.01f;
		square_horn.dy -= 0.01f;
		break;
	}
	InitBuffer_cube(cube);
	InitBuffer_square_horn(square_horn);
	glutPostRedisplay();
}

// ---- 그리기 콜백 함수---------------------------------------------------------------------------------------------------
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- 변경된 배경색 설정 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- 렌더링 파이프라인에 세이더 불러오기

	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

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

		qobj = gluNewQuadric(); // 객체 생성하기
		gluQuadricDrawStyle(qobj, GLU_LINE); // 도형 스타일
		gluQuadricNormals(qobj, GLU_SMOOTH); //? 생략 가능
		gluQuadricOrientation(qobj, GLU_OUTSIDE); //? 생략 가능


		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);

		if (all_animation == 6) {
			transformMatrix = glm::scale(transformMatrix, glm::vec3(zero_scale));
		}
		
		
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.x), x_axis);
		transformMatrix = glm::translate(transformMatrix, glm::vec3(translate_origin_glu)); // 원점, 제자리이동반복 ,0.0f, 0.0f, -0.9f

		transformMatrix = glm::rotate(transformMatrix, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));//마우스
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobjglu.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobjglu.x), glm::vec3(1.0f, 0.0f, 0.0f));

	
		transformMatrix = glm::scale(transformMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
		if (all_animation == 5) {
			transformMatrix = glm::scale(transformMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		}
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

		if (targetglu == 3)
			gluSphere(qobj, 0.5, 50, 50); // 구 객체 만들기

		else if (targetglu == 4)
			gluCylinder(qobj, 1.0, 0.0, 2.0, 20, 8);


	}

	{

		glm::mat4 transformMatrix(1.0f);


		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);

		if (all_animation == 6) {
			transformMatrix = glm::scale(transformMatrix, glm::vec3(zero_scale));
		}


		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.x), x_axis);
		transformMatrix = glm::translate(transformMatrix, glm::vec3(translate_origin_obj));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));//마우스
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobj.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotateobj.x), glm::vec3(1.0f, 0.0f, 0.0f));

		transformMatrix = glm::scale(transformMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		if (all_animation == 5) {
			transformMatrix = glm::scale(transformMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
		}
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
	else if (target == 1) {
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

	//s_circle_spiral
	if (all_animation == 5) {
		glBindVertexArray(s_circle_spiral.vao);								//--- 사용할 VAO 불러오기
		{

			glm::mat4 transformMatrix(1.0f);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), x_axis);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);
			transformMatrix = glm::scale(transformMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
			glLineWidth(2);
			glDrawArrays(GL_LINE_STRIP, 0, s_circle_spiral.pointnum);

		}
	}



	glutSwapBuffers();									//--- 화면에 출력하기
}


void reset() {
	//cube.Setname("cube");
	//square_horn.Setname("square_horn");
	cube.dx = 0.0f;
	cube.dy = 0.0f;
	square_horn.dx = 0.0f;
	square_horn.dy = 0.0f;

	translate_origin_glu = {0.0f,0.0f,-0.9f};
	translate_origin_obj = {0.0f,0.0f,0.9f};
	InitBuffer_cube(cube);
	InitBuffer_square_horn(square_horn);

	InitBuffer_line_xyz(xyz);
	InitBuffer_s_circle_spiral(s_circle_spiral);
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void ReadObj(FILE* path) {
	char count[128];
	int vertexnum = 0;
	int facenum = 0;
	int uvnum = 0;
	//--- 1. 전체 버텍스 개수 및 삼각형 개수 세기
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
	//--- 2. 메모리 할당
	glm::vec3* vertex = new glm::vec3[vertexnum];
	glm::vec3* face = new glm::vec3[facenum];
	glm::vec3* uvdata = new glm::vec3[facenum];
	glm::vec3* uv = new glm::vec3[uvnum];
	char bind[128];
	//--- 3. 할당된 메모리에 각 버텍스, 페이스, uv 정보 입력
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
	//--- 필요한 경우 읽어온 값을 전역 변수 등에 저장
	delete[] vertex;
	delete[] face;
	delete[] uvdata;
	delete[] uv;
	vertex;
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


	reset();
	std::cout << " reset 초기화 완료" << '\n';

	for (std::string s : Guide) {
		std::cout << s << '\n';
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력
	glutSpecialFunc(specialkeyborad);
	glutMouseFunc(Mouse); // 마우스 입력
	glutMotionFunc(Motion); // 마우스 입력

	glutMainLoop();
}