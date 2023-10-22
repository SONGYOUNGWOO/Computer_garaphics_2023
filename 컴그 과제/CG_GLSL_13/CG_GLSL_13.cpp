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
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
class shape {
private:
	std::string name;
public:
	glm::vec3 p[4]; //사각형의 각 점의 좌표
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;	
	std::vector<float> color;
	GLint motions;
	float mouse_check_size;
	int* points;
	//생성자
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

	//소멸자
	~shape() {
		delete points;
	}
	//멤버 함수(method)
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
		std::cout << "정점 위치들" << '\n';
		for (int i = 0; i < 4; ++i) {
			std::cout << "p[" << i << "] = ("<< this->p[i].x<<", " << this->p[i].y << ", " << this->p[i].z << ")" << '\n';
		}
		std::cout << "색상" << '\n';
		for (int i = 0; i < 4; i++) {
			std::cout << "color[" << i << "] = (" << this->color.at(i* 3) << ", " << this->color.at(i * 3 + 1) << ", " << this->color.at(i * 3 + 2) << ")" << '\n';
		}
		std::cout << "------------------------------------------------------------------" << '\n';
	}
	//glm::vec3 mat_size;
	//GLfloat allvertex[4][3];	// 위치
	//GLfloat allcolor[4][3];		// 색상
	//GLint choice;				// 어떤 도형 0점 1선 2삼각형 3사각형
	//int dx, dy, touchnum;		// 도형 방향 , 벽에 닿은 횟수				// 0 이랑 180이 위아래
};

//--- 전역변수 -------------------------------------------------------------------------------------------------------
shape axis;
shape rect;
int choice(0);		//그릴 도형 모양
bool start = false; // 타이머 실행 여부
int n = 0;			// 출력할 도형 개수
GLuint vao, vbo[2];
int target{ 0 };  //선택한 도형
bool Point_Inside_Rectangle = false; // 사각형 내부
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
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램
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
//-----------------------------------------------------------------------------------------------------------------------------------

//----------------------------void InitBuffer(shape& s)------------------------------------------------------------------------------
void InitBuffer(shape& s) {
	//버퍼 생성
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

		//디버깅
/*		{
			std::cout << std::format("현재 도형 [{}]: vertex - ({},{},{}), time : {}\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2), s.time);
		}	*/	
	}

	//if(s.pointnum + 1 > 0)
	//	index.push_back(0);		//점이 1개일떄
	//if (s.pointnum + 1 > 1)
	//	index.push_back(1);		//점이 2개일떄
	//if (s.pointnum + 1 > 2)
	//	index.push_back(2);		//점이 3개일떄
	//if (s.pointnum + 1 > 3){
	//	index.push_back(0);
	//	index.push_back(2);
	//	index.push_back(3);		//점이 4개일떄
	//}
	//if (s.pointnum + 1 > 4) {
	//	index.push_back(0);
	//	index.push_back(3);
	//	index.push_back(4);		//점이 5개일떄
	//}
	//if(s.pointnum + 1 > 5) {
	//	index.push_back(0);
	//	index.push_back(4);
	//	index.push_back(5);		//점이 8개일떄
	//}
	//s.indexnum = index.size();

	//gpu 버퍼에 저장하기
	{
		glGenVertexArrays(1, &s.vao); //--- VAO 를 지정하고 gpu할당하기
		glBindVertexArray(s.vao);		//--- VAO를 바인드하기
		glGenBuffers(2, s.vbo);		//--- 2개의 VBO를 지정하고 gpu할당하기

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
	
	//이미 gpu에 저장된 내용을 cpu에서 제거	
	vertex.clear();
	color.clear();
}

//----------------사각형 내부 판정 -----------------------------------------------------------------------------------------------
bool IsPointInsideRectangle(const glm::vec2& point, const shape& rect) {
	int x_count = 0; // 교차점 개수를 0으로 초기화
	int y_count = 0; // 교차점 개수를 0으로 초기화

	// 직사각형 꼭짓점을 선으로변환
	glm::vec2 rectSegments[4][2];
	for (int i = 0; i < 4; i++) {
		rectSegments[i][0] = glm::vec2(rect.p[i].x, rect.p[i].y);
		rectSegments[i][1] = glm::vec2(rect.p[(i + 1) % 4].x, rect.p[(i + 1) % 4].y); //다음 꼭짓점(3->0)
	}

	//위아래 2개 벡터 - 같은x , y 비교
	for (int i = 0; i < 4; i += 2) {
		glm::vec2 p1 = rectSegments[i][0];
		glm::vec2 p2 = rectSegments[i][1];

		if (p1.x != p2.x) { // 수평 무시			
			float intersectionY = (point.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x) + p1.y;	//dy * m(벡터의 기울기) + x = Px
			if ((p1.y == p2.y and point.y < p1.y) || point.y <= intersectionY) {
				std::cout << i + 1 << "번쨰 count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}
		}
		else { //기울기가 0일 때
			if (point.y < p1.y) {
				std::cout << i + 1 << "번쨰 count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}

		}		
	}
	//양옆벡터 - 같은y, x 비교
	for (int i = 1; i < 4; i += 2) {
		glm::vec2 p1 = rectSegments[i][0];
		glm::vec2 p2 = rectSegments[i][1];

		if (p1.y != p2.y) { // 수평 무시			
			float intersectionX = (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;	//dy * m(벡터의 기울기) + x = Px			
			if ((p1.x == p2.x and point.x < p1.x) || point.x <= intersectionX) {
				std::cout << i + 1 << "번쨰 count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}

		}
		else { //기울기가 0일 때
			if (point.x < p1.x) {
				std::cout << i + 1 << "번쨰 count ++" << '\n';
				i % 2 == 0 ? x_count++ : y_count++;
			}

		}
	}
	
	std::cout << "x_count = " << x_count << '\n';
	std::cout << "y_count = " << y_count << '\n';
	// 홀짞 규칙을 사용하여 점이 직사각형 안에 있는지 확인, 1이면 내부 0이면 외부
	if ((y_count % 2) == 1 and (x_count % 2) == 1) {
		std::cout << "사각형 내부" << '\n';
		return true;
	}
	else {
		std::cout << "사각형 외부" << '\n';
		return false;
	}
}

//----------------Mouse--------------------------------------------------------------------------------------------------------------
void Mouse(int button, int state, int x, int y) {

	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (600 / 2.0f - y) / (600 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		for (int i = 0; i < 4; ++i) {//충돌판정
			if (rect.p[i].x - rect.mouse_check_size < mx and  mx < rect.p[i].x + rect.mouse_check_size and	 
				rect.p[i].y - rect.mouse_check_size < my and my < rect.p[i].y + rect.mouse_check_size) {
				target = i;
				left_button = true;
				//std::cout << "rect[:" << target << "].x =" << rect.p[target].x << '\n';
			}
		}
		//사각형 내부
		if (IsPointInsideRectangle(glm::vec2(mx, my), rect)) {
			Point_Inside_Rectangle = true;
		}
	
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		target = -1; // target 초기화
		left_button = false;
		Point_Inside_Rectangle = false;
	}


	mouse[0] = mx;
	mouse[1] = my;
	glutPostRedisplay();// 화면 다시 그리기 요청
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
	glutPostRedisplay(); // 화면 다시 그리기 요청
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
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

// --------- GLvoid drawScene()------------------------------------------------------------------------------------------------------
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);//--- 변경된 배경색 설정 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);//--- 렌더링 파이프라인에 세이더 불러오기
	{
		glm::mat4 transformMatrix(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));//--- modelTransform 변수에 변환 값 적용하기
	}
	// x, y 축 axis 그리기
	glBindVertexArray(axis.vao);//--- 사용할 VAO 불러오기
	//std::cout << axis.Getname() << '\n';
	//axis.Print_member();
	glLineWidth(3);
	glDrawArrays(GL_LINES, 0, 4);

	//사각형 출력
	glBindVertexArray(rect.vao);//--- 사용할 VAO 불러오기
	//std::cout << rect.Getname() << '\n';
	//rect.Print_member();
	//점 출력
	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, 4);

	//라인으로 묶기
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
	//	glBindVertexArray(rect[i].vao);								//--- 사용할 VAO 불러오기
	//	//{
	//	//	glm::mat4 transformMatrix(1.0f);
	//	//	transformMatrix = glm::translate(transformMatrix, glm::vec3(rect[i].x, rect[i].y, 0.0f));
	//	//	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	//	//	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
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
	//	//std::cout << "출력 후 rect[" << i << "]: (" << rect[i].x << ", " << rect[i].y << ")" << std::endl;
	//	//{
	//	//	GLint bufferSize = 0;
	//	//	glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[0]);
	//	//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	//	//	// 현재 바인딩된 VBO의 크기를 가져옴
	//	//	GLfloat* data = new GLfloat[bufferSize / sizeof(GLfloat)];
	//	//	glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, data);
	//	//	// VBO의 데이터를 읽어옴
	//	//	for (int i = 0; i < bufferSize / sizeof(GLfloat); i++) {
	//	//		std::cout << "Data[" << i << "]: " << data[i] << std::endl;
	//	//	}
	//	//	delete[] data;
	//	//	// 데이터 출력 후 메모리를 해제
	//	//}
	//
	//}

	glutSwapBuffers();//--- 화면에 출력하기
}

// ---------void reset()-------------------------------------------------------------------------------------------------------------
void reset() {	
	shape& s = rect;
	std::cout << "InitBuffer 직전" << '\n';
	s.Setname("Rectangle");
	InitBuffer(s);
	std::cout << "InitBuffer 직후" << '\n';


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

//--- 다시그리기 콜백 함수-----------------------------------------------------------------------------------------------------------
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//---------------------mian함수--------------------------------------------------------------------------
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example9");
	//--- GLEW 초기화하기e
	glewExperimental = GL_TRUE;
	glewInit();
	std::cout << " GLEW 초기화 완료" << '\n';

	make_shaderProgram();

	reset();
	std::cout << " reset 초기화 완료" << '\n';


	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력
	glutMouseFunc(Mouse); // 마우스 입력
	glutMotionFunc(Motion); // 마우스 입력

	glutMainLoop();
}