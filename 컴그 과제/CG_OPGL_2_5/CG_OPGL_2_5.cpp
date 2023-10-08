#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <random>
#include <stdio.h>

//glew32.lib freeglut.lib 
using namespace std;

const int WIN_X = 10, WIN_Y = 10;
const int WIN_W = 800, WIN_H = 800;

// ���� ���� ���� �Լ�
float randomnum(float a, float b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(a, b);
    return dis(gen);
}
int randomint(int a, int b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(a, b);
    return dis(gen);
}

//�簢�� 5��
typedef struct Rectangle {
    GLfloat x, y; // �߽� ��ǥ
    GLfloat size; // ũ��
    GLfloat r, g, b; // ����
    //GLfloat mx, my;
    struct Rectangle* next; //���� �ڷḦ ����Ű�� ������
    struct Rectangle* prior; //���� �ڷḦ ����Ű�� ������
}Rect;

Rect rect[5];
int n = 0;

Rect* head = nullptr; //Rect* head = NULL
Rect* tail = nullptr; //Rect* tail = NULL

Rect* eraser = nullptr;

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

// ���ο� �簢�� ����Ʈ�� �߰�
void addRectangle(int x, int y) {
    Rect* new_Rectangle = new Rect;

    //���θ��� ����ü �ʱ�ȭ
    new_Rectangle->x = randomnum(-1.0, 1.0);
    new_Rectangle->y = randomnum(-1.0, 1.0);
    new_Rectangle->size = 0.05f;
    new_Rectangle->r = randomnum(0.0, 1.0);
    new_Rectangle->g = randomnum(0.0, 1.0);
    new_Rectangle->b = randomnum(0.0, 1.0);

    new_Rectangle->prior = tail;
    new_Rectangle->next = nullptr;

    //����Ʈ�� �������� ���λ��� Rect�� ����    
    if (head != nullptr) {       
        tail->next = new_Rectangle;
        tail = new_Rectangle;
    }
    else {
        head = new_Rectangle;
        tail = new_Rectangle;
    }
    n++;
}

Rect* delete_node(Rect* del) {
    Rect* tmp = del->next;
    if (head != del && tail != del) {
        del->prior->next = del->next;
        del->next->prior = del->prior;
    }
    else if (del == head) {
        head = del->next;  
        if(head != nullptr)
            head->prior = nullptr;
    }

    else if (del == tail) {
        tail = del->prior;
        tail->next = nullptr;
    }
    
    delete del;

    return tmp;
}

GLvoid drawScene() {
    glClearColor(0.745f, 0.745f, 0.745f, 1.0f); // ȸ���� �������� ����
    glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
    

    for (Rect* finder = head; finder!= nullptr; finder = finder->next) {
        cout << finder->x << endl << finder->y << endl;
        glColor3f(finder->r, finder->g, finder->b);

        glRectf(finder->x - finder->size, finder->y - finder->size,
            finder->x + finder->size, finder->y + finder->size);
    }
    if (eraser != nullptr) {
        glColor3f(eraser->r, eraser->g, eraser->b);
        glRectf(eraser->x - eraser->size, eraser->y - eraser->size,
            eraser->x + eraser->size, eraser->y + eraser->size);
    }

    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

bool left_button = false;
//���콺 Ŭ�� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        eraser = new Rect;
        eraser->x = (x - WIN_W / 2.0f) / (WIN_W / 2.0f);
        eraser->y = ((WIN_H / 2.0f) - y) / (WIN_H / 2.0f);
        eraser->r = randomnum(0.0, 1.0);
        eraser->g = randomnum(0.0, 1.0);
        eraser->b = randomnum(0.0, 1.0);
        eraser->size = 0.1f;
        eraser->prior = tail;
        eraser->next = nullptr;
        //���찳 ����
        left_button = true;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        delete eraser;
        eraser = nullptr;
        //���찳 ����
        left_button = false;
    }
    glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}


//�ּҸ� �ް� �ٸ� �̸� ������ ���۷�������
bool IsCollision(const Rect& rect1, const Rect& rect2) {

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

void Motion(int x, int y) {
    if (left_button == true) {
        eraser->x = (x - WIN_W / 2.0f) / (WIN_W / 2.0f);
        eraser->y = ((WIN_H / 2.0f) - y) / (WIN_H / 2.0f);
    }

    for (Rect* finder = head; finder != nullptr;) {
        if (IsCollision(*eraser, *finder))
        {
            eraser->r = finder->r;
            eraser->g = finder->g;
            eraser->b = finder->b;
            eraser->size += 0.01f;

            finder = delete_node(finder);

        }
        else {
            finder = finder->next;
        }
    }
   
    glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':case 'R':

        for (Rect* finder = head; finder != nullptr;) {
            Rect* tmp = finder->next;
            delete_node(finder);
            finder = tmp;
        }
        
        for (int i = 0; i < randomint(20,40); i++) {
            addRectangle(0.0, 0.0); //�簢�� �߰�
        }
        break;
    }
    glutPostRedisplay(); // ȭ�� �ٽ� �׸��� ��û

}



int main(int argc, char** argv)
{
    srand(static_cast<unsigned>(time(nullptr)));
    // ������ ����
    glutInit(&argc, argv); // glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
    glutInitWindowPosition(WIN_X, WIN_Y); // �������� ��ġ ����
    glutInitWindowSize(WIN_W, WIN_H); // �������� ũ�� ����
    glutCreateWindow("Example5"); // ������ ����(������ �̸�)

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else std::cout << "GLEW Initialized\n";

    //--- ������ �簢�� ����
    for (int i = 0; i < 40; i++) {
        addRectangle(0.0, 0.0); //�簢�� �߰�
    }
    //---
    glutDisplayFunc(drawScene); // ��� �Լ��� ����
    glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
    glutMouseFunc(Mouse); // ���콺 �Է�
    glutMotionFunc(Motion); // ���콺 �巡�� �Է�
    glutKeyboardFunc(Keyboard); // Ű���� �Է�
   

    glutMainLoop(); // �̺�Ʈ ó�� ����
    
}
