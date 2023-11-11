#include "DEFAULT.hpp"
//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGSlib 

const std::string Guide[]{
	"--------------------------------------------------------------------------------------------------",
	"도형의 모드(L) : LINE/FILL",
	"경로 출력하기(O) : on/off",
	"날라오는 속도 변경하기(+,-) : 빨라지기 느려지기 ",
	"C/c: 초기화",
	"q : 프로그램 종료",
	"--------------------------------------------------------------------------------------------------"
};
const int WIN_X = 400, WIN_Y = 0;
const int WIN_W = 800, WIN_H = 800;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WIN_X, WIN_Y); // 윈도우의 위치 지정
	glutInitWindowSize(WIN_W, WIN_H); // 윈도우의 크기 지정
	glutCreateWindow("Slice_Polygon");
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
	glutTimerFunc(30, timer, 0);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); 
	glutMouseFunc(Mouse);	
	glutMotionFunc(Motion); 

	glutMainLoop();
}

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

class Mesh {
public:
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	bool vao_ox;

	std::vector<glm::vec3> vertex;
	std::vector<glm::vec3> color;
	std::vector<unsigned int> index;

	Mesh() {
		cout << "Mesh 생성자 호출";
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao); 
		glGenBuffers(2, vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(0); //초기화
		vao_ox = true;

	}


	~Mesh() {
		if (vao_ox) {
			glDeleteBuffers(1, &ebo);
			glDeleteBuffers(2, vbo);
			glDeleteVertexArrays(1, &vao);
		}
	}

	// 복사 생성자 ( Mesh b = a ) ( copy construction)
	Mesh(const Mesh& other) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(2, vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(0); //초기화

		for (const glm::vec3& v : other.vertex) {
			vertex.push_back(v);
		}
		for (const glm::vec3& v : other.color) {
			color.push_back(v);
		}
		for (const GLuint& ui : other.index) {
			index.push_back(ui);
		}	
		send_gpu();
		vao_ox = true;
	}

	// 복사 할당(대입) 연산자(= copy assignment operator)
	Mesh& operator=(const Mesh& other) {
		if (this == &other) {
			// a = a = a = a = a;
			return *this;
		}
		// 1. 기본 값 제거
		// 2. 새로운 값으로 할당
		for (const glm::vec3& v : other.vertex) {
			vertex.push_back(v);
		}
		for (const glm::vec3& v : other.color) {
			color.push_back(v);
		}
		for (const GLuint& ui : other.index) {
			index.push_back(ui);
		}
		send_gpu();
		return *this;
	}

	// 이동 생산자 ( move construction)
	Mesh(Mesh&& other) noexcept {
		// 기존 변수 초기화
		vertex.clear();
		color.clear();
		index.clear();

		// 이동 시작
		vao = other.vao;
		vbo[0] = other.vbo[0];
		vbo[1] = other.vbo[1];
		vao = other.vao;


		vertex = std::move(other.vertex);
		color = std::move(other.color);
		index = std::move(other.index);
		vao_ox = true;
		// other의 내용을 초기화 해야함(ex : 포인터는 nullptr, 밸류값은 0으로 같은거)
		other.vao_ox = false;
	}

	// 이동 할당(대입) 연산자 (= move assignment operator)
	Mesh& operator=(Mesh&& other) noexcept {
		if (this == &other) {
			// a = a = a = a = a;
			return *this;
		}

		vao = other.vao;		
		vbo[0] = other.vbo[0];
		vbo[1] = other.vbo[1];
		vao = other.vao;


		vertex = std::move(other.vertex);
		color = std::move(other.color);
		index = std::move(other.index);

		other.vao_ox = false;
		return *this;
	}

	void send_gpu();

	void set_polygon(const int& vertex_num = 3);

	void draw();
};

void Mesh::send_gpu() {
	//gpu 버퍼에 저장하기
	glBindVertexArray(vao);
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);	//배열의 사이즈
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glEnableVertexAttribArray(0);//--- attribute 인덱스 0번을 사용가능하게 함
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); //--- 변수 colors에서 버텍스 색상을 복사한다.
	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);//--- colors 배열의 사이즈: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), index.data(), GL_STATIC_DRAW);

}

void Mesh::set_polygon(const int& vertex_num) {
	vertex.clear();
	color.clear();
	index.clear();

	float s_rad = 360.0 / vertex_num;

	for (int i = 0; i < vertex_num; ++i)
	{
		vertex.push_back({ cos(glm::radians(i * s_rad)), sin(glm::radians(i * s_rad)), 0.0f });
		color.push_back({ randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f), randomnum(0.0f, 1.0f) });		
		
		if (i >= 2) {
			index.push_back(0);
			index.push_back(i - 1);
			index.push_back(i);
		}
	}	
}

void Mesh::draw()
{
	for (int i = 0; i < vertex.size(); ++i) {
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));
	}
}

class Object {
public:
	glm::vec3 translate{ 0.0f};
	glm::vec3 rotate{ 0.0f };
	glm::vec3 scale{ 1.0f};
	glm::vec3 after_translate{ 0.0f };
	glm::vec3 after_rotate{ 0.0f };
	glm::vec3 after_scale{ 1.0f};
	float y_speed{ 1.0f };
	bool start_x{ false }; // 양수 = true

	Object() {
	}

	void reset() {
		translate = { 0.0f,0.0f,0.0f };
		rotate = { 0.0f,0.0f,0.0f };
		scale = { 1.0f,1.0f,1.0f };
		after_translate = { 0.0f,0.0f,0.0f };
		after_rotate = { 0.0f,0.0f,0.0f };
		after_scale = { 1.0f,1.0f,1.0f };
	}

	Mesh mesh;

	void set_world_transform(glm::mat4& transformMatrix) const;
	void set_location(const float& x, const float& y, const float& z) {
		translate.x = x;
		translate.y = y;
		translate.z = z;
	}
	void translate_after(glm::mat4& transformMatrix) const {
		transformMatrix = glm::translate(transformMatrix, after_translate);
	}
	void rotate_after(glm::mat4& transformMatrix) const {
		transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.z), z_axis);
	}
	void scale_after(glm::mat4& transformMatrix) const {
		transformMatrix = glm::scale(transformMatrix, after_scale);
	}

	void translate_matrix(glm::mat4& transformMatrix) const {
		transformMatrix = glm::translate(transformMatrix, translate);
	}
	void rotate_matrix(glm::mat4& transformMatrix) const {
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.z), z_axis);
	}
	void scale_matrix(glm::mat4& transformMatrix) const {
		transformMatrix = glm::scale(transformMatrix, scale);
	}

	void set_scale(const float& x, const float& y, const float& z);
	void go_trans(const float& x, const float& y, const float& z);// 이동
	void Z_spin(const float&, const bool& degreeb);// z축으로 자전시키는 함수

};

void Object::set_scale(const float& x, const float& y, const float& z) {
	after_scale.x = x;
	after_scale.y = y;
	after_scale.z = z;
}

void Object::go_trans(const float& x, const float& y, const float& z) {
	after_translate.x += x;
	after_translate.y += y;
	after_translate.z += z;
}

void Object::Z_spin(const float& degree, const bool& degreeb) {
	//각도 범위 유지
	after_rotate.y += degree;
	while (after_rotate.y > 360.0f) after_rotate.y -= 360.0f;
	while (after_rotate.y < 0.0f) after_rotate.y += 360.0f;
}

void Object::set_world_transform(glm::mat4& transformMatrix) const {
	translate_after(transformMatrix);
	rotate_after(transformMatrix);
	scale_after(transformMatrix);
	translate_matrix(transformMatrix);
	rotate_matrix(transformMatrix);
	scale_matrix(transformMatrix);
}

typedef struct allshape {
	GLfloat x, y, size;			// 중심 좌표, 마우스 클릭
	float time, dx, dy;
	int pointnum;
	int indexnum;				 //index.size()
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	bool canuse;				// 도형이 사용가능 여부 (지워졌나)
	std::vector<float> color;
	GLint motions, down;

	float chy;
}shape;

//--- 전역변수 ----
shape basket;
int target{ 0 };  //선택한 도형
bool left_button = false;

//Object* slice_polygon = new Object[5]; //한번에 존재 하는 폴리곤 개수
std::vector<Object> slice_polygon;

//class VECTOR {
//	size_t value_size{ 0 };
//	Object* p{ nullptr };
//public:
//	VECTOR() {}
//	size_t size() {
//		return value_size;
//	}
//
//	void push_back(Object& other) {
//		Object* new_p = new Object[value_size + 1];
//		if (p != nullptr) {
//			for (int i = 0; i < value_size; i++) {
//				new_p[i] = std::move(p[i]);	//복사 할당 연산자
//			}
//			delete[] p;
//		}
//		p = new_p;
//		p[value_size] = other;
//		++value_size;
//	}
//};

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

	for (int i = 0; i < s.pointnum + 1; ++i)
	{
		if (s.pointnum == 1) {
			vertex.push_back(0.0f);		//x - cos
			vertex.push_back(0.0f);		//y - sin
			vertex.push_back(0.0f);		//z
		}
		else {
			vertex.push_back(cos(glm::radians(i * s_rad)) * s.size);		//x - cos
			vertex.push_back(sin(glm::radians(i * s_rad)) * s.size);		//y - sin
			vertex.push_back(0.0f);											//z
		}
		

		color.push_back(s.color.at(i * 3));		//r
		color.push_back(s.color.at(i * 3 + 1));	//g
		color.push_back(s.color.at(i * 3 + 2));	//b

		
		//{
		//	std::cout << std::format("현재 도형 [{}]: vertex - ({},{},{}), time : {}\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2), s.time);
		//}	
	}

	if (s.pointnum + 1 > 0)
		index.push_back(0);		//점이 1개일떄
	if (s.pointnum + 1 > 1)
		index.push_back(1);		//점이 2개일떄
	if (s.pointnum + 1 > 2)
		index.push_back(2);		//점이 3개일떄
	if (s.pointnum + 1 > 3) {
		index.push_back(0);
		index.push_back(2);
		index.push_back(3);		//점이 4개일떄
	}
	if (s.pointnum + 1 > 4) {
		index.push_back(0);
		index.push_back(3);
		index.push_back(4);		//점이 5개일떄
	}
	if (s.pointnum + 1 > 5) {
		index.push_back(0);
		index.push_back(4);
		index.push_back(5);		//점이 8개일떄
	}
	s.indexnum = index.size();

	//gpu 버퍼에 저장하기
	{
		glGenVertexArrays(1, &s.vao); 
		glBindVertexArray(s.vao);		
		glGenBuffers(2, s.vbo);		

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[0]);											
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//배열의 사이즈
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glEnableVertexAttribArray(0);//--- attribute 인덱스 0번을 사용가능하게 함

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[1]); //--- 변수 colors에서 버텍스 색상을 복사한다.
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW);//--- colors 배열의 사이즈: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
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

bool IsCollision(const shape& rect1, const shape& rect2) {

	// 두 사각형의 축 정의
	float rect1_left = rect1.x - rect1.size;
	float rect1_right = rect1.x + rect1.size;
	float rect1_top = rect1.y + rect1.size;
	float rect1_bottom = rect1.y - rect1.size;

	float rect2_left = rect2.x - rect2.size;
	float rect2_right = rect2.x + rect2.size;
	float rect2_top = rect2.y + rect2.size;
	float rect2_bottom = rect2.y - rect2.size;

	// 충돌 감지
	if (rect1_left < rect2_right &&
		rect1_right > rect2_left &&
		rect1_top > rect2_bottom &&
		rect1_bottom < rect2_top) {
		return true; // 충돌 발생
	}

	return false; // 충돌 없음

}

void Mouse(int button, int state, int x, int y) {

	float mx = (x - WIN_W / 2.0f) / (WIN_W / 2.0f);
	float my = (WIN_H / 2.0f - y) / (WIN_H / 2.0f);


	glutPostRedisplay(); // 화면 다시 그리기 요청
}

void Motion(int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (left_button == true) {
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

int b_keyboard{};

void timer(int value) {
	static int count{};//폴리곤 나오는 시간
	count = ++count % 200; //2초

	if (!count) {
		Object new_p;
		new_p.mesh.set_polygon(randomnum(3, 8));//메쉬초기화 
		new_p.mesh.send_gpu();
		new_p.translate = {(randomnum(-1, 1) > 0) ? -1 : 1, 0.0f, 1.0f};
		if (new_p.translate.x > 0) {
			new_p.start_x = true;
		}
		else {
			new_p.start_x = false;
		}
		new_p.scale = {0.2f,0.2f,0.2f};
		new_p.y_speed = 0.05f;
		slice_polygon.push_back(move(new_p));
	}

	for (Object& o : slice_polygon) { //래퍼런스는 메모리를 할당 x
		o.translate.y += o.y_speed;
		o.y_speed -= 0.005f;
		o.y_speed = glm::clamp(o.y_speed, -0.5f, 0.5f);
		if (o.start_x) {
			o.translate.x -= 0.05f;
		}
		else {
			o.translate.x += 0.05f;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, timer, value);
}

void basket_timer(int value) {
	basket.x += basket.dx * 0.01f;
	//std::cout << "timer[" << value << "]: (" << rect[value].dx << ", " << rect[value].x << ")" << std::endl;

	if ((basket.dx < 0 and basket.x - basket.size <= -1) || (basket.dx > 0 and basket.x + basket.size >= 1)) {
		basket.dx *= -1;
		basket.x += basket.dx * 0.01f;
	}
	glutPostRedisplay();
	glutTimerFunc(10, basket_timer, value);
}
GLvoid Keyboard(unsigned char key, int x, int y) {

	switch (key) {

	case 'q':case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay(); // 화면 다시 그리기 요청
}

void reset() {

}

// ---- 그리기 콜백 함수
GLvoid drawScene()
{
	glClearColor(0.313f, 0.737f, 0.874f, 1.0f);			//--- 변경된 배경색 설정 //하늘색 0.313, 0.737 , 0.874
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- 렌더링 파이프라인에 세이더 불러오기

	for(Object& o : slice_polygon){

		// 1. object 의 월드 변환을  shader에 입력(uniform 변수)
		glm::mat4 transformMatrix(1.0f);
		o.set_world_transform(transformMatrix);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- 버텍스 세이더에서 모델링 변환 위치 가져오기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		// 2. object 가 가진 모양(Mesh)에게 출력 요청
		o.mesh.draw();
	}

	glutSwapBuffers();									//--- 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

