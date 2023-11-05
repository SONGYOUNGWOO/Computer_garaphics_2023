#include "DEFAULT.hpp"

#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것r
//------------------------------------------------------------------------------------------------------
//glew32.lib freeglut.lib 
//------------------------------------------------------------------------------------------------------
const std::string Guide[]{
	"--------------------------------------------------------------------------------------------------",
	"로봇 그리기",
	"--------------------------------------------------------------------------------------------------",
	"w/a/s/d: 로봇이 앞 뒤 좌 우 방향으로 이동 방향을 바꿔서 걷는다 . 가장자리에 도달하면 로봇은 뒤로 돌아 다시 걷는다",
	"+/-: 걷는 속도가 빨라지거나/느려진다. 속도가 바뀔 때 걷는 다리의 각도가 늘어나거나/줄어든다.",
	"j: 로봇이 제자리에서 점프한다. 장애물 위로 점프하여 올라가고 내려갈 수 있다.",
	"i: 모든 변환을 리셋하고 다시 시작",
	"g/G: 전체 객체들을 y축으로 양/음 방향으로 회전 (중앙의 구의 y축에 대하여 회전)",
	"--------------------------------------------------------------------------------------------------",
	"카메라 설정: 키보드 명령으로 카메라 이동",
	"--------------------------------------------------------------------------------------------------",
	"z/Z: 앞뒤로 이동",
	"x/X: 좌우로 이동",
	"y/Y: 카메라가 현재 위치에서 화면 중심 y축을 기준으로 공전",
	"--------------------------------------------------------------------------------------------------",
	"o/O: 앞면이 좌우로 열린다",
	"1: 직각 투영/원근 투영",
	"2: 은면제거 적용/해제",
	"C/c: 초기화",
	"q : 프로그램 종료",
	"--------------------------------------------------------------------------------------------------"
};
//------------------------------------------------------------------------------------------------------

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

	float glu_radius; //glu 반지름
	int glu_slice;	//glu 점개수

	Object() {
		after_translate = { 0.0f,0.0f,0.0f };
		after_rotate = { 0.0f,0.0f,0.0f };
		after_scale = { 1.0f,1.0f,1.0f };
		translate = { 0.0f,0.0f,0.0f };
		st_translate = { 0.0f,0.0f,0.0f };
		rotate = { 0.0f,0.0f,0.0f };
		scale = { 1.0f,1.0f,1.0f };
		glu_radius = 0.1f;
		glu_slice = 50;
		
	}

	void reset() {
		after_translate = { 0.0f,0.0f,0.0f };
		after_rotate = { 0.0f,0.0f,0.0f };
		after_scale = { 1.0f,1.0f,1.0f };
		translate = { 0.0f,0.0f,0.0f };
		st_translate = { 0.0f,0.0f,0.0f };
		rotate = { 0.0f,0.0f,0.0f };
		scale = { 1.0f,1.0f,1.0f };
		glu_radius = 0.1f;
		glu_slice = 50;
	}

	void setlocation(const float& x, const float& y, const float& z) {
		translate.x = x;
		translate.y = y;
		translate.z = z;
	}

	void setlocation(const glm::vec3& vecter) {
		translate = vecter;
	}

	void setScale(const float& x, const float& y, const float& z);
	void setScale(const glm::vec3& vecter);

	void afterScale(const float& x, const float& y, const float& z = 1.0f);
	void afterScale(const glm::vec3& vecter);

	void Y_spin(const float&,  const bool& degreeb);// y축으로 자전시키는 함수
	
	void X_spin(const float&); // z축으로 자전시키는 함수
	
	void go_trans(const float& x, const float& y, const float& z);// 이동
	void go_trans(const glm::vec3& vecter);// 이동

	void aftertranslate(const float& x, const float& y, const float& z);
	void aftertranslate(const glm::vec3& vecter);

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

//-------------------

class Robot : public Object {
private:
	Object head;
	Object nose;
	Object body;
	Object left_arm;
	Object right_arm;
	Object left_leg;
	Object right_leg;

	bool arm_leg_di{ false }; //로복 팔 방향
	float arm_leg_deg{ 0.0f }; //로봇 팔 각도
	
public : 
	Robot() : Object() {
		head.reset();
		nose.reset();
		body.reset();
		left_arm.reset();
		right_arm.reset();
		left_leg.reset();
		right_leg.reset();
		reset(); // Robot
	}
	float speed{ 0.05f };
	float robot_di{ 0.0f };//로봇이 봐야하는 방향
	float robot_re_di{ 0.0f };//로봇이 현재 보는 방향

	bool wpress{ false };//키입력 감지
	bool spress{ false };
	bool apress{ false };
	bool dpress{ false };

	void robot_reset() {
		head.setlocation(0.0f, 6.0f, 0.0f);
		head.setScale(glm::vec3{ 1.0f });

		nose.setlocation(0.0f, 6.0f, 1.0f);
		nose.setScale(glm::vec3{ 0.1f });

		body.setlocation(0.0f, 4.0f, 0.0f);
		body.setScale(glm::vec3{ 1.0f });

		left_arm.setlocation(-1.5f, 3.5f, 0.0f);
		left_arm.setScale(0.5f, 1.5f, 0.5f);

		right_arm.setlocation(1.5f, 3.5f, 0.0f);
		right_arm.setScale(0.5f, 1.5f, 0.5f);

		left_leg.setlocation(-0.5f, 1.5f, 0.0f);
		left_leg.setScale(0.5f, 1.5f, 0.5f);

		right_leg.setlocation(0.5f, 1.5f, 0.0f);
		right_leg.setScale(0.5f, 1.5f, 0.5f);


	}


	void render_head(const glm::vec3  axis[3]);
	void render_nose(const glm::vec3  axis[3]);
	void render_body(const glm::vec3  axis[3]);
	void render_arms(const glm::vec3  axis[3]);
	void render_legs(const glm::vec3  axis[3]);

	void render();
	
	void motion();

};


int neg{ 1 };//방향변경용
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
void Object::setScale(const glm::vec3& vecter) {
	scale = vecter;
}
void Object::afterScale(const float& x, const float& y, const float& z) {
	after_scale.x = x;
	after_scale.y = y;
	after_scale.z = z;
}
void Object::afterScale(const glm::vec3& vecter) {
	after_scale = vecter;
}
void Object::aftertranslate(const float& x, const float& y, const float& z) {
	after_translate.x = x;
	after_translate.y = y;
	after_translate.z = z;
}
void Object::aftertranslate(const glm::vec3& vecter) {
	after_translate = vecter;
}
void Object::go_trans(const float& x, const float& y, const float& z) {
	after_translate.x = x;
	after_translate.y = y;
	after_translate.z = z;
}
void Object::go_trans(const glm::vec3& vecter) {
	after_translate = vecter;
}


Robot robot;
Mesh mcube, mpyramid;
Linexyz line;
Floor_Mesh floor_b;
std::vector<glm::vec3> color;

void make_shaderProgram();
void getobjfile(Mesh& mesh, const std::string& objname);
void reset();

void box_srt(glm::mat4& transformMatrix);

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

static glm::vec3 cameraPos = glm::vec3{0.0f, 20.0f, 20.0f }, cameraDirection{ zero }, cameraUp{ y_axis };
glm::vec3 camer_n = glm::normalize(cameraDirection - cameraPos);
glm::vec3 camer_u = glm::cross(camer_n, cameraUp);
glm::vec3 camer_v = glm::cross(camer_n, camer_u);

bool DEPTH_T{ true }; // 은면제거
bool t_or_l{ true };//면 또는 선
bool left_button{ false }; //좌클릭
int mouse_x{ 0 }, mouse_y{ 0 }; // 마우스 좌표
bool b_animation{ false };
bool b_camera{ false };
bool ortho{ false }; // 투영변환
int all_animation{ 0 }; // 애니메인션 0:x,1:1, 2:2, 3:3, 4:t, 5:r
int obj_style{ 2 }; // 1: GLU_LINE, 2:GLU_FILL
GLfloat degree{ 0.0f }; // 좌클릭시 회전각
float boxopen{0.0f};// box 0번 인덱스 정면의 메쉬가 회전할 각도
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
		
		cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(dx * 1), y_axis) * glm::vec4{ cameraPos, 1.0f };

		//std::cout << "dy : " << dy << '\n';
		
		//y 변환

		glm::vec3 n = glm::normalize(cameraDirection - cameraPos);
		glm::vec3 u = glm::cross(n, cameraUp);
		cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(dy * 1), u) * glm::vec4{ cameraPos, 1.0f };

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

void Robot::motion() {
	arm_leg_deg += arm_leg_di ? 1.0f : -1.0f;
	float max = glm::clamp(speed * 900, 2.0f, 100.0f);

	if (arm_leg_deg > max) {
		arm_leg_di = false;
	}
	if (arm_leg_deg < -max) {
		arm_leg_di = true;
	}
	//각 부위 적용

	left_arm.rotate.x = arm_leg_deg;
	right_arm.rotate.x = - arm_leg_deg;;
	left_leg.rotate.x = -arm_leg_deg;
	right_leg.rotate.x = arm_leg_deg;;

}


//----------------Timer-----------------------------------
int b_keyboard{};
void Timer_obj(int value){
	
	const int o{ 1 };//"o/O: 앞면이 좌우로 열린다",
	const int w{ 2 };//w/a/s/d: 로봇이 앞 뒤 좌 우 방향으로 이동 방향을 바꿔서 걷는다 . 가장자리에 도달하면 로봇은 뒤로 돌아 다시 걷는다
	const int s{ 3 };
	const int a{ 4 };
	const int d{ 5 };
	const int T{ 7 };//"t/T : 크레인의 맨 위 2 개의 팔이 x 축에 대하여 양 음 방향으로 서로 반대방향으로 회전한다 . 다시 누르면 멈춘다",
	const int t{ 8 };
	const int G{ 9 };//"G/g: 전체 객체들을 y축으로 양/음 방향으로 회전 (중앙의 구의 y축에 대하여 회전)",
	const int g{ 10 };
	const int E{ 19 };//"e/E: 2 개 포신이 조금씩 이동해서 한 개가 된다 다시 제자리로 이동해서 2 개가 된다",
	const int e{ 20 };

	
	robot.motion();



	switch (b_keyboard) {
	case o://"o/O: 앞면이 좌우로 열린다",
		if (boxopen < 120.0f) {
			boxopen += 1.0f;
			//std::cout << boxopen << '\n';
		}
		break;
	case w://"z축 -",
		if (!robot.wpress) {
			robot.Y_spin(robot.robot_di - robot.robot_re_di, false);
			robot.robot_re_di = robot.robot_di;
			robot.wpress = true;
			robot.spress = false;
			robot.apress = false;
			robot.dpress = false;
		}
		if (-50.0f < robot.translate.z  and robot.translate.z < 50.0f) {
			robot.translate.z -= robot.speed;
		}
	
		break;
	case s://"z축 +",
		if (!robot.spress) {
			robot.Y_spin(robot.robot_di - robot.robot_re_di, false);
			robot.robot_re_di = robot.robot_di;
			robot.spress = true;
			robot.wpress = false;
			robot.apress = false;
			robot.dpress = false;
		}
		if (-50.0f < robot.translate.z and robot.translate.z < 50.0f) {
			robot.translate.z += robot.speed;
		}
		break;
	case a://"x축 -",
		if (!robot.apress) {
			robot.Y_spin(robot.robot_di - robot.robot_re_di, false);
			robot.robot_re_di = robot.robot_di;
			robot.apress = true;
			robot.spress = false;
			robot.wpress = false;
			robot.dpress = false;
		}
		if (-50.0f < robot.translate.z and robot.translate.z < 50.0f) {
			robot.translate.x -= robot.speed;
		}
		break;
	case d://"x축 +",
		if (!robot.dpress) {
			robot.Y_spin(robot.robot_di - robot.robot_re_di, false);
			robot.robot_re_di = robot.robot_di;
			robot.dpress = true;
			robot.spress = false;
			robot.apress = false;
			robot.wpress = false;
		}
		if (-50.0f < robot.translate.z and robot.translate.z < 50.0f) {

			robot.translate.x += robot.speed;
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
	
		cameraPos.z += 0.01f;
		break;
	case z:
	
		cameraPos.z -= 0.01f;
		break;

	case X:	//"x/X: 카메라가 x 축 양 음 방향으로 이동",
		
		cameraPos.x += 0.01f;
		break;
	case x:
	
		cameraPos.x -= 0.01f;
		break;

	case Y:	//"y/Y: 카메라 기준 y 축에 대하여 회전",
		cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(ch * 10), y_axis) * glm::vec4{ cameraPos, 1.0f };
		break;
	case y:
		cameraPos = glm::rotate(glm::mat4{ 1.0f }, glm::radians(-ch * 10), y_axis) * glm::vec4{ cameraPos, 1.0f };
		break;

	case R:	//"r/R: 화면의 중심의 y 축에 대하여 카메라가 회전 중점에 대하여 공전",
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(cr), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case r:
		cameraPos = glm::rotate(glm::mat4(1.0f), glm::radians(-cr), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraPos, 1.0f);
		break;
	case A:
		cameraDirection = glm::rotate(glm::mat4{ 1.0f }, glm::radians(camera_R), y_axis) * glm::vec4{ cameraDirection - cameraPos,1.0f } + glm::vec4{ cameraPos,1.0f };
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
		robot.robot_reset();
		cameraPos = glm::vec3{ 0.0f, 1.0f, 1.0f };
		cameraDirection = { zero };
		cameraUp = { y_axis };
		b_animation = { false };
		b_camera = { false };
		reset();
		break;
	case'2'://2: 은면제거 적용/해제
		DEPTH_T = DEPTH_T == true ? false : true;
		break;
	case'1'://"1: 직각 투영/원근 투영",
		ortho = ortho == true ? false : true;
		break;;
	case'O': case'o'://"o/O: 앞면이 좌우로 열린다",
		b_animation = true;
		b_keyboard = 1;
		glutTimerFunc(10, Timer_obj, 0);
		break;

	case'W': case'w':
		if(!b_animation)
			glutTimerFunc(10, Timer_obj, 0);

		robot.robot_di = 180.0f;
		b_animation = true;
		b_keyboard = 2;
		break;
	case'A': case'a':
		if (!b_animation)
			glutTimerFunc(10, Timer_obj, 0);
		robot.robot_di = 270.0f;
		b_animation = true;
		b_keyboard = 4;
		break;
	case'S': case's':
		if (!b_animation)
			glutTimerFunc(10, Timer_obj, 0);
		robot.robot_di = 360.0f;
		b_animation = true;
		b_keyboard = 3;
		break;
	case'D': case'd':
		if (!b_animation)
			glutTimerFunc(10, Timer_obj, 0);
		robot.robot_di = 90.0f;
		b_animation = true;
		b_keyboard = 5;
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
	case'W': case'w':
		/*robot.Y_spin(90.0f, false);
		std::cout << "각도: " << robot.rotate.y << '\n';*/
		b_animation = false;
	case'A': case'a':
		b_animation = false;
	case'S': case's':
		b_animation = false;
	case'D': case'd':
		b_animation = false;
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
	void draw_robot();
	void draw_box();

	glClearColor(0.785f, 0.785f, 0.785f, 1.0f);//버퍼 초기화, 배경색
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//버퍼 초기화, 배경색			
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);//--- 렌더링 파이프라인에 세이더 불러오기

	//glViewport(50, 50, 400, 400);
	{
		Color_option(true);
		setColor({ 1.0f,0.0f,0.0f });
	}
	depth_choice();//은면제거
	projectTransform_choice();//원근투영,직각투영
	viewTransform();//뷰변환
	draw_coordinate_axis();//좌표축 그리기

	draw_robot();// 로봇 출력
	draw_box(); //상자 출력
	glutSwapBuffers();//--- 화면에 출력하기		
}

void viewTransform()
{
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
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

void draw_coordinate_axis() //xyz축 
{
	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::scale(transformMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
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
void draw_obj(const int& stage = 0) {
	Mesh& m = mcube;
	glBindVertexArray(m.vao);								//--- 사용할 VAO 불러오기
	Color_option(false);
	if (stage == 1) {
		if (t_or_l) {//면으로 출력
			for (int j = 0; j < 6; j++) { // indexnum = 12; , 0면이 정면
				if (j == 0) {
					setColor(color.at(j));
					{	//카메라 기준 왼쪽 문
						glm::mat4 transformMatrix(1.0f);
						transformMatrix = glm::translate(transformMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
						transformMatrix = glm::rotate(transformMatrix, glm::radians(boxopen), y_axis);
						transformMatrix = glm::scale(transformMatrix, glm::vec3{ 15.0f,30.0f,30.0f });
						transformMatrix = glm::translate(transformMatrix, glm::vec3(-1.0f, 0.0f, -1.0f));
						unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
						glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기

						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
					}
					{	//카메라 기준 오른쪽 문
						glm::mat4 transformMatrix(1.0f);
						transformMatrix = glm::translate(transformMatrix, glm::vec3(-30.0f, 30.0f, 30.0f));
						transformMatrix = glm::rotate(transformMatrix, glm::radians(-boxopen), y_axis);
						transformMatrix = glm::scale(transformMatrix, glm::vec3{ 15.0f,30.0f,30.0f });
						transformMatrix = glm::translate(transformMatrix, glm::vec3(1.0f, 0.0f, -1.0f));

						unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
						glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
					}
					glm::mat4 transformMatrix(1.0f);
					box_srt(transformMatrix);
					unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
					glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
					continue;
				}
				setColor(color.at(j));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
			}
		}
		else { //선으로 출력
			for (int j = 0; j < m.indexnum / 2; j++) {
				glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));	//큐브 1면 출력
			}
		}

	}
	else {
		if (t_or_l) {//면으로 출력
			for (int j = 0; j < 6; j++) { // indexnum = 12;
				setColor(color.at(j));
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
			}
		}
		else { //선으로 출력
			for (int j = 0; j < m.indexnum / 2; j++) {
				glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));	//큐브 1면 출력
			}
		}

	}

}

void drawGLU() {
	GLUquadricObj* qobj;
	qobj = gluNewQuadric(); // 객체 생성하기
	if (obj_style == 2) {
		gluQuadricDrawStyle(qobj, GLU_LINE);// 도형 스타일
	}
	else {
		gluQuadricDrawStyle(qobj, GLU_FILL);// 도형 스타일
	}
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	gluSphere(qobj, 1.0f, 50, 50); // 객체 만들기
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


void Robot::render_head(const glm::vec3  axis[3])
{
	{
		Object& o = head;
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, scale);


		transformMatrix = glm::translate(transformMatrix, o.translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, o.scale);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
		for (int j = 0; j < mcube.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
}
void Robot::render_nose(const glm::vec3  axis[3])
{

	Object& o = nose;
	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::translate(transformMatrix, translate);
	for (int i = 0; i < 3; i++) {
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
	}
	transformMatrix = glm::scale(transformMatrix, scale);


	transformMatrix = glm::translate(transformMatrix, o.translate);
	for (int i = 0; i < 3; i++) {
		transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
	}
	transformMatrix = glm::scale(transformMatrix, o.scale);

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
	
	drawGLU();

}
void Robot::render_body(const glm::vec3  axis[3])
{
	{
		Object& o = body;
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, scale);


		transformMatrix = glm::translate(transformMatrix, o.translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, o.scale);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
		for (int j = 0; j < mcube.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
}
void Robot::render_arms(const glm::vec3  axis[3])
{
	{
		Object& o = left_arm;
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, scale);


		transformMatrix = glm::translate(transformMatrix, o.translate);
		for (int i = 0; i < 3; i++) {
			glm::vec3 sh = { 0.0f, 1.0f, 0.0f };
			transformMatrix = glm::translate(transformMatrix, sh);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
			transformMatrix = glm::translate(transformMatrix, glm::vec3{ -1.0 } * sh);
		}
		transformMatrix = glm::scale(transformMatrix, o.scale);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
		for (int j = 0; j < mcube.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
	{
		Object& o = right_arm;
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, scale);


		transformMatrix = glm::translate(transformMatrix, o.translate);
		for (int i = 0; i < 3; i++) {
			glm::vec3 sh = { 0.0f, 1.0f,0.0f };
			transformMatrix = glm::translate(transformMatrix, sh);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
			transformMatrix = glm::translate(transformMatrix, glm::vec3{ -1.0 } *sh);
		}
		transformMatrix = glm::scale(transformMatrix, o.scale);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
		for (int j = 0; j < mcube.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
}
void Robot::render_legs(const glm::vec3  axis[3])
{
	{
		Object& o = left_leg;
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, scale);


		transformMatrix = glm::translate(transformMatrix, o.translate);
		for (int i = 0; i < 3; i++) {
			glm::vec3 sh = { 0.0f, 1.5f,0.0f };
			transformMatrix = glm::translate(transformMatrix, sh);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
			transformMatrix = glm::translate(transformMatrix, glm::vec3{ -1.0 } *sh);
		}
		transformMatrix = glm::scale(transformMatrix, o.scale);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
		for (int j = 0; j < mcube.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
	{
		Object& o = right_leg;
		glm::mat4 transformMatrix(1.0f);

		transformMatrix = glm::translate(transformMatrix, translate);
		for (int i = 0; i < 3; i++) {
			transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate[i]), axis[i]);
		}
		transformMatrix = glm::scale(transformMatrix, scale);


		transformMatrix = glm::translate(transformMatrix, o.translate);
		for (int i = 0; i < 3; i++) {
			glm::vec3 sh = { 0.0f, 1.5f,0.0f };
			transformMatrix = glm::translate(transformMatrix, sh);
			transformMatrix = glm::rotate(transformMatrix, glm::radians(o.rotate[i]), axis[i]);
			transformMatrix = glm::translate(transformMatrix, glm::vec3{ -1.0 } *sh);
		}
		transformMatrix = glm::scale(transformMatrix, o.scale);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
		for (int j = 0; j < mcube.indexnum / 2; j++) { // indexnum = 12;
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(j * 6 * sizeof(unsigned int)));
		}
	}
}
void Robot::render() {
	glBindVertexArray(mcube.vao);//--- 사용할 VAO 불러오기
	const glm::vec3 axis[] = { x_axis, y_axis, z_axis };
	Color_option(false);
	int cnt{};
	setColor(color.at(cnt++));
	render_head(axis);
	setColor(color.at(cnt++));
	render_nose(axis);
	setColor(color.at(cnt++));
	render_body(axis);
	setColor(color.at(cnt++));
	render_arms(axis);
	setColor(color.at(cnt++));
	render_legs(axis);
}
void draw_robot()
{
	void drawGLU();	//사용하기전에 선언.
	robot.render();

}
void draw_box() {
	glm::mat4 transformMatrix(1.0f);

	box_srt(transformMatrix);

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform 변수에 변환 값 적용하기
	draw_obj(1);

}

void box_srt(glm::mat4& transformMatrix)
{
	transformMatrix = glm::translate(transformMatrix, glm::vec3(0.0f, 30.0f, 0.0f));
	transformMatrix = glm::scale(transformMatrix, glm::vec3{ 30.0f });
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

//void InitBuffer_floor(Floor_Mesh& s) {
//	//버퍼 생성
//	std::vector<float> s_vertex;
//	std::vector<float> l_vertex;
//	std::vector<float> vertex;
//
//	std::vector<float> color;
//
//
//	std::vector<unsigned int> index;
//	float degree{};
//	int count{};
//	while (degree < 360) {
//		vertex.push_back(cos(glm::radians(degree)));
//		vertex.push_back(0.0f);
//		vertex.push_back(sin(glm::radians(degree)));
//		index.push_back(count);
//		//검은색
//		color.push_back(randomnum(0.0f,1.0f));	//r
//		color.push_back(randomnum(0.0f, 1.0f));	//g
//		color.push_back(randomnum(0.0f, 1.0f));	//b
//		degree += 90.0f;
//		count++;
//	}
//
//	index.push_back(0);		
//	index.push_back(2);		
//	index.push_back(1);		
//	index.push_back(1);
//	index.push_back(3);
//	index.push_back(2);		
//
//
//	s.indexnum = index.size();
//
//	//gpu 버퍼에 저장하기
//	{
//		glGenVertexArrays(1, &s.vao); //--- VAO 를 지정하고 할당하기
//		glBindVertexArray(s.vao);		//--- VAO를 바인드하기
//		glGenBuffers(2, s.vbo);		//--- 2개의 VBO를 지정하고 할당하기
//
//		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
//		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[0]);											//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
//		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//--- triShape 배열의 사이즈: 9 * float
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
//		glEnableVertexAttribArray(0);													//--- attribute 인덱스 0번을 사용가능하게 함
//
//		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
//		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[1]);  										//--- 변수 colors에서 버텍스 색상을 복사한다.
//		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW); 	//--- colors 배열의 사이즈: 9 *float
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);							//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
//		glEnableVertexAttribArray(1);
//
//		glGenBuffers(1, &s.ebo);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), index.data(), GL_STATIC_DRAW);
//
//	}
//	vertex.clear();
//	color.clear();
//	index.clear();
//}

void reset() {
	// color 넣기

	int cnt{};
	while (cnt < 6) {
		color.push_back({ randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) });
		cnt++;
	}

	// 축 초기화
	InitBuffer_Line_xyz(line);
	//InitBuffer_floor(floor_b);

	// 오브젝트 초기화
	robot.robot_reset();


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

