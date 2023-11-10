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
const int WIN_W = 900, WIN_H = 900;

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
	glutTimerFunc(30, basket_timer, 0);

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

class Object{
public:
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
};

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
shape rect[15];
shape basket;
int choice(0);		//�׸� ���� ���
bool start = false; // Ÿ�̸� ���� ����
int n = 0;			// ����� ���� ����
GLuint vao, vbo[2];
int target{ 0 };  //������ ����
bool left_button = false;

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

	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		for (int i = 0; i < n; ++i) {

			if (rect[i].pointnum == 0) {
				continue;
			}
			if (rect[i].x - rect[i].size < mx and mx < rect[i].x + rect[i].size and	 //�浹����
				rect[i].y - rect[i].size < my and my < rect[i].y + rect[i].size) {
				left_button = true;
				target = i;
			}
		}
		std::cout << "target�� : " << target << "\n";
		//
		std::cout << "���콺 Ŭ�� Ÿ�̸� ���۵� ���� ��ȣ[:" << target << "]" << '\n';
		//InitBuffer(rect[target]);
		//glutTimerFunc(10, TimerA, target);

	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		for (int i = 0; i < 15; ++i) {
			if (i == target || rect[i].pointnum == 0) {
				continue;
			}
			if (IsCollision(rect[i], rect[target])) {
				rect[target].pointnum += rect[i].pointnum;
				if (rect[target].pointnum > 6) {
					rect[target].pointnum = 1;
				}
				rect[i].pointnum = 0;//���� ���� ����
				InitBuffer(rect[i]);
				InitBuffer(rect[target]);

				glutTimerFunc(10, TimerB, target);
				break;
			}
		}
		target = -1; // target �ʱ�ȭ
		left_button = false;
	}

	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

void Motion(int x, int y) {
	float mx = (x - 800 / 2.0f) / (800 / 2.0f);
	float my = (800 / 2.0f - y) / (800 / 2.0f);

	if (left_button == true) {

		rect[target].x = mx;
		rect[target].y = my;

		std::cout << "rect[:" << target << "].x =" << rect[target].x << '\n';
		std::cout << "rect[:" << target << "].y =" << rect[target].y << '\n' << '\n';

	}

	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}


int b_keyboard{};
void TimerA(int value) {
	const int y{ 16 };
	const int mouse{ 1 };

	if (rect[value].time > 1.0) {
	
		rect[value].time = 0.0;
	}
	else {
		rect[value].time += 0.01f;
	}
	InitBuffer(rect[value]);

	glutPostRedisplay();
	if (rect[value].pointnum < 6)
		glutTimerFunc(10, TimerA, value);
}
void TimerB(int value) {

	if (rect[value].motions == 1) {
		if (value != target) {
			rect[value].x += rect[value].dx * 0.01f;
			rect[value].y += rect[value].dy * 0.01f;
			//std::cout << "timer[" << value << "]: (" << rect[value].dx << ", " << rect[value].x << ")" << std::endl;

			if ((rect[value].dx < 0 and rect[value].x - rect[value].size <= -1) || (rect[value].dx > 0 and rect[value].x + rect[value].size >= 1)) {
				rect[value].dx *= -1;
				rect[value].x += rect[value].dx * 0.01f;
			}
			if ((rect[value].dy < 0 and rect[value].y - rect[value].size <= -1) || (rect[value].dy > 0 and rect[value].y + rect[value].size >= 1)) {
				rect[value].dy *= -1;
				rect[value].y += rect[value].dy * 0.01f;
			}
			//std::cout << rect[i].x << '\n';
			InitBuffer(rect[value]);
		}
	}
	else {
		if (rect[value].down == 1) {			// ���� �̵�
			rect[value].y += rect[value].dy * 0.01f; //���� ��ȯ
			rect[value].chy += 0.01f;

			if (rect[value].chy > rect[value].size * 2) {

				rect[value].down = 0;
			}

			else if (rect[value].chy > rect[value].size * 2 || (rect[value].dy < 0 && rect[value].y - rect[value].size <= -1) || (rect[value].dy > 0 && rect[value].y + rect[value].size >= 1)) {
				rect[value].dy *= -1;
			}
		}
		else { // ���� �̵�
			rect[value].x += rect[value].dx * 0.01f;

			if ((rect[value].dx < 0 && rect[value].x - rect[value].size <= -1) || (rect[value].dx > 0 && rect[value].x + rect[value].size >= 1)) {
				rect[value].dx *= -1;
				rect[value].chy = 0.0f;
				rect[value].down = 1;
			}
		}


	}
	glutPostRedisplay();
	glutTimerFunc(10, TimerB, value);
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
	//��
	case 'l': case 'L':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 2;
			InitBuffer(rect[i]);
		}
		break;
	//�ﰢ��
	case 't':case 'T':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 3;
			InitBuffer(rect[i]);
		}
		break;
	// �簢��
	case 'r':case 'R':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 4;
			InitBuffer(rect[i]);
		}
		break;
	//������
	case 'p':case 'P':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = 5;
			InitBuffer(rect[i]);
		}
		break;
	// �ٽ�
	case 'a':case 'A':
		for (int i = 0; i < 4; ++i) {
			rect[i].pointnum = i+ 2;
			InitBuffer(rect[i]);
		}
		break;

	
	
		break;
	}
	glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}

void reset() {
	n = 0;
	basket.x = 0.0f;
	basket.y = -0.5f;
	basket.dx = randomnum(0.0f, 1.0f) < 0.5f ? 1 : -1;
	basket.down = 0;   // y�� ���ϴ°�
	basket.size = 0.2f;
	basket.pointnum = 2;
	basket.color.push_back(0.0f);		//r
	basket.color.push_back(0.0f);		//g
	basket.color.push_back(1.0f);		//b
	//InitBuffer(basket);
	for (shape& r : rect) {

		r.x = randomnum(-1.0f, 1.0f);
		r.y = randomnum(-1.0f, 1.0f);
		r.dx = randomnum(0.0f, 1.0f) < 0.5f ? 1 : -1;
		r.dy = randomnum(0.0f, 1.0f) < 0.5f ? 1 : -1;

		r.motions = randomnum(0.0f, 1.0f) < 0.5f ? 1 : 2;
		r.chy = 1.0f; // ��ȭ�� ���
		r.down = 0;   // y�� ���ϴ°�

		r.time = 0.0f;
		r.pointnum = n++ / 3 + 1;
		r.size = 0.2f;

		for (int j = 0; j < 7; j++) {
			r.color.push_back(randomnum(0.0f, 1.0f));		//r
			r.color.push_back(randomnum(0.0f, 1.0f));		//g
			r.color.push_back(randomnum(0.0f, 1.0f));		//b
		}
		InitBuffer(r);
	}

}

// ---- �׸��� �ݹ� �Լ�
GLvoid drawScene()
{
	glClearColor(0.313f, 0.737f, 0.874f, 1.0f);			//--- ����� ���� ���� //�ϴû� 0.313, 0.737 , 0.874
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glUseProgram(shaderProgramID);						//--- ������ ���������ο� ���̴� �ҷ�����

	glBindVertexArray(basket.vao);								//--- ����� VAO �ҷ�����
	{
		glm::mat4 transformMatrix(1.0f);
		transformMatrix = glm::translate(transformMatrix, glm::vec3(basket.x, basket.y, 0.0f));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

	}
	glLineWidth(5);
	glDrawElements(GL_LINES, basket.indexnum, GL_UNSIGNED_INT, 0);


	dddddddddddddddddddddd();

	glutSwapBuffers();									//--- ȭ�鿡 ����ϱ�
}

void dddddddddddddddddddddd()
{
	for (int i = 0; i < n; ++i) {

		glBindVertexArray(rect[i].vao);								//--- ����� VAO �ҷ�����
		{
			glm::mat4 transformMatrix(1.0f);
			transformMatrix = glm::translate(transformMatrix, glm::vec3(rect[i].x, rect[i].y, 0.0f));
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");	//--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));		//--- modelTransform ������ ��ȯ �� �����ϱ�

		}


		if (choice == 0) {
			if (rect[i].pointnum == 1) {
				glPointSize(5);
				glDrawElements(GL_POINTS, rect[i].indexnum, GL_UNSIGNED_INT, 0);
			}
			else if (rect[i].pointnum == 2) {
				glLineWidth(5);
				glDrawElements(GL_LINES, rect[i].indexnum, GL_UNSIGNED_INT, 0);

			}
			else
				glDrawElements(GL_TRIANGLES, rect[i].indexnum, GL_UNSIGNED_INT, 0);
		}
		else {
			glLineWidth(5);
			glDrawArrays(GL_LINE_LOOP, 0, rect[i].pointnum + 1);
		}
	}
}


//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

