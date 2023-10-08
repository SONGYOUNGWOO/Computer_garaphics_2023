#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
typedef struct allshape {
	GLfloat x, y, size;			// 중심 좌표, 마우스 클릭
	GLfloat allvertex[4][3];	// 위치
	GLfloat allcolor[4][3];		// 색상
	GLint choice;				// 어떤 도형 0점 1선 2삼각형 3사각형
	int dx, dy, touchnum;		// 도형 방향 , 벽에 닿은 횟수
	float rad;					// 0 이랑 180이 위아래
	GLuint vao;
	GLuint vbo[2];
	float chy;
	//glm::vec3 mat_size;
}shape;
shape rect[4];

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
void bounce();
void zizzag();
void quadric_spiral();
void circle_spiral();
GLvoid Reshape(int w, int h);
//void InitBuffer();
void InitBuffer(shape* s);

float randomnum(float a, float b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(a, b);
	return dis(gen);
}
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

	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
	//return shaderProgramID;

}
int n = 0;
bool a = false;
bool b = false;

void InitBuffer(shape* s) {
	glGenVertexArrays(1, &s->vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(s->vao);		//--- VAO를 바인드하기
	glGenBuffers(2, s->vbo);		//--- 2개의 VBO를 지정하고 할당하기

	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, s->vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allvertex, GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, s->vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allcolor, GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glEnableVertexAttribArray(1);
}
void setline(int i) {
	rect[i].allvertex[0][0] =  - rect[i].size;
	rect[i].allvertex[0][1] =  - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] =  rect[i].size;
	rect[i].allvertex[1][1] =  - rect[i].size / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = 0.0f;
	rect[i].allvertex[2][1] = rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = - rect[i].size;
	rect[i].allvertex[3][1] = - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;

}
void settriangle(int i) {
	rect[i].allvertex[0][0] =  - rect[i].size / 2;
	rect[i].allvertex[0][1] =  - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].size / 2;
	rect[i].allvertex[1][1] =  - rect[i].size / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = 0.0f;
	rect[i].allvertex[2][1] =  rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] =  - rect[i].size;
	rect[i].allvertex[3][1] = - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;

}

void changet(float mx, float my) {
	int ct = 0;
	if (mx > 0)  ct += 1;
	if (my < 0)  ct += 2;

	rect[ct].x = mx;
	rect[ct].y = my;

	//std::cout << "rect["<< ct << "]: (" << rect[ct].x << ", " << rect[ct].y << ")"<< std::endl;

	rect[ct].size = randomnum(0.1, 0.2);
	rect[ct].dx = rect[ct].dx = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;
	rect[ct].dy = rect[ct].dy = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;

	GLfloat randomColor[3] = { randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) };
	for (int a = 0; a < 4; ++a) {

		for (int b = 0; b < 3; ++b) {
			rect[ct].allcolor[a][b] = randomColor[b];
		}

	}

	if (rect[ct].choice == 0) {
		settriangle(ct);
	}
	else {
		setline(ct);
	}
	InitBuffer(&rect[ct]);
}

void Mouse(int button, int state, int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		changet(mx, my); // 추가
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}
bool start = false;
bool boolai = false;
int caset{0};

void bounce()
{
	for (int i = 0; i < 4; ++i) {
		rect[i].x += rect[i].dx * 0.01f;
		rect[i].y += rect[i].dy * 0.01f;
		std::cout << "timer[" << i << "]: (" << rect[i].dx << ", " << rect[i].x << ")" << std::endl;

		if ((rect[i].dx < 0 and rect[i].x - rect[i].size <= -1) || (rect[i].dx > 0 and rect[i].x + rect[i].size >= 1)) {
			rect[i].dx *= -1;
			rect[i].x += rect[i].dx * 0.01f;
		}
		if ((rect[i].dy < 0 and rect[i].y - rect[i].size <= -1) || (rect[i].dy > 0 and rect[i].y + rect[i].size >= 1)) {
			rect[i].dy *= -1;
			rect[i].y += rect[i].dy * 0.01f;
		}
		std::cout << rect[i].x << '\n';
		//settriangle(i);
	}
}
void zizzag()
{
	for (int i = 0; i < 4; ++i) {

		if (((int)rect[i].rad / 90) % 2 == 0) { // 0 이랑 180이 위아래

			rect[i].y += rect[i].dy * 0.01f;
			rect[i].chy += 0.01f;

			if (rect[i].chy > rect[i].size * 2) {

				rect[i].rad = 180.0f + rect[i].dx * 90.0f;
			}
			else if ((rect[i].dy < 0 and rect[i].y - rect[i].size <= -1) || (rect[i].dy > 0 and rect[i].y + rect[i].size >= 1)) {
				rect[i].rad = 180.0f + rect[i].dx * 90.0f;
				rect[i].dy *= -1;
			}
		}
		else {

			rect[i].x += rect[i].dx * 0.01f;

			if ((rect[i].dx < 0 and rect[i].x - rect[i].size <= -1) || (rect[i].dx > 0 and rect[i].x + rect[i].size >= 1)) {
				rect[i].dx *= -1;
				rect[i].rad = 90.0f - rect[i].dy * 90.0f; // 아래를 바라보도록 각도 설정
				rect[i].chy = 0.0f;
			}

		}
		std::cout << i << "번쨰 좌표 : " << rect[i].x << ", " << rect[i].y << '\n';


	}
}
void quadric_spiral()
{
	for (int i = 0; i < 4; ++i) {

		if (((int)rect[i].rad / 90) % 2 == 0) { // 0 이랑 180이 위아래

			rect[i].y += rect[i].dy * 0.01f;

			// y가 벽에 닿으면 90, 270 좌, 우로 방향 변경
			if ((rect[i].dy < 0 and rect[i].y - rect[i].size <= -1) || (rect[i].dy > 0 and rect[i].y + rect[i].size >= 1)) {
				rect[i].touchnum += 1;
				rect[i].rad = 180.0f + rect[i].dx * 90.0f; //좌우
				rect[i].dy *= -1;
			}

			else if (rect[i].touchnum > 3) {
				if ((rect[i].dy < 0 and rect[i].y < (-1 + rect[i].size * rect[i].touchnum / 2)) || (rect[i].dy > 0 and rect[i].y >(1 - rect[i].size * rect[i].touchnum / 2))) {
					rect[i].touchnum += 1;
					rect[i].rad = 180.0f + rect[i].dx * 90.0f; //좌우
					rect[i].dy *= -1;
				}
				if ((-1 + rect[i].size * rect[i].touchnum / 2) > 0.99f) {
					rect[i].touchnum = 0;
				}
			}
		}
		else { // 좌 우 면

			rect[i].x += rect[i].dx * 0.01f;

			// x가 벽에 닿으면 0, 180 상, 하로  방향 변경
			if ((rect[i].dx < 0 and rect[i].x - rect[i].size <= -1) || (rect[i].dx > 0 and rect[i].x + rect[i].size >= 1)) {
				rect[i].touchnum += 1;						// 벽에 닿음
				rect[i].rad = 90.0f - rect[i].dy * 90.0f;	// 상 하
				rect[i].dx *= -1;
			}

			else if (rect[i].touchnum > 4) {
				if ((rect[i].dx < 0 and rect[i].x < (-1 + rect[i].size * rect[i].touchnum / 2)) || (rect[i].dx > 0 and rect[i].x >(1 - rect[i].size * rect[i].touchnum / 2))) {
					rect[i].touchnum += 1;
					rect[i].rad = 90.0f - rect[i].dy * 90.0f; //상 하
					rect[i].dx *= -1;
				}
				if ((-1 + rect[i].size * rect[i].touchnum / 2) > 0.99f) {
					rect[i].touchnum = 0;
				}
			}

		}
	}
}
void circle_spiral()
{
	for (int i = 0; i < 4; ++i) {

		rect[i].x = cos(glm::radians(rect[i].rad)) * rect[i].chy;
		rect[i].y = sin(glm::radians(rect[i].rad)) * rect[i].chy;

		rect[i].chy += 0.01f * (i + 1);
		rect[i].rad += 7.0f * (i + 1);
		if (rect[i].chy > 1) {
			rect[i].chy = 0;
		}
		if (rect[i].rad > 360) {
			rect[i].rad -= 360;
		}

	}
}
void TimerA(int value) {

	if (start) {
		if (caset == 1) {
			bounce();
		}
		else if (caset == 2) {
			zizzag();
		}
		else if (caset == 3) {
			quadric_spiral();
		}
		else if (caset == 4) {
			circle_spiral();
		}
	}
	glutPostRedisplay();
	glutTimerFunc(30, TimerA, 1);
}



GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
		//1. 튕기기
	case 'a':case 'A':
		//caset = caset == 2 ? 0 : 2;
		//start = caset == 2 ? true : false;
		if (caset != 1) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			caset = 1;
		}
		else {
			caset = 0;
			start = false;
		}
		break;

		// 2.좌우로 지그재그
	case 'b':case 'B':
		//caset = caset == 2 ? 0 : 2;
		//start = caset == 2 ? true : false;
		if (caset != 2) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			for (int i = 0; i < 4; ++i) {
				rect[i].rad = 180.0f + rect[i].dx * 90.0f; //90 or 270
			}
			caset = 2;
		}
		else {
			caset = 0;
			start = false;
		}
		break;

		// 3. 사각 스파이럴
	case 'c':case 'C':
		//caset = caset == 3 ? 0 : 3;			
		//start = caset == 3 ? true : false;	
		if (caset != 3) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			for (int i = 0; i < 4; ++i) {
				rect[i].y = 1.0f;
				rect[i].rad = 0.0f + rect[i].dx * 180.0f; // 0 or 180
			}
			caset = 3;
		}
		else {
			caset = 0;
			start = false;
		}
	
		break;
		// 원 스파이럴
	case 'd':case 'D':
		//caset = caset == 4 ? 0 : 4;			// 실행중인게 원운동(4번)이 아닐시 원운동으로 바꿈
		//start = caset == 4 ? true : false;	// 만약 원운동을 끄게 하면 start도 꺼짐
		if (caset != 4) {
			if (start == false) {
				glutTimerFunc(10, TimerA, 1);
				start = true;
			}
			for (int i = 0; i < 4; ++i) {
				rect[i].chy = (pow(rect[i].x, 2) + pow(rect[i].y, 2)); // r값
			}
			caset = 4;
		}
		else {
			caset = 0;
			start = false;
		}
		break;

	case 'q':case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

// -- - 그리기 콜백 함수
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- 변경된 배경색 설정 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- 렌더링 파이프라인에 세이더 불러오기
	
	for (int i = 0; i < 4; ++i) {
		/*InitBuffer(&rect[i]);*/
		glBindVertexArray(rect[i].vao);								//--- 사용할 VAO 불러오기
		{
			glm::mat4 transformMatrix(1.0f);
			//std::cout << "출력 전 rect[" << i << "]: (" << rect[i].x << ", " << rect[i].y << ")" << std::endl;
			//transformMatrix = glm::scale(transformMatrix, rect[i].mat_size);
			// 
			//srt 순서
			transformMatrix = glm::translate(transformMatrix, glm::vec3(rect[i].x, rect[i].y, 0.0));
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rect[i].rad), glm::vec3(0.0, 0.0, 1.0));

			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

		}
		if (rect[i].choice == 0) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else {
			glLineWidth(2);
			glDrawArrays(GL_LINE_STRIP, 0, 4);

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
		
	}
	{
		glm::mat4 transformMatrix(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
	}
	glutSwapBuffers();									//--- 화면에 출력하기
}
void reset() {

	for (int i = 0; i < 4; ++i) {

		float x = i % 2 == 0 ? -0.5 : 0.5;
		float y = i / 2 == 0 ? -0.5 : 0.5;
		
		//새로생긴 변수 초기화
		//std::cout << rect[i].x << rect[i].y << std::endl;
		//rect[i].mat_size.x= 0.5f; 
		//rect[i].mat_size.y= 1.0f;
		//rect[i].mat_size.z= 0.0f;

		rect[i].rad = 0.0f;
		rect[i].chy = 0.0f; // 변화량 기록
		rect[i].touchnum = 0;

		changet(x, y);
		std::cout << "rect[" << i << "]: (" << rect[i].dx << ", " << rect[i].dy << ")" << std::endl;

	}
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
	//--- GLEW 초기화하기
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

	glutMainLoop();
}