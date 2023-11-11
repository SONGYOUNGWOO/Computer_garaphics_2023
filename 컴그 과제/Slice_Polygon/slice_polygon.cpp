#include "DEFAULT.hpp"
//glew32.lib freeglut.lib 
#define _CRT_SECURE_NO_WARNINGSlib 

const std::string Guide[]{
	"--------------------------------------------------------------------------------------------------",
	"������ ���(L) : LINE/FILL",
	"��� ����ϱ�(O) : on/off",
	"������� �ӵ� �����ϱ�(+,-) : �������� �������� ",
	"C/c: �ʱ�ȭ",
	"q : ���α׷� ����",
	"--------------------------------------------------------------------------------------------------"
};
const int WIN_X = 400, WIN_Y = 0;
const int WIN_W = 800, WIN_H = 800;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WIN_X, WIN_Y); // �������� ��ġ ����
	glutInitWindowSize(WIN_W, WIN_H); // �������� ũ�� ����
	glutCreateWindow("Slice_Polygon");
	//--- GLEW �ʱ�ȭ�ϱ�e
	glewExperimental = GL_TRUE;
	glewInit();

	std::cout << " GLEW �ʱ�ȭ �Ϸ�" << '\n';

	make_shaderProgram();

	reset();
	std::cout << " reset �ʱ�ȭ �Ϸ�" << '\n';

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
		cout << "Mesh ������ ȣ��";
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao); 
		glGenBuffers(2, vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(0); //�ʱ�ȭ
		vao_ox = true;

	}


	~Mesh() {
		if (vao_ox) {
			glDeleteBuffers(1, &ebo);
			glDeleteBuffers(2, vbo);
			glDeleteVertexArrays(1, &vao);
		}
	}

	// ���� ������ ( Mesh b = a ) ( copy construction)
	Mesh(const Mesh& other) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(2, vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(0); //�ʱ�ȭ

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

	// ���� �Ҵ�(����) ������(= copy assignment operator)
	Mesh& operator=(const Mesh& other) {
		if (this == &other) {
			// a = a = a = a = a;
			return *this;
		}
		// 1. �⺻ �� ����
		// 2. ���ο� ������ �Ҵ�
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

	// �̵� ������ ( move construction)
	Mesh(Mesh&& other) noexcept {
		// ���� ���� �ʱ�ȭ
		vertex.clear();
		color.clear();
		index.clear();

		// �̵� ����
		vao = other.vao;
		vbo[0] = other.vbo[0];
		vbo[1] = other.vbo[1];
		vao = other.vao;


		vertex = std::move(other.vertex);
		color = std::move(other.color);
		index = std::move(other.index);
		vao_ox = true;
		// other�� ������ �ʱ�ȭ �ؾ���(ex : �����ʹ� nullptr, ������� 0���� ������)
		other.vao_ox = false;
	}

	// �̵� �Ҵ�(����) ������ (= move assignment operator)
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
	//gpu ���ۿ� �����ϱ�
	glBindVertexArray(vao);
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);	//�迭�� ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glEnableVertexAttribArray(0);//--- attribute �ε��� 0���� ��밡���ϰ� ��
	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);//--- colors �迭�� ������: 9 *float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
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
	bool start_x{ false }; // ��� = true

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
	void go_trans(const float& x, const float& y, const float& z);// �̵�
	void Z_spin(const float&, const bool& degreeb);// z������ ������Ű�� �Լ�

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
	//���� ���� ����
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
	GLfloat x, y, size;			// �߽� ��ǥ, ���콺 Ŭ��
	float time, dx, dy;
	int pointnum;
	int indexnum;				 //index.size()
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;
	bool canuse;				// ������ ��밡�� ���� (��������)
	std::vector<float> color;
	GLint motions, down;

	float chy;
}shape;

//--- �������� ----
shape basket;
int target{ 0 };  //������ ����
bool left_button = false;

//Object* slice_polygon = new Object[5]; //�ѹ��� ���� �ϴ� ������ ����
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
//				new_p[i] = std::move(p[i]);	//���� �Ҵ� ������
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

GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID; //--- ���̴� ���α׷�

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");					//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);			//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);								//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");					//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);			//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);		//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);								//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
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

	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
	//return shaderProgramID;

}

void InitBuffer(shape& s) {
	//���� ����
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
		//	std::cout << std::format("���� ���� [{}]: vertex - ({},{},{}), time : {}\n", i, vertex.at(i * 3), vertex.at(i * 3 + 1), vertex.at(i * 3 + 2), s.time);
		//}	
	}

	if (s.pointnum + 1 > 0)
		index.push_back(0);		//���� 1���ϋ�
	if (s.pointnum + 1 > 1)
		index.push_back(1);		//���� 2���ϋ�
	if (s.pointnum + 1 > 2)
		index.push_back(2);		//���� 3���ϋ�
	if (s.pointnum + 1 > 3) {
		index.push_back(0);
		index.push_back(2);
		index.push_back(3);		//���� 4���ϋ�
	}
	if (s.pointnum + 1 > 4) {
		index.push_back(0);
		index.push_back(3);
		index.push_back(4);		//���� 5���ϋ�
	}
	if (s.pointnum + 1 > 5) {
		index.push_back(0);
		index.push_back(4);
		index.push_back(5);		//���� 8���ϋ�
	}
	s.indexnum = index.size();

	//gpu ���ۿ� �����ϱ�
	{
		glGenVertexArrays(1, &s.vao); 
		glBindVertexArray(s.vao);		
		glGenBuffers(2, s.vbo);		

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[0]);											
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);	//�迭�� ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
		glEnableVertexAttribArray(0);//--- attribute �ε��� 0���� ��밡���ϰ� ��

		//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, s.vbo[1]); //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(GLfloat), color.data(), GL_STATIC_DRAW);//--- colors �迭�� ������: 9 *float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
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

	// �� �簢���� �� ����
	float rect1_left = rect1.x - rect1.size;
	float rect1_right = rect1.x + rect1.size;
	float rect1_top = rect1.y + rect1.size;
	float rect1_bottom = rect1.y - rect1.size;

	float rect2_left = rect2.x - rect2.size;
	float rect2_right = rect2.x + rect2.size;
	float rect2_top = rect2.y + rect2.size;
	float rect2_bottom = rect2.y - rect2.size;

	// �浹 ����
	if (rect1_left < rect2_right &&
		rect1_right > rect2_left &&
		rect1_top > rect2_bottom &&
		rect1_bottom < rect2_top) {
		return true; // �浹 �߻�
	}

	return false; // �浹 ����

}

void Mouse(int button, int state, int x, int y) {

	float mx = (x - WIN_W / 2.0f) / (WIN_W / 2.0f);
	float my = (WIN_H / 2.0f - y) / (WIN_H / 2.0f);


	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

void Motion(int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (left_button == true) {
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

int b_keyboard{};

void timer(int value) {
	static int count{};//������ ������ �ð�
	count = ++count % 200; //2��

	if (!count) {
		Object new_p;
		new_p.mesh.set_polygon(randomnum(3, 8));//�޽��ʱ�ȭ 
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

	for (Object& o : slice_polygon) { //���۷����� �޸𸮸� �Ҵ� x
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
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

void reset() {

}

// ---- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	glClearColor(0.313f, 0.737f, 0.874f, 1.0f);			//--- ����� ���� ���� //�ϴû� 0.313, 0.737 , 0.874
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- ������ ���������ο� ���̴� �ҷ�����

	for(Object& o : slice_polygon){

		// 1. object �� ���� ��ȯ��  shader�� �Է�(uniform ����)
		glm::mat4 transformMatrix(1.0f);
		o.set_world_transform(transformMatrix);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		// 2. object �� ���� ���(Mesh)���� ��� ��û
		o.mesh.draw();
	}

	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

