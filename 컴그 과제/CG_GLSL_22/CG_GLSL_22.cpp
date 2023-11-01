#include "DEFAULT.hpp"

#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것r
//------------------------------------------------------------------------------------------------------
//glew32.lib freeglut.lib 
//------------------------------------------------------------------------------------------------------
const std::string Guide[]{
	"--------------------------------------------------------------------------------------------------",
	"b/B: 크레인의 아래 몸체가 x 축 방향으로 양 음 방향으로 이동한다 . 다시 누르면 멈춘다",
	"m/M: 크레인의 중앙 몸체가 y 축에 대하여 양 음 방향으로 회전한다 . 다시 누르면 멈춘다",
	"f/F: 포신이 y 축에 대하여 양 음 방향으로 회전하는데 , 두 포신이 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
	"e/E: 2 개 포신이 조금씩 이동해서 한 개가 된다 다시 제자리로 이동해서 2 개가 된다",
	"t/T : 크레인의 맨 위 2 개의 팔이 z 축에 대하여 양 음 방향으로 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
	"g/G: 전체 객체들을 y축으로 양/음 방향으로 회전 (중앙의 구의 y축에 대하여 회전)",
	"--------------------------------------------------------------------------------------------------",
	"카메라 변환",
	"z/Z: 카메라가 z 축 양 음 방향으로 이동",
	"x/X: 카메라가 x 축 양 음 방향으로 이동",
	"y/Y: 카메라 기준 y 축에 대하여 회전",
	"r/R: 화면의 중심의 y 축에 대하여 카메라가 회전 중점에 대하여 공전",
	"a/A: r 명령어와 같이 화면의 중심의 축에 대하여 카메라가 회전하는 애니메이션을 진행한다 멈춘다",
	"--------------------------------------------------------------------------------------------------",
	"1: 직각 투영/원근 투영",
	"2: 은면제거 적용/해제",
	"C/c: 초기화",
	"q : 프로그램 종료",
	"--------------------------------------------------------------------------------------------------"
};
//------------------------------------------------------------------------------------------------------
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
class Linexyz {//xyz축 그리기
public:
	glm::vec3 p[6]; // 각 점의 좌표
	GLuint vao;
	GLuint vbo[2];
	std::vector<float> color;

	int* points;
	
	Linexyz() {//생성자
		points = new int;
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		color.clear();

		for (int i = 0; i < 6; ++i) {
			p[i] = { 0.0f, 0.0f, 0.0f };
			p[i][i / 2] = (i % 2 == 0 ? -1.0f : 1.0f);

			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
			color.push_back(randomnum(0.0f, 1.0f));
		}
	}
};
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
class Camera {
public:
	glm::vec3 cameraPos{};
	glm::vec3 cameraDirection{};
	glm::vec3 cameraUp{};
	glm::vec3 n;
	glm::vec3 u;
	glm::vec3 v;
	glm::mat4 view;
	Camera() {
		cameraPos = glm::vec3{ 0.0f, 1.0f, 1.0f };
		cameraDirection = glm::vec3{ 0.0f,0.0f,0.0f };
		cameraUp = glm::vec3{ 0.0f,1.0f,0.0f };
		n = glm::vec3{ 0.0f,0.0f,0.0f };
		u = glm::vec3{ 0.0f,0.0f,0.0f };
		v = glm::vec3{ 0.0f,0.0f,0.0f };
		view = glm::mat4(1.0f);
	}
};
class Floor_Mesh { //바닥
public:
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	GLuint indexnum;
	Floor_Mesh() {
		vao = 0;
		vbo[0] = 0;
		vbo[1] = 0;
		ebo = 0;
		indexnum = 0;
	}
}; 
class Object {
public:
	
	glm::vec3 after_translate;// after 이동
	glm::vec3 after_rotate;//  after 공전
	glm::vec3 after_scale;//  after 공전
	
	glm::vec3 translate;// 위치
	glm::vec3 st_translate;// 위치
	glm::vec3 rotate;// 자전
	glm::vec3 scale; // 크기

	Object() {
		after_translate = { 0.0f,0.0f,0.0f };
		after_rotate = { 0.0f,0.0f,0.0f };
		after_scale = { 1.0f,1.0f,1.0f };
		translate = { 0.0f,0.0f,0.0f };
		st_translate = { 0.0f,0.0f,0.0f };
		rotate = { 0.0f,0.0f,0.0f };
		scale = { 1.0f,1.0f,1.0f };
	}

	void reset() {
		after_translate = { 0.0f,0.0f,0.0f };
		after_rotate = { 0.0f,0.0f,0.0f };
		after_scale = { 1.0f,1.0f,1.0f };
		translate = { 0.0f,0.0f,0.0f };
		st_translate = { 0.0f,0.0f,0.0f };
		rotate = { 0.0f,0.0f,0.0f };
		scale = { 1.0f,1.0f,1.0f };
	}

	void setlocation(const float& x, const float& y, const float& z) {
		translate.x = x;
		translate.y = y;
		translate.z = z;
	}

	void setScale(const float& x, const float& y, const float& z);

	void afterScale(const float& x, const float& y, const float& z);

	void Y_spin(const float&,  const bool& degreeb);// y축으로 자전시키는 함수
	
	void X_spin(const float&); // z축으로 자전시키는 함수
	
	void go_trans(const float& x, const float& y, const float& z);// 이동

	void aftertranslate(const float& x, const float& y, const float& z);

	void setWorldTransform(glm::mat4& transformMatrix) const;

	void trans_after_translate(glm::mat4& transformMatrix) const {
		transformMatrix = glm::translate(transformMatrix, after_translate);
	}

	void trans_after_rotate(glm::mat4& transformMatrix) const {
		transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.z), z_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.y), y_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.x), x_axis);
	}
	void trans_after_scale(glm::mat4& transformMatrix) const {
		transformMatrix = glm::scale(transformMatrix, after_scale);
	}
	void trans_stt_translate(glm::mat4& transformMatrix) const {
		transformMatrix = glm::translate(transformMatrix, - st_translate);
	}
	void trans_translate(glm::mat4& transformMatrix) const {
		transformMatrix = glm::translate(transformMatrix, translate);
	}
	void trans_rotate(glm::mat4& transformMatrix) const {
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.z), z_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.x), x_axis);
	}
	void trans_scale(glm::mat4& transformMatrix) const {
		transformMatrix = glm::scale(transformMatrix, scale);
	}
	void trans_st_translate(glm::mat4& transformMatrix) const {
		transformMatrix = glm::translate(transformMatrix, st_translate);
	}
};
int neg{ 1 };
void Object::X_spin(const float& degree) {
	rotate.x += degree * neg;

	//각도 범위 유지
	while (rotate.x > 90.0f) {
		rotate.x = 90.0f;
		neg *= -1;
	}
	while (rotate.x < -90.0f) {
		rotate.x = -90.0f;
		neg *= -1;
	}
}
void Object::Y_spin(const float& degree , const bool& degreeb) {
	if (degreeb) {
		rotate.y += degree * neg;
		if (rotate.y > 50.0f || rotate.y == 0.0f) {
			neg *= -1;
		}
	}
	else {//각도 범위 유지
		rotate.y += degree;

		while (rotate.y > 360.0f) rotate.y -= 360.0f;
		while (rotate.y < 0.0f) rotate.y += 360.0f;
	}
	
	;
}

void Object::setScale(const float& x, const float& y, const float& z) {
	scale.x = x;
	scale.y = y;
	scale.z = z;
}
void Object::afterScale(const float& x, const float& y, const float& z) {
	after_scale.x = x;
	after_scale.y = y;
	after_scale.z = z;
}
void Object::aftertranslate(const float& x, const float& y, const float& z) {
	after_translate.x += x;
	after_translate.y += y;
	after_translate.z += z;
}
void Object::go_trans(const float& x, const float& y, const float& z) {
	after_translate.x += x;
	after_translate.y += y;
	after_translate.z += z;
}


Object Square_bottom, Square_top, Square_bottom_barrel[2], Square_top_barrel[2], All, Mid, Bottom_barrel[2];
Mesh mcube, mpyramid;
Linexyz line;
Floor_Mesh floor_b;
Camera camera_num[3];

void make_shaderProgram();
void getobjfile(Mesh& mesh, const std::string& objname);
void reset();

//------------------------------------------------------------------------------------------------------
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 0);
	glutInitWindowSize(1000, 800);
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
	glutSpecialFunc(specialkeyborad);
	glutMouseFunc(Mouse); // 마우스 입력
	glutMotionFunc(Motion); // 마우스 입력
	glutMouseWheelFunc(handleMouseWheel);	//마우스 휠 콜백 합수 지정
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(Keyboardup);

	glutMainLoop();
}
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
////-------------------------------------------------------------------------------------------------------------------------
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
//전역변수 ----------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
bool DEPTH_T{ true }; // 은면제거

static glm::vec3 cameraPos = glm::vec3{0.0f, 1.0f, 1.0f }, cameraDirection{ zero }, cameraUp{ y_axis };

glm::vec3 camer_n = glm::normalize(cameraDirection - cameraPos);
glm::vec3 camer_u = glm::cross(camer_n, cameraUp);
glm::vec3 camer_v = glm::cross(camer_n, camer_u);
bool t_or_l{ true };//면 또는 선
bool left_button{ false }; //좌클릭
int mouse_x{ 0 }, mouse_y{ 0 }; // 마우스 좌표
bool b_animation{ false };
bool b_camera{ false };
bool ortho{ false }; // 투영변환
int all_animation{ 0 }; // 애니메인션 0:x,1:1, 2:2, 3:3, 4:t, 5:r
int obj_style{ 1 }; // 1: GLU_LINE, 2:GLU_FILL
GLfloat degree{ 0.0f }; // 좌클릭시 회전각
float startb_b;// Bottom_barrel[0].translate.x;
int setca{ 0 };
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

void setColor(const glm::vec3& color) {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "ucolor");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniform3fv(modelLocation, 1, &color[0]);		//--- modelTransform 변수에 변환 값 적용하기
}
void Color_option(const bool& vertex_color) {// true: 정점 색상, false: 내가 넣은 색상
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "usev_color");
	glUniform1i(modelLocation, vertex_color);		
}
void draw_object();
//------------------------------------------------------------------------------------------------------
void make_vertexShaders()//--- 프래그먼트 세이더 객체 만들기
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
void make_shaderProgram()//--- 버퍼 생성하고 데이터 받아오기
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
//------------------------------------------------------------------------------------------------------
//----------------Mouse----------------------------------
GLvoid Mouse(int button, int state, int x, int y) {

	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse_x = mx;
		mouse_y = my;
		//degree += 10.0f;

		left_button = true;

	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

		left_button = false;
	}

	glutPostRedisplay(); // 화면 다시 그리기 요청
}
GLvoid Motion(int x, int y) {
	//중앙값
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);
	if (left_button == true) {
		float dx = mx - mouse_x;
		float dy = my - mouse_y;
		//x 변환
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos = glm::rotate(glm::mat4{1.0f}, glm::radians(dx * 1), y_axis) * glm::vec4{ camera_num[setca].cameraPos, 1.0f};
		}
		//cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(dx * 1), y_axis) * glm::vec4{ cameraPos, 1.0f };

		//std::cout << "dy : " << dy << '\n';
		
		//y 변환

		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].n = glm::normalize(camera_num[setca].cameraDirection - camera_num[setca].cameraPos);
			camera_num[setca].u = glm::cross(camera_num[setca].n, camera_num[setca].cameraUp);
		}

		//glm::vec3 n = glm::normalize(cameraDirection - cameraPos);
		//glm::vec3 u = glm::cross(n, cameraUp);
		//cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(dy * 1), u) * glm::vec4{ cameraPos, 1.0f };

		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(dy * 1), camera_num[setca].u) * glm::vec4{ camera_num[setca].cameraPos, 1.0f };
		}
		mouse_x = mx;
		mouse_y = my;

	}

	glutPostRedisplay(); // 화면 다시 그리기 요청
}
float zoom{ 10.0f };//zoom > 0, 크면 더 뒤로감

GLvoid handleMouseWheel(int wheel, int direction, int x, int y) {
	//wheel : 마우스 휠의 상태(눌려져있을때 1, 아님 0)
	//direction: 휠의 돌아간 방향 (1: 앞으로, -1: 뒤로, 0: 휠 가만히)
	//x,y : 마우스 위치
	if (direction < 0) {	//휠을 아래로 돌림
		zoom += 1.0f;
	}
	else if (direction > 0) {	//휠을 위로 돌림
		zoom -= 1.0f;
		if (zoom < 0.0f) {
			zoom += 1.0f;
		}
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

//----------------Timer-----------------------------------
int b_keyboard;
void Timer_obj(int value){
	int neg{ 1 };//방향
	float ch{ 0.01f };
	float cr{ 1.0f }; //각도
	float rotationAngle = glm::radians(5.0f);
	
	const int B{ 1 };//"b/B: 크레인의 아래 몸체가 x 축 방향으로 양 음 방향으로 이동한다 . 다시 누르면 멈춘다",
	const int b{ 2 };
	const int M{ 3 };//"m/M: 크레인의 중앙 몸체가 y 축에 대하여 양 음 방향으로 회전한다 . 다시 누르면 멈춘다",
	const int m{ 4 };
	const int F{ 5 };//"f/F: 포신이 y 축에 대하여 양 음 방향으로 회전하는데 , 두 포신이 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
	const int f{ 6 };
	const int T{ 7 };//"t/T : 크레인의 맨 위 2 개의 팔이 x 축에 대하여 양 음 방향으로 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
	const int t{ 8 };
	const int G{ 9 };//"G/g: 전체 객체들을 y축으로 양/음 방향으로 회전 (중앙의 구의 y축에 대하여 회전)",
	const int g{ 10 };
	const int E{ 19 };//"e/E: 2 개 포신이 조금씩 이동해서 한 개가 된다 다시 제자리로 이동해서 2 개가 된다",
	const int e{ 20 };
	switch (b_keyboard) {
	case B://"b/B: 크레인의 아래 몸체가 x 축 방향으로 양 음 방향으로 이동한다 . 다시 누르면 멈춘다",
		All.aftertranslate(0.1f, 0.0f, 0.0f);
		break;
	case b:
		All.aftertranslate(-0.1f, 0.0f, 0.0f);
		break;
	case M://"m/M: 크레인의 중앙 몸체가 y 축에 대하여 양 음 방향으로 회전한다 . 다시 누르면 멈춘다",
		Mid.Y_spin(1.0f, false);
		break;
	case m:
		Mid.Y_spin(-1.0f, false);
		break;
	case F://"f/F: 포신이 y 축에 대하여 양 음 방향으로 회전하는데 , 두 포신이 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
		for (int i = 0; i < 2; ++i) {
			Bottom_barrel[i].Y_spin(0.5f * neg, true);
			neg *= -1;
		}
		break;
	case f:
		for (int i = 0; i < 2; ++i) {
			Bottom_barrel[i].Y_spin(0.5f * -neg,true);
			neg *= -1;
		}
		break;
	case T://"t/T : 크레인의 맨 위 2 개의 팔이 x 축에 대하여 양 음 방향으로 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
		for (int i = 0; i < 2; ++i) {
			Square_top_barrel[i].X_spin(1.0f * -neg);
			neg *= -1;
		}
		break;
	case t:
		for (int i = 0; i < 2; ++i) {
			Square_top_barrel[i].X_spin(1.0f * neg);
			neg *= -1;
		}
		break;
	case G:	//"G/g: 전체 객체들을 y축으로 양/음 방향으로 회전 (중앙의 구의 y축에 대하여 회전)",
		All.Y_spin(1.0f, false);
		break;
	case g:
		All.Y_spin(-1.0f,false);
		break;
	case E:	//"e/E: 2 개 포신이 조금씩 이동해서 한 개가 된다 다시 제자리로 이동해서 2 개가 된다",

		if (Bottom_barrel[1].after_translate.x > -3.0f and b_animation == true) {
			for (int i = 0; i < 2; ++i) {

				Bottom_barrel[i].go_trans(ch, 0.0f, 0.0f);
				ch *= -1;
				//std::cout << "Bottom_barrel[ "<< i <<"].after_translate.x : " << Bottom_barrel[i].after_translate.x << '\n';
			}
		}
		else {
			for (int i = 0; i < 2; ++i) {
				Bottom_barrel[i].after_translate.x = 3 * ch;
				ch *= -1;
			}
		}
		break;
	case e:
		
		for (int i = 0; i < 2; ++i) {
			Bottom_barrel[i].after_translate.x += ch;
			ch *= -1;
		}
		break;
	}

	glutPostRedisplay();
	if (b_animation)
		glutTimerFunc(10, Timer_obj, 0);
}
void Timer_Camera(int value) {
	int neg{ 1 };//방향
	float ch{ 0.01f };
	float cr{ 1.0f }; //각도
	float camera_R{ 1.0f };
	glm::vec3 camer_new{ 0.0f };//at-eye
	float rotationAngle = glm::radians(5.0f);
	const int Z{ 11 };//"z/Z: 카메라가 z 축 양/음 방향으로 이동",
	const int z{ 12 };
	const int X{ 13 };//"x/X: 카메라가 x 축 양 음 방향으로 이동",
	const int x{ 14 };
	const int Y{ 15 };//"y/Y: 카메라 기준 y 축에 대하여 회전",
	const int y{ 16 };
	const int R{ 17 };//"r/R: 화면의 중심의 y 축에 대하여 카메라가 회전 중점에 대하여 공전",
	const int r{ 18 };
	const int A{ 21};
	switch (b_keyboard) {
	case Z:	//"z/Z: 카메라가 z 축 양/음 방향으로 이동",
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos.z += 0.01f;
		}
		
		break;
	case z:
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos.z -= 0.01f;
		}
		
		break;

	case X:	//"x/X: 카메라가 x 축 양 음 방향으로 이동",
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos.x += 0.01f;
		}
		
		break;
	case x:
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos.x -= 0.01f;
		}
		break;

	case Y:	//"y/Y: 카메라 기준 y 축에 대하여 회전",
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(ch * 10), y_axis) * glm::vec4{ camera_num[setca].cameraPos, 1.0f };
		}
		break;
	case y:
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(-ch * 10), y_axis) * glm::vec4{ camera_num[setca].cameraPos, 1.0f };
		}
		break;

	case R:	//"r/R: 화면의 중심의 y 축에 대하여 카메라가 회전 중점에 대하여 공전",
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(cr), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_num[setca].cameraPos, 1.0f);
		}
	
		break;
	case r:
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(-cr), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_num[setca].cameraPos, 1.0f);
		}
		break;
	case A:
		for (int setca = 0; setca < 3; ++setca) {
			camera_num[setca].cameraDirection = glm::rotate(glm::mat4{ 1.0f }, glm::radians(camera_R), y_axis) * glm::vec4{ camera_num[setca].cameraDirection - camera_num[setca].cameraPos,1.0f } + glm::vec4{ camera_num[setca].cameraPos,1.0f };
		}
		
		break;
	}

	glutPostRedisplay();
	if (b_camera)
		glutTimerFunc(10, Timer_Camera, 0);
}
//"--------------------------------------------------------------------------------------------------",
//--------keyboard----------------------------------------
GLvoid Keyboard(unsigned char key, int x, int y) {
	//빨강:x, 가로, 파랑:z,세로
	std::vector<float> vertex;
	switch (key)
	{
	case'Q':case'q':
		exit(0);
		break;

	case'c':case'C'://"c/C:초기화",
		for (int setca = 0; setca < 3; ++setca) {
			if (setca == 0) {
				camera_num[setca].cameraPos = glm::vec3{ 0.0f, 1.0f, 1.0f };
				camera_num[setca].cameraDirection = glm::vec3{ zero };
				camera_num[setca].cameraUp = glm::vec3{ y_axis };
			}
			else if (setca == 1) {
				camera_num[setca].cameraPos = glm::vec3{ 0.0f,1.0f,0.0f };
				camera_num[setca].cameraDirection = glm::vec3{ zero };
				camera_num[setca].cameraUp = glm::vec3{ x_axis };
			}

			else {
				camera_num[setca].cameraPos = glm::vec3{ 1.0f,0.0f,0.0f };
				camera_num[setca].cameraDirection = glm::vec3{ zero };
				camera_num[setca].cameraUp = glm::vec3{ y_axis };
			}
		}
		cameraPos = glm::vec3{ 0.0f, 1.0f, 1.0f };
		cameraDirection = { zero };
		cameraUp = { y_axis };
		b_animation = { false };
		b_camera = { false };
		Square_bottom.reset();
		Square_top.reset();
		All.reset();
		Mid.reset();
		for (Object& o : Square_bottom_barrel) {
			o.reset();
		}
		for (Object& o : Square_top_barrel) {
			o.reset();
		}
		for (Object& o : Bottom_barrel) {
			o.reset();
		}
		reset();
		break;
	case'2'://2: 은면제거 적용/해제
		DEPTH_T = DEPTH_T == true ? false : true;
		break;
	case'1'://"1: 직각 투영/원근 투영",
		ortho = ortho == true ? false : true;
		break;;
	case'B'://"b/B: 크레인의 아래 몸체가 x 축 방향으로 양 음 방향으로 이동한다 . 다시 누르면 멈춘다",
		b_keyboard = 1;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'b':
		b_keyboard = 2;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'M'://"m/M: 크레인의 중앙 몸체가 y 축에 대하여 양 음 방향으로 회전한다 . 다시 누르면 멈춘다",
		b_keyboard = 3;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'm':
		b_keyboard = 4;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'F'://"f/F: 포신이 y 축에 대하여 양 음 방향으로 회전하는데 , 두 포신이 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
		b_keyboard = 5;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'f':
		b_keyboard = 6;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'T'://"t/T : 크레인의 맨 위 2 개의 팔이 z 축에 대하여 양 음 방향으로 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
		b_keyboard = 7;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case't':
		b_keyboard = 8;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'G'://"g/G: 전체 객체들을 y축으로 양/음 방향으로 회전 (중앙의 구의 y축에 대하여 회전)",
		b_keyboard = 9;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'g':
		b_keyboard = 10;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;
	case'S':case's'://"s/S: 모든 움직임 멈추기",
		b_animation = false;
		b_camera = false;
		break;
	case'Z'://"z/Z: 카메라가 z 축 양 음 방향으로 이동",
		b_keyboard = 11;
		b_camera = true;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
	case'z':
		b_camera = true;
		b_keyboard = 12;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
		
	case'X'://"x/X: 카메라가 x 축 양 음 방향으로 이동",
		b_camera = true;
		b_keyboard = 13;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
	case'x':
		b_camera = true;
		b_keyboard = 14;
		glutTimerFunc(10, Timer_Camera, 0);
		break;

	case'Y'://"y/Y: 카메라 기준 y 축에 대하여 회전",
		b_camera = true;
		b_keyboard = 15;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
	case'y':
		b_camera = true;
		b_keyboard = 16;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
	case'R'://"r/R: 화면의 중심의 y 축에 대하여 카메라가 회전 중점에 대하여 공전",
		b_camera = true;
		b_keyboard = 17;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
	case'r':
		b_camera = true;
		b_keyboard = 18;
		glutTimerFunc(10, Timer_Camera, 0);
		break;
		
	case'a':case'A'://"a/A: r 명령어와 같이 화면의 중심의 축에 대하여 카메라가 회전하는 애니메이션을 진행한다 멈춘다",
	
		b_keyboard = 21;
		glutTimerFunc(10, Timer_Camera, 0);
		b_camera = b_camera == true ? false : true;
		break;

	case'e':case'E'://"e/E: 2 개 포신이 조금씩 이동해서 한 개가 된다 다시 제자리로 이동해서 2 개가 된다",
		for (Object& o : Bottom_barrel) {
			o.reset();
		}
		startb_b = Bottom_barrel[0].after_translate.x;//Bottom_barrel[0].translate.x;
		b_keyboard = 19;
		b_animation = b_animation == true ? false : true;
		glutTimerFunc(10, Timer_obj, 0);
		break;


	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}
GLvoid Keyboardup(unsigned char key, int x, int y) {
	std::vector<float> vertex;
	switch (key)
	{
	case'Q':case'q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}
GLvoid specialkeyborad(int key, int x, int y) {//← / → / ↑ / ↓ : 좌 / 우 / 상 / 하로 객체를 이동한다. (x축, y축으로 이동)
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

int scene_ch{};
// ---- 그리기 콜백 함수------------------------------------------------------------------------------------
GLvoid drawScene()
{
	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);//버퍼 초기화, 배경색
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//버퍼 초기화, 배경색			
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);//--- 렌더링 파이프라인에 세이더 불러오기

	//0
	{
		scene_ch = 0;
		glViewport(50, 50, 400, 400);
		{
			Color_option(true);
			setColor({ 1.0f,0.0f,0.0f });
		}
		depth_choice();//은면제거
		projectTransform_choice();//원근투영,직각투영
		viewTransform();//뷰변환
		draw_coordinate_axis();//좌표축 그리기
		draw_floor();//바닥그리기
		draw_object();// 오브젝트 출력
	}
	//1 직각 투영 xz 평면
	{
		scene_ch = 1;
		glViewport(500, 50, 400, 400);
		{
			Color_option(true);
			setColor({ 1.0f,0.0f,0.0f });
		}
		depth_choice();//은면제거


		projectTransform_choice();//원근투영,직각투영
		viewTransform();//뷰변환

		draw_coordinate_axis();//좌표축 그리기
		draw_floor();//바닥그리기

		draw_object();// 오브젝트 출력
	}
	//2 직각 투영 xy 평면
	{
		scene_ch = 2;
		glViewport(500, 500, 300, 300);
		{
			Color_option(true);
			setColor({ 1.0f,0.0f,0.0f });
		}

		depth_choice();//은면제거
		projectTransform_choice();//원근투영,직각투영
		viewTransform();//뷰변환

		draw_coordinate_axis();//좌표축 그리기
		draw_floor();//바닥그리기

		draw_object();// 오브젝트 출력
	}


	glutSwapBuffers();//--- 화면에 출력하기		
}

void viewTransform()
{
	
	
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camera_num[scene_ch].cameraPos, camera_num[scene_ch].cameraDirection, camera_num[scene_ch].cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");  //뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	

}

void projectTransform_choice()
{
	// 투영변환
	if (ortho) {  //직각투영
		//std::cout << "직각투영" << "\n";
		glm::mat4 projection = glm::mat4(1.0f);
		float len{ 10.0f };
		projection = glm::ortho(-len, len, -len, len, -len, len);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	}
	// 원근투영
	else {
		//std::cout << "원근투영" << "\n";
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 300.0f); //투영 공간 설정 : fovy, aspect, near, far
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -zoom));
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
}

void depth_choice()
{
	if (DEPTH_T) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

void draw_coordinate_axis()
{
	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

		glBindVertexArray(line.vao);
		glLineWidth(4);
		{
			Color_option(false);
			setColor({ 1.0f,0.0f,0.0f });
		}
		glDrawArrays(GL_LINES, 0, 2);
		{
			setColor({ 0.0f,1.0f,0.0f });
		}
		glDrawArrays(GL_LINES, 2, 2);
		{
			setColor({ 0.0f,0.0f,1.0f });
		}
		glDrawArrays(GL_LINES, 4, 2);

	}
}

void draw_floor()
{
	{
		glBindVertexArray(floor_b.vao);								//--- 사용할 VAO 불러오기
		if (t_or_l) {//면으로 출력
			{
				Color_option(true);
			}
			glm::mat4 transformMatrix(1.0f);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(45.0f), y_axis);
			transformMatrix = glm::scale(transformMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
			transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

			glDrawElements(GL_TRIANGLES, floor_b.indexnum, GL_UNSIGNED_INT, 0);
		}
	}
}

void draw_object()
{
	void drawGLU();	//사용하기전에 선언.
	void draw_obj();
	{
		glm::mat4 transform{ 1.0f };
		All.setWorldTransform(transform);
		Square_bottom.setWorldTransform(transform);
		{
			Color_option(false);
			setColor({ 0.0f,0.0f,0.0f });
		}
		draw_obj();
	}
	{
		for (int i = 0; i < 2; ++i) {
			glm::mat4 transform{ 1.0f };
			All.setWorldTransform(transform);
			Bottom_barrel[i].setWorldTransform(transform);
			Square_bottom_barrel[i].setWorldTransform(transform);
			{
				Color_option(false);
				setColor({ 0.0f,0.0f,1.0f });
			}
			draw_obj();
		}
	}
	{
		glm::mat4 transform{ 1.0f };
		All.setWorldTransform(transform);
		Mid.setWorldTransform(transform);
		Square_top.setWorldTransform(transform);
		{
			Color_option(false);
			setColor({ 1.0f,1.0f,1.0f });
		}
		draw_obj();
	}
	{
		for (int i = 0; i < 2; ++i) {
			glm::mat4 transform{ 1.0f };
			All.setWorldTransform(transform);
			Mid.setWorldTransform(transform);
			Square_top_barrel[i].setWorldTransform(transform);
			{
				Color_option(false);
				setColor({ 0.0f,1.0f,0.0f });
			}
			draw_obj();
		}
	}

}


void modelUniform(const glm::mat4& transformMatrix) {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
}
void Object::setWorldTransform(glm::mat4& transformMatrix) const {
	trans_after_translate(transformMatrix);
	trans_after_rotate(transformMatrix);
	trans_after_scale(transformMatrix);
	trans_stt_translate(transformMatrix);
	trans_translate(transformMatrix);
	trans_rotate(transformMatrix);
	trans_scale(transformMatrix);
	trans_st_translate(transformMatrix);
	modelUniform(transformMatrix);
}
void draw_obj() {
	Mesh& m = mcube;
	glBindVertexArray(m.vao);								//--- 사용할 VAO 불러오기
	if (t_or_l) {//면으로 출력
		for (int j = 0; j < m.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
	else { //선으로 출력
		for (int j = 0; j < m.indexnum / 2; j++) {
			glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));	//큐브 1면 출력
		}
	}
}
void drawGLU() {
	GLUquadricObj* qobj;
	qobj = gluNewQuadric(); // 객체 생성하기
	if (obj_style == 1) {
		gluQuadricDrawStyle(qobj, GLU_LINE);// 도형 스타일
	}
	else {
		gluQuadricDrawStyle(qobj, GLU_FILL);// 도형 스타일
	}
	gluQuadricNormals(qobj, GLU_SMOOTH); 
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	gluSphere(qobj, 1.0, 50, 50); // 객체 만들기
}

void InitBuffer_Line_xyz(Linexyz& l) {
	//버퍼 생성
	std::vector<float> vertex;
	std::vector<float> color;

	for (int i = 0; i < 6; ++i)
	{
		vertex.push_back(l.p[i].x);
		vertex.push_back(l.p[i].y);
		vertex.push_back(l.p[i].z);

		color.push_back(l.color.at(i * 3));		//r
		color.push_back(l.color.at(i * 3 + 1));	//g
		color.push_back(l.color.at(i * 3 + 2));	//b
	}

	//gpu 버퍼에 저장하기
	{
		glGenVertexArrays(1, &l.vao); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(l.vao);		//--- VAO를 바인드하기
		glGenBuffers(2, l.vbo);		//--- 2개의 VBO를 지정하고 할당하기

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, l.vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, l.vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glEnableVertexAttribArray(1);

	}

	vertex.clear();
	color.clear();
}

void InitBuffer_floor(Floor_Mesh& s) {
	//버퍼 생성
	std::vector<float> s_vertex;
	std::vector<float> l_vertex;
	std::vector<float> vertex;

	std::vector<float> color;


	std::vector<unsigned int> index;
	float degree{};
	int count{};
	while (degree < 360) {
		vertex.push_back(cos(glm::radians(degree)));
		vertex.push_back(0.0f);
		vertex.push_back(sin(glm::radians(degree)));
		index.push_back(count);
		//검은색
		color.push_back(randomnum(0.0f,1.0f));	//r
		color.push_back(randomnum(0.0f, 1.0f));	//g
		color.push_back(randomnum(0.0f, 1.0f));	//b
		degree += 90.0f;
		count++;
	}

	index.push_back(0);		
	index.push_back(2);		
	index.push_back(1);		
	index.push_back(1);
	index.push_back(3);
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

void reset() {
	// 축 초기화
	InitBuffer_Line_xyz(line);
	InitBuffer_floor(floor_b);

	// 오브젝트 초기화
	Square_bottom.go_trans(0.0f, 1.0f, 0.0f);
	Square_bottom.setScale(2.0f, 1.0f, 2.0f);


	int count = -1;
	for (Object& b_b : Square_bottom_barrel) {
		b_b.go_trans(1.5f * count, 0.2f, 2.8f) ;
		b_b.setScale(0.2f, 0.2f, 0.8f);
		count = 1;
	}

	Square_top.go_trans(0.0f, 2.5f, 0.0f);
	Square_top.setScale(1.0f, 0.5f, 1.0f);

	count = -1;
	for (Object& t_b : Square_top_barrel) {
		t_b.go_trans(0.5f * count, 3.6f, 0.0f);
		t_b.setScale(0.2f, 0.6f, 0.2f);
		count = 1;
	}


	for (int setca = 0; setca < 3; ++setca) {
		if (setca == 0) {
			camera_num[setca].cameraPos = glm::vec3{ 0.0f, 1.0f, 1.0f };
			camera_num[setca].cameraDirection = glm::vec3{ zero };
			camera_num[setca].cameraUp = glm::vec3{ y_axis };
		}
		else if (setca == 1) {
			camera_num[setca].cameraPos = glm::vec3{ 0.0f,1.0f,0.0f };
			camera_num[setca].cameraDirection = glm::vec3{ zero };
			camera_num[setca].cameraUp = glm::vec3{ x_axis };
		}

		else {
			camera_num[setca].cameraPos = glm::vec3{ 1.0f,0.0f,0.0f };
			camera_num[setca].cameraDirection = glm::vec3{ zero };
			camera_num[setca].cameraUp = glm::vec3{ y_axis };
		}
	}
}



GLvoid Reshape(int w, int h){//--- 다시그리기 콜백 함수

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
			/*std::cout << "vertex[" << vertIndex << "] : {" << vertex[vertIndex].x << ", " << vertex[vertIndex].y << ", " << vertex[vertIndex].z << "}" << '\n';
			std::cout << "color[" << vertIndex << "] : {" << color[vertIndex].x << ", " << color[vertIndex].y << ", " << color[vertIndex].z << "}" << '\n';*/
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
			/*std::cout << "face[" << faceIndex << "] : {" << face[faceIndex].x << ", " << face[faceIndex].y << ", " << face[faceIndex].z << "}" << '\n';*/
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

