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
	"y: y 축에 대하여 자전한다 멈춘다",
	"t: 육면체의 윗면 애니메이션 시작 정지윗면의 가운데 축을 중심으로 회전한다",
	"s: 육면체의 옆면을 연다 닫는다",
	"b: 육면체의 뒷면을 연다 닫는다",

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
//class shapecube {
//private:
//	 std::string name;
//public:
//	//------------------------------
//	glm::vec3 p[8]; //사각형의 각 점의 좌표
//	GLuint vao;
//	GLuint vbo[2];
//	GLuint ebo;
//	float dx, dy;//마우스
//	int indexnum;	
//	std::vector<float> color;
//	//------------------------------
//	int* points;
//	//생성자
//	shapecube() {
//		points = new int;
//		name = "None";
//		vao = 0;
//		vbo[0] = 0;
//		vbo[1] = 0;
//		ebo = 0;
//		color.clear();
//		dx = 0.0f;
//		dy = 0.0f;
//		indexnum = 0;
//		for (int i = 0; i < 8; ++i) {
//			p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f) + dx);	
//			p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f) + dy);	
//			p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);	
//
//			color.push_back(randomnum(0.0f, 1.0f));
//			color.push_back(randomnum(0.0f, 1.0f));
//			color.push_back(randomnum(0.0f, 1.0f));
//		}
//	}
//
//	//소멸자
//	~shapecube() {
//		delete points;
//	}
//	//멤버 함수(method)
//	void reset() {
//		for (int i = 0; i < 8; ++i) {
//			p[i].x = (((i % 4) % 3 == 0 ? -1.0f : 1.0f));
//			p[i].y = ((i / 4 == 0 ? 1.0f : -1.0f));
//			p[i].z = ((i % 4) < 2 ? 1.0f : -1.0f);
//		}
//	}
//	/*void Setname(const std::string& name) {
//		name = name;
//	}*/
//
//	std::string Getname() {
//		return name;
//	}
//
//	void Print_member() {
//		std::cout << "------------------------------------------------------------------" << '\n';
//		std::cout << "정점 위치들" << '\n';
//		for (int i = 0; i < 4; ++i) {
//			std::cout << "p[" << i << "] = (" << p[i].x << ", " << p[i].y << ", " << p[i].z << ")" << '\n';
//		}
//		std::cout << "색상" << '\n';
//		for (int i = 0; i < 4; i++) {
//			std::cout << "color[" << i << "] = (" << color.at(i * 3) << ", " << color.at(i * 3 + 1) << ", " << color.at(i * 3 + 2) << ")" << '\n';
//		}
//		std::cout << "------------------------------------------------------------------" << '\n';
//	}
//
//};
//class shapep {
//private:
//	std::string name;
//public:
//	glm::vec3 p[5]; //사각형의 각 점의 좌표
//	GLuint vao;
//	GLuint vbo[2];
//	GLuint ebo;
//	int indexnum;
//	std::vector<float> color;
//	//------------------
//	float dx, dy;
//	int* points;
//	//생성자
//	shapep () {
//		points = new int;
//		name = "None";
//		vao = 0;
//		vbo[0] = 0;
//		vbo[1] = 0;
//		ebo = 0;
//		color.clear();
//		dx = 0.0f;
//		dy = 0.0f;
//		indexnum = 0;
//
//		for (int i = 0; i < 5; ++i) {
//			if (i == 0) {
//				p[i].x = (0.0f + dx);		//x - cos
//				p[i].y = (1 + dy);		//y - sin
//				p[i].z = (0.0f);		//z
//			}
//			else {
//				p[i].x = ((i / 3 == 0 ? -1.0f : 1.0f) + dx);	//x - cos
//				p[i].y = (-1.0f + dy);		//y - sin
//				p[i].z = (i % 3 == 1 ? 1.0f : -1.0f);	//z
//			}
//			color.push_back(randomnum(0.0f, 1.0f));
//			color.push_back(randomnum(0.0f, 1.0f));
//			color.push_back(randomnum(0.0f, 1.0f));
//		}
//	}
//
//	//소멸자
//	~shapep () {
//		delete points;
//	}
//	//멤버 함수(method)
//	void reset() {
//		for (int i = 0; i < 5; ++i) {
//			if (i == 0) {
//				p[i].x = (0.0f);		//x - cos
//				p[i].y = (1);		//y - sin
//				p[i].z = (0.0f);		//z
//			}
//			else {
//				p[i].x = ((i / 3 == 0 ? -1.0f : 1.0f));	//x - cos
//				p[i].y = (-1.0f);		//y - sin
//				p[i].z = (i % 3 == 1 ? 1.0f : -1.0f);	//z
//			}
//		}
//	}
//	//void Setname(const std::string& name) {
//	//	name = name;
//	//}
//
//	std::string Getname() {
//		return name;
//	}
//
//	void Print_member() {
//		std::cout << "------------------------------------------------------------------" << '\n';
//		std::cout << "정점 위치들" << '\n';
//		for (int i = 0; i < 4; ++i) {
//			std::cout << "p[" << i << "] = (" << p[i].x << ", " << p[i].y << ", " << p[i].z << ")" << '\n';
//		}
//		std::cout << "색상" << '\n';
//		for (int i = 0; i < 4; i++) {
//			std::cout << "color[" << i << "] = (" << color.at(i * 3) << ", " << color.at(i * 3 + 1) << ", " << color.at(i * 3 + 2) << ")" << '\n';
//		}
//		std::cout << "------------------------------------------------------------------" << '\n';
//	}
//
//};
//class  shap_circle_spiral {
//public:
//	glm::vec3 p[1];
//	GLuint vao;
//	GLuint vbo[2];
//	std::vector<float> color;
//	std::vector<float> vertex;
//	//------------------
//	float dx, dy;
//	int drawpoint;
//	int pointnum;
//	int* points;
//	
//	//생성자
//	shap_circle_spiral() {
//		points = new int;
//		vao = 0;
//		vbo[0] = 0;
//		vbo[1] = 0;
//		color.clear();
//		drawpoint = 0;
//		pointnum = 0;
//		p->x = 0.0f;
//		p->y = 0.0f;
//		p->z = 0.0f;
//
//		color.push_back(randomnum(0.0f, 1.0f));
//		color.push_back(randomnum(0.0f, 1.0f));
//		color.push_back(randomnum(0.0f, 1.0f));
//		
//	}
//};
class Mesh {
public:
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	GLuint indexnum;
	Mesh() {
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		ebo = 0;
		indexnum = 0;
	}
};
class Mesh_motion_change{
public:
	glm::vec3 rotate;
	glm::vec3 translate;
	glm::vec3 translate_st;
	glm::vec3 scale;
	float radx, rady , radz;
	Mesh_motion_change() {
		rotate = { 0.0f,0.0f,0.0f };
		translate = { 0.0f,0.0f,0.0f };
		translate_st = { 0.0f,0.0f,0.0f };
		scale = { 1.0f,1.0f,1.0f };
		radx = 0.0f;
		rady = 0.0f;
		radz = 0.0f;
	}
};
//--- 전역변수 -------------------------------------------------------------------------------------------------------
//--------------------xyz-----------------------------------------------------------
const glm::vec3 x_axis{ 1.0f,0.0f,0.0f }; //x축
const glm::vec3 y_axis{ 0.0f,1.0f,0.0f }; //y축
const glm::vec3 z_axis{ 0.0f,0.0f,1.0f }; //z축
const glm::vec3 zero{ 0.0f,0.0f,0.0f }; //z축
Mesh mcube, mpyramid;
Mesh_motion_change m_motion_ch[6];

//shapecube cube;
//shapep square_horn;
linexyz xyz;//xyz축 그리기
int meshface{ 0 };  //선택한 도형 처음 도형은 정육면체
bool DEPTH_T{ true }; // 은면제거
bool t_or_l{ true };//면 또는 선
bool left_button{ false }; //좌클릭
bool b_animation{ false };
int all_animation{ 0 }; // 애니메인션 0:x,1:1, 2:2, 3:3, 4:t, 5:r

//-----------------------------------------------------------
glm::vec3 translate_origin_glu{ 0.0f };//glu초기값,0.0f, 0.0f, -0.9f
glm::vec3 translate_origin_obj{ 0.0f };//obj초기값,0.0f, 0.0f, 0.9f
GLfloat degree{ 0.0f }; // 좌클릭시 회전각
glm::vec3 rotate{ 0.0f }; // 축을 기준으로 공전 또는 자전
glm::vec3 turn_top_rotate{ 0.0f }; // 축을 기준으로 공전 또는 자전
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
void meshface_srt(int& i);

GLvoid Reshape(int w, int h);
void getobjfile(Mesh& mesh, const std::string& objname);
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

//----------Timer_turn_top-------------------------------------------------------------------------------------
void Timer_turn_top(int value) {
	if (all_animation == 4) {
		m_motion_ch[5].translate_st = { 0.0f,1.0f,0.0f };
		m_motion_ch[5].radx += 1.0f;


		if (m_motion_ch[5].radx >= 360.0f) {
			m_motion_ch[5].radx -= 360.0f;
		}
		std::cout << "m_motion_ch[5].radx: " << m_motion_ch[5].radx << "\n";
	}
	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, Timer_turn_top, 0);
}
//----------Timer_turn_face-------------------------------------------------------------------------------------
void Timer_turn_face(int value) {
	if (all_animation == 2) {
		m_motion_ch[2].translate_st = { 0.0f,-1.0f, -1.0f };
		m_motion_ch[2].radx -= 1.0f;

		//if (m_motion_ch[2].radx <= -90.0f) {
		//	m_motion_ch[2].radx += 90.0f;
		//}
	}

	glutPostRedisplay();
	if (b_animation && m_motion_ch[2].radx > -90.0f)
		glutTimerFunc(10, Timer_turn_face, 0);
}
//----------Timer_y_rotate-------------------------------------------------------------------------------------
void Timer_side_up(int value) {
	if (all_animation == 1) {
		m_motion_ch[1].translate_st = { 0.0f,-1.0f, 0.0f };
		m_motion_ch[3].translate_st = { 0.0f,1.0f, 0.0f };
		m_motion_ch[1].translate.y += 0.1f;
		m_motion_ch[3].translate.y += 0.1f;
	}
	glutPostRedisplay();
	if (b_animation && m_motion_ch[1].translate.y < 2.0f)
		glutTimerFunc(10, Timer_side_up, 0);
}
//----------Timer_back_scale------------------------------------------------------------------------------------
void Timer_back_scale(int value) {
	if (all_animation == 5) {
		m_motion_ch[0].translate_st = { 0.0f,0.0f, 1.0f };
		m_motion_ch[0].scale.x -= 0.01f;
		m_motion_ch[0].scale.y -= 0.01f;
		m_motion_ch[0].scale.z -= 0.01f;

	}
	glutPostRedisplay();
	if (b_animation && m_motion_ch[0].scale.x > 0.0f)
		glutTimerFunc(10, Timer_back_scale, 0);
}
//----------Timer_turn_y------------------------------------------------------------------------------------
void Timer_turn_y(int value) {
	if (all_animation == 6) {
		
		rotate.y += 0.1f;
		if (rotate.y >= 360.0f) {
			rotate.y -= 360.0f;
		}
	}
	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, Timer_turn_y, 0);
}

//--------keyboard----------------------------------------

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

	case'P':case'p'://p : 초기 위치로 리셋(자전 애니메이션도 멈추기
		rotate.x = 0.0f;
		rotate.y = 0.0f;
		degree = 0.0f;
		rotateobj.x = 0.0f;
		rotateobj.y = 0.0f;
		rotateobjglu.x = 0.0f;
		rotateobjglu.y = 0.0f;
		all_animation = 0;
		translate_origin_obj = { 0.0f,0.0f,0.0f };
		break;

	case'R':case'r': //"키보드 r: xz 평면에 스파이럴을 그리고 , 그 스파이럴 위치에 따라 객체 이동 애니메이션",

		break;
	case'T':case't': //"t: 육면체의 윗면 애니메이션 시작 정지윗면의 가운데 축을 중심으로 회전한다",
		all_animation = 4;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_turn_top, 0);
		break;

	case'F':case'f'://  f: 육면체의 앞면을 연다 앞면을 닫는다
		all_animation = 2;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_turn_face, 0);
		break;
	case'S':case's':// s : 육면체의 옆면을 연다 닫는다
		all_animation = 1;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_side_up, 0);
		break;
	case'B':case'b':// "b: 육면체의 뒷면을 연다 닫는다",
		all_animation = 5;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_back_scale, 0);
		break;

	case'Y': case'y':// "b: 육면체의 뒷면을 연다 닫는다",
		all_animation = 6;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_turn_y, 0);
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
;
		break;

	case GLUT_KEY_RIGHT:// →
	
		break;

	case GLUT_KEY_UP:// ↑

		break;

	case GLUT_KEY_DOWN:// ↓

		break;
	}

	glutPostRedisplay();
}



// ---- 그리기 콜백 함수---------------------------------------------------------------------------------------------------
GLvoid drawScene()
{
	//glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- 변경된 배경색 설정 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			//--- 변경된 배경색 설정 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- 렌더링 파이프라인에 세이더 불러오기
	//xyz축그리기
	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(-30.0f), x_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);
		//transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);
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

	//정육면체
	
	Mesh& m = mcube;
	glBindVertexArray(m.vao);								//--- 사용할 VAO 불러오기
		
	if (t_or_l) {//면으로 출력
		for (int j = 0; j < m.indexnum / 2; j++) { // indexnum = 12;
			meshface_srt(j);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
			}
	}
	else { //선으로 출력
		for (int j = 0; j < m.indexnum * 3; j++) {
			meshface_srt(j);
			glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(j * 3 * sizeof(unsigned int)));	//큐브 1면 출력
		}
	}

	glutSwapBuffers();									//--- 화면에 출력하기
}

void meshface_srt(int& i) {
	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::rotate(transformMatrix, glm::radians(-30.0f), x_axis);
	transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);
	transformMatrix = glm::scale(transformMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
	transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);
	transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.x), x_axis);

	transformMatrix = glm::translate(transformMatrix, glm::vec3(m_motion_ch[i].translate));
	transformMatrix = glm::translate(transformMatrix, glm::vec3(m_motion_ch[i].translate_st));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(m_motion_ch[i].scale));

	transformMatrix = glm::rotate(transformMatrix, glm::radians(m_motion_ch[i].radx), x_axis);
	transformMatrix = glm::rotate(transformMatrix, glm::radians(m_motion_ch[i].rady), y_axis);
	transformMatrix = glm::rotate(transformMatrix, glm::radians(m_motion_ch[i].radz), z_axis);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(m_motion_ch[i].translate_st) * glm::vec3{-1.0f});

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
}


void reset() {
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void ReadObj(Mesh& mesh, FILE* path) {
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
	std::cout << "vertexnum" << vertexnum << '\n';
	std::cout << "facenum" << facenum << '\n';
	int vertIndex = 0;
	int faceIndex = 0;
	//int uvIndex = 0;
	//--- 2. 메모리 할당
	glm::vec3* vertex = new glm::vec3[vertexnum];
	glm::vec3* color = new glm::vec3[vertexnum];
	glm::uvec3* face = new glm::uvec3[facenum];
	//glm::vec3* uvdata = new glm::vec3[facenum];
	//glm::vec3* uv = new glm::vec3[uvnum];
	char bind[128];
	//--- 3. 할당된 메모리에 각 버텍스, 페이스, uv 정보 입력
	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(path, "%f %f %f\n",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);
			color[vertIndex].x = vertIndex % 2 == 0 ? 0.0f : 1.0f;
			color[vertIndex].y = (vertIndex/2) % 2 == 0 ? 0.0f : 1.0f;
			color[vertIndex].z = (vertIndex/4) % 2 == 0 ? 0.0f : 1.0f;
			std::cout << "vertex[" << vertIndex << "] : {" << vertex[vertIndex].x << ", " << vertex[vertIndex].y << ", " << vertex[vertIndex].z << "}" << '\n';
			std::cout << "color[" << vertIndex << "] : {" << color[vertIndex].x << ", " << color[vertIndex].y << ", " << color[vertIndex].z << "}" << '\n';
			vertIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			unsigned int temp_face[3], temp_uv[3], temp_normal[3];
			//fscanf(path, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
			fscanf(path, "%d %d %d\n",
	/*			&temp_face[0], &temp_uv[0], &temp_normal[0],
				&temp_face[1], &temp_uv[1], &temp_normal[1],
				&temp_face[2], &temp_uv[2], &temp_normal[2]);*/
				&temp_face[0], &temp_face[1], &temp_face[2]);
			face[faceIndex].x = temp_face[0] - 1;
			face[faceIndex].y = temp_face[1] - 1;
			face[faceIndex].z = temp_face[2] - 1;
			//uvdata[faceIndex].x = temp_uv[0];
			//uvdata[faceIndex].y = temp_uv[1];
			//uvdata[faceIndex].z = temp_uv[2];
			//uv[faceIndex].x = temp_normal[0];
			//uv[faceIndex].y = temp_normal[1];
			//uv[faceIndex].z = temp_normal[2];
			std::cout << "face[" << faceIndex << "] : {" << face[faceIndex].x << ", " << face[faceIndex].y << ", " << face[faceIndex].z << "}" << '\n';
			faceIndex++;
		}
		//else if (bind[0] == 'v' && bind[1] == 't' && bind[2] == '\0') {
		//	fscanf(path, "%f %f\n", &uv[uvIndex].x, &uv[uvIndex].y);
		//	uvIndex++;
		//}
	}
	//--- 필요한 경우 읽어온 값을 전역 변수 등에 저장
	//gpu 버퍼에 저장하기
	{
		glGenVertexArrays(1, &mesh.vao); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(mesh.vao);		//--- VAO를 바인드하기
		glGenBuffers(2, mesh.vbo);		//--- 2개의 VBO를 지정하고 할당하기

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
		glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(glm::vec3), color, GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &mesh.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, facenum * sizeof(glm::uvec3), face, GL_STATIC_DRAW);

	}
	mesh.indexnum = facenum ; // 삼각형 면의 개수
	
	//----------------------------------------------
	delete[] vertex;
	delete[] face;
	delete[] color;
}

void getobjfile(Mesh& mesh, const std::string& objname){
	FILE* fp = fopen(objname.c_str(), "rt");
	if (!fp) {
		std::cout << objname << "파일 오픈 실패." << '\n';
		return;
	}
	else {
		std::cout << objname << "파일 오픈" << '\n';
	}
	ReadObj(mesh, fp);
	fclose(fp);
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

	getobjfile(mcube, "obj\\cube.obj");
	getobjfile(mpyramid, "obj\\pyramid.obj");


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