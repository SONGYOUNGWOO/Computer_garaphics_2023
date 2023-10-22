#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <random>
//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
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

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램
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

const GLfloat triShape[3][3] = {							//--- 삼각형 위치 값
	{ -0.5, -0.5, 0.0 },
	{ 0.5, -0.5, 0.0 },
	{ 0.0, 0.5, 0.0}
};
const GLfloat colors[3][3] = {								//--- 삼각형 꼭지점 색상
	{ 1.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{0.0, 0.0, 1.0 }
};
GLuint vao, vbo[2];
void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao);		//--- VAO를 바인드하기
	glGenBuffers(2, vbo);		//--- 2개의 VBO를 지정하고 할당하기

	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glEnableVertexAttribArray(1);													//--- attribute 인덱스 1번을 사용 가능하게 함.

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

	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
	//return shaderProgramID;

}

typedef struct allshape {
	GLfloat x, y , sizel;		// 중심 좌표, 마우스 클릭
	GLfloat allvertex[4][3];	// 위치
	GLfloat allcolor[4][3];		// 색상
	GLint choice;				// 어떤 도형 0점 1선 2삼각형 3사각형
}shape;
shape rect[10];					//최대 10개
int n = 0;
//-------------------------------------------------------------------------------------------------------------
//------------InitBuffer---------------------------------------------------------------------------------------
void InitBuffer(shape *s) {
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao);		//--- VAO를 바인드하기
	glGenBuffers(2, vbo);		//--- 2개의 VBO를 지정하고 할당하기

	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allvertex, GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), s->allcolor, GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
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

	if (rect[n].choice == 0) {		// 점
		setpoint(n);
	}
	else if (rect[n].choice == 1) {	// 선
		setline(n);
	}
	else if (rect[n].choice == 2) {	//삼각형
		settriangle(n);
	}
	else if (rect[n].choice == 3) {	//사각형
		setsquare(n);
	}
	n++;
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		addshape(x, y); //사각형 추가
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

bool w = false;
bool a = false;
bool s = false;
bool d = false;

void TimerW(int value) {
	for (int i = 0; i < n; ++i) {
		rect[i].y += 0.01f;
		if (rect[i].choice == 0) {		// 점
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// 선
			setline(i);
		}
		else if (rect[i].choice == 2) {	//삼각형
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//사각형
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
		if (rect[i].choice == 0) {		// 점
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// 선
			setline(i);
		}
		else if (rect[i].choice == 2) {	//삼각형
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//사각형
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
		if (rect[i].choice == 0) {		// 점
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// 선
			setline(i);
		}
		else if (rect[i].choice == 2) {	//삼각형
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//사각형
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
		if (rect[i].choice == 0) {		// 점
			setpoint(i);
		}
		else if (rect[i].choice == 1) {	// 선
			setline(i);
		}
		else if (rect[i].choice == 2) {	//삼각형
			settriangle(i);
		}
		else if (rect[i].choice == 3) {	//사각형
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
	case 'p':case 'P':	//p: 점 그리기
		rect[i].choice = 0;
		break;
	case 'i':case 'I':	//l: 선 그리기
		rect[i].choice = 1;
		break;
	case 't':case 'T':	//t: 삼각형 그리기
		rect[i].choice = 2;
		break;
	case 'r':case 'R':	//r: 사각형 그리기
		
		rect[i].choice = 3;
		break;
	case 'w':case 'W':	//w: 위
		w = true;
		a = false;
		s = false;
		d = false;
		if (w == true)
			glutTimerFunc(60, TimerW, 1);
		break;
	case 'a':case 'A':	//a: 왼쪽
		w = false;
		a = true;
		s = false;
		d = false;
		if (a == true)
			glutTimerFunc(60, TimerA, 1);
		break;
	case 's':case 'S':	//s: 아래
		w = false;
		a = false;
		s = true;
		d = false;
		if (s == true)
			glutTimerFunc(60, TimerS, 1);
		break;
	case 'd':case 'D':	//d: 오른쪾
		w = false;
		a = false;
		s = false;
		d = true;
		if (d == true)
			glutTimerFunc(60, TimerD, 1);
		break;
	case 'c':case 'C':	//c: 모든 도형을 삭제한다.
		n = 0;
		break;
	case 'q':case 'Q':	//q: 종료
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

	for (int i = 0; i < n; ++i) {
		// std::cout << rect[i].x;
		InitBuffer(&rect[i]);
		glBindVertexArray(vao);								//--- 사용할 VAO 불러오기

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

	glutSwapBuffers();									//--- 화면에 출력하기
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
	glutCreateWindow("Example7");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력
	glutMouseFunc(Mouse); // 마우스 입력
	glutMainLoop();
}