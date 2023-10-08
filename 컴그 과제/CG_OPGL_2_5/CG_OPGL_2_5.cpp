#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <random>
#include <stdio.h>

//glew32.lib freeglut.lib 
using namespace std;

const int WIN_X = 10, WIN_Y = 10;
const int WIN_W = 800, WIN_H = 800;

// 랜덤 색상 생성 함수
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

//사각형 5개
typedef struct Rectangle {
    GLfloat x, y; // 중심 좌표
    GLfloat size; // 크기
    GLfloat r, g, b; // 색상
    //GLfloat mx, my;
    struct Rectangle* next; //다음 자료를 가르키는 포인터
    struct Rectangle* prior; //이전 자료를 가르키는 포인터
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

// 새로운 사각형 리스트에 추가
void addRectangle(int x, int y) {
    Rect* new_Rectangle = new Rect;

    //새로만든 구조체 초기화
    new_Rectangle->x = randomnum(-1.0, 1.0);
    new_Rectangle->y = randomnum(-1.0, 1.0);
    new_Rectangle->size = 0.05f;
    new_Rectangle->r = randomnum(0.0, 1.0);
    new_Rectangle->g = randomnum(0.0, 1.0);
    new_Rectangle->b = randomnum(0.0, 1.0);

    new_Rectangle->prior = tail;
    new_Rectangle->next = nullptr;

    //리스트의 마지막에 새로생긴 Rect를 연결    
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
    glClearColor(0.745f, 0.745f, 0.745f, 1.0f); // 회색을 바탕색을 설정
    glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
    

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

    glutSwapBuffers(); // 화면에 출력하기
}

bool left_button = false;
//마우스 클릭 콜백 함수
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
        //지우개 생성
        left_button = true;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        delete eraser;
        eraser = nullptr;
        //지우개 제거
        left_button = false;
    }
    glutPostRedisplay(); // 화면 다시 그리기 요청
}


//주소를 받고 다른 이름 변수로 래퍼런스변수
bool IsCollision(const Rect& rect1, const Rect& rect2) {

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
   
    glutPostRedisplay(); // 화면 다시 그리기 요청
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
            addRectangle(0.0, 0.0); //사각형 추가
        }
        break;
    }
    glutPostRedisplay(); // 화면 다시 그리기 요청

}



int main(int argc, char** argv)
{
    srand(static_cast<unsigned>(time(nullptr)));
    // 윈도우 생성
    glutInit(&argc, argv); // glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
    glutInitWindowPosition(WIN_X, WIN_Y); // 윈도우의 위치 지정
    glutInitWindowSize(WIN_W, WIN_H); // 윈도우의 크기 지정
    glutCreateWindow("Example5"); // 윈도우 생성(윈도우 이름)

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else std::cout << "GLEW Initialized\n";

    //--- 지워질 사각형 생성
    for (int i = 0; i < 40; i++) {
        addRectangle(0.0, 0.0); //사각형 추가
    }
    //---
    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutMouseFunc(Mouse); // 마우스 입력
    glutMotionFunc(Motion); // 마우스 드래그 입력
    glutKeyboardFunc(Keyboard); // 키보드 입력
   

    glutMainLoop(); // 이벤트 처리 시작
    
}
