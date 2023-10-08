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
	GLfloat x, y, size;		// 중심 좌표, 마우스 클릭
	GLfloat allvertex[4][3];	// 위치
	GLfloat allcolor[4][3];		// 색상
	GLint choice;				// 어떤 도형 0점 1선 2삼각형 3사각형
}shape;
shape rect[4];					//최대 10개
int n = 0;
bool a = false;
bool b = false;

void InitBuffer(shape* s) {
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
void setline(int i) {
	rect[i].allvertex[0][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].x + rect[i].size;
	rect[i].allvertex[1][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = rect[i].x;
	rect[i].allvertex[2][1] = rect[i].y + rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[3][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;


}
void settriangle(int i) {
	rect[i].allvertex[0][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[0][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[0][2] = 0.0f;

	rect[i].allvertex[1][0] = rect[i].x + rect[i].size;
	rect[i].allvertex[1][1] = rect[i].y - rect[i].size/ 2;
	rect[i].allvertex[1][2] = 0.0f;

	rect[i].allvertex[2][0] = rect[i].x;
	rect[i].allvertex[2][1] = rect[i].y + rect[i].size;
	rect[i].allvertex[2][2] = 0.0f;

	rect[i].allvertex[3][0] = rect[i].x - rect[i].size;
	rect[i].allvertex[3][1] = rect[i].y - rect[i].size / 2;
	rect[i].allvertex[3][2] = 0.0f;
}

void quadrant(float mx, float my) {
	int aa = 0;
	if (mx > 0)  aa += 1;
	if (my < 0)  aa += 2;

	rect[aa].x = mx;
	rect[aa].y = my;
	rect[aa].size = randomnum(0.1, 0.3);
	settriangle(aa);

}
void changet(float mx, float my) {
	int ct = 0;
	if (mx > 0)  ct += 1;
	if (my < 0)  ct += 2;

	rect[ct].x = mx;
	rect[ct].y = my;
	rect[ct].size = randomnum(0.1, 0.3);

	

	if (rect[ct].choice == 0) {

		settriangle(ct);
	}
	else {
	
		setline(ct);
	}


}

void Mouse(int button, int state, int x, int y){
	float mx =  (x - 800 / 2.0f) / (800 / 2.0f);
	float my =  (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		changet(mx, my); // 추가

	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':case 'A':
		a = true;
		b = false;
		if (a) {
			for (int i = 0; i < 4; ++i) {
				rect[i].choice = 0;
			}
			
		}
		break;
	case 'b':case 'B':
		a = false;
		b = true;
		if (b) {
			for (int i = 0; i < 4; ++i) {
				rect[i].choice = 1;
			}
		}
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
		InitBuffer(&rect[i]);
		glBindVertexArray(vao);								//--- 사용할 VAO 불러오기

		std::cout << rect[i].choice;

		if (rect[i].choice == 0) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else  {
			glLineWidth(2);
			glDrawArrays(GL_LINE_STRIP, 0, 4);
			
		}
	}

	glutSwapBuffers();									//--- 화면에 출력하기
}
void reset() {

	for (int i = 0; i < 4; ++i) {

		float x = i % 2 == 0 ? -0.5 : 0.5;
		float y = i / 2 == 0 ? -0.5 : 0.5;
		quadrant(x, y);
		GLfloat randomColor[3] = { randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) };

		for (int a = 0; a < 4; ++a) {
			
			for (int b = 0; b < 3; ++b) {
				rect[i].allcolor[a][b] = randomColor[b];
			}

		}

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
	glutCreateWindow("Example8");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	make_shaderProgram();

	InitBuffer();
	reset();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력
	glutMouseFunc(Mouse); // 마우스 입력
	glutMainLoop();
}