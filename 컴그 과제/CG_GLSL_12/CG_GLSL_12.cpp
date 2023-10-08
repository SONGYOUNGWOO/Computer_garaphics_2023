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
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
typedef struct allshape {
	GLfloat x, y, size;			// 중심 좌표, 마우스 클릭
	float time;
	int pointnum;
	int indexnum;
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;

	std::vector<float> color;

	//float chy;
	//glm::vec3 mat_size;
	//GLfloat allvertex[4][3];	// 위치
	//GLfloat allcolor[4][3];		// 색상
	//GLint choice;				// 어떤 도형 0점 1선 2삼각형 3사각형
	//int dx, dy, touchnum;		// 도형 방향 , 벽에 닿은 횟수				// 0 이랑 180이 위아래
}shape;

//--- 전역변수 ----
shape rect[4];
int choice(0);		//그릴 도형 모양
bool start = false; // 타이머 실행 여부
int n = 0;			// 출력할 도형 개수


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

void InitBuffer(shape& s) {
	//버퍼 생성
	std::vector<float> s_vertex;
	std::vector<float> l_vertex;
	std::vector<float> vertex;

	std::vector<float> color;


	std::vector<unsigned int> index;



	float s_rad = 360.0 / s.pointnum;
	float l_rad = 360.0 / (s.pointnum + 1);
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
		vertex.push_back((1 - s.time) * s_vertex.at(i * 3) + s.time * l_vertex.at(i * 3));			//x
		vertex.push_back((1 - s.time) * s_vertex.at(i * 3 + 1) + s.time * l_vertex.at(i * 3 + 1));	//y
		vertex.push_back(0.0);	//z
		
		color.push_back(s.color.at(i * 3));	//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b

		//디버깅
		{
			std::cout << std::format("현재 도형 [{}]: vertex - ({},{},{}), time : {}\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2), s.time);
		}		
	}

	if(s.pointnum + 1 > 0)
		index.push_back(0);		//점이 1개일떄
	if (s.pointnum + 1 > 1)
		index.push_back(1);		//점이 2개일떄
	if (s.pointnum + 1 > 2)
		index.push_back(2);		//점이 3개일떄
	if (s.pointnum + 1 > 3){
		index.push_back(0);
		index.push_back(2);
		index.push_back(3);		//점이 4개일떄
	}
	if (s.pointnum + 1 > 4) {
		index.push_back(0);
		index.push_back(3);
		index.push_back(4);		//점이 5개일떄
	}
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
		std::cout << "마우스 클릭 타이머 시작된 도형 번호[:" << target  <<"]" << '\n';
		glutTimerFunc(10, TimerA, target);
		
		
		//changet(mx, my); // 추가
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}



//타이머 함수
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
	//std::cout << "타이머 호출: rect[" << value << "].pointnum =" << rect[value].pointnum << '\n';
	InitBuffer(rect[value]);

	glutPostRedisplay();	
	if(rect[value].pointnum < 6)
		glutTimerFunc(10, TimerA, value);
}



GLvoid Keyboard(unsigned char key, int x, int y) {

	
	switch (key) {
	//선
	case 'l': case 'ㅣ':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 2;
			InitBuffer(rect[i]);
		}
		break;
	//삼각형
	case 't':case 'T':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 3;
			InitBuffer(rect[i]);
		}
		break;
	// 사각형
	case 'r':case 'R':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 4;
			InitBuffer(rect[i]);
		}
		break;
	//오각형
	case 'p':case 'P':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 5;
			InitBuffer(rect[i]);
		}
		break;
	// 다시
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
	
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

// -- - 그리기 콜백 함수
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);			//--- 변경된 배경색 설정 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- 렌더링 파이프라인에 세이더 불러오기

	for (int i = 0; i < n; ++i) {

		glBindVertexArray(rect[i].vao);								//--- 사용할 VAO 불러오기
		{
			glm::mat4 transformMatrix(1.0f);
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

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

	glutSwapBuffers();									//--- 화면에 출력하기
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