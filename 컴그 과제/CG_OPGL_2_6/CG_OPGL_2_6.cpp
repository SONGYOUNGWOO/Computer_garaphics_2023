#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <random>
#include <stdio.h>
//glew32.lib freeglut.lib 
using namespace std;

const int WIN_X = 10, WIN_Y = 10;
const int WIN_W = 800, WIN_H = 800;

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}
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
    GLfloat size,size2; // 크기
    GLfloat r, g, b; // 색상
    GLfloat x1, x2, x3, x4, y1, y2, y3, y4; // 사각형 4등분 중심
    bool selecbol;
    int selecrect;
    struct Rectangle* next; //다음 자료를 가르키는 포인터
    struct Rectangle* prior; //이전 자료를 가르키는 포인터
}Rect;

Rect* head = nullptr; //Rect* head = NULL
Rect* tail = nullptr; //Rect* tail = NULL

// 새로운 사각형 리스트에 추가
void addRectangle() {
    Rect* new_Rectangle = new Rect;

    //새로만든 구조체 초기화
    new_Rectangle->x = randomnum(-1.0, 1.0);
    new_Rectangle->y = randomnum(-1.0, 1.0);
    new_Rectangle->size = 0.2f;
    new_Rectangle->size2 = new_Rectangle->size / 2;
    new_Rectangle->selecrect = randomint(1, 3); // 1 ~ 3

    new_Rectangle->x1 = new_Rectangle->x - new_Rectangle->size2;
    new_Rectangle->x2 = new_Rectangle->x + new_Rectangle->size2;
    new_Rectangle->x3 = new_Rectangle->x - new_Rectangle->size2;
    new_Rectangle->x4 = new_Rectangle->x + new_Rectangle->size2;

    new_Rectangle->y1 = new_Rectangle->y + new_Rectangle->size2;
    new_Rectangle->y2 = new_Rectangle->y + new_Rectangle->size2;
    new_Rectangle->y3 = new_Rectangle->y - new_Rectangle->size2;
    new_Rectangle->y4 = new_Rectangle->y - new_Rectangle->size2;

    new_Rectangle->r = randomnum(0.0, 1.0);
    new_Rectangle->g = randomnum(0.0, 1.0);
    new_Rectangle->b = randomnum(0.0, 1.0);
    new_Rectangle->selecbol = false;

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
}

Rect* delete_node(Rect* del) {
    Rect* tmp = del->next;
    if (head != del && tail != del) {
        del->prior->next = del->next;
        del->next->prior = del->prior;
    }
    else if (del == head) {
        head = del->next;
        if (head != nullptr)
            head->prior = nullptr;
    }

    else if (del == tail) {
        tail = del->prior;
        tail->next = nullptr;
    }

    delete del;

    return tmp;
}

// 대각선
void Timerm1(int value) {
    float chsize = 0.01f; // 줄어들크기
    for (Rect* finder = head; finder != nullptr;) {
        if (finder->selecbol) {
            // 대각선
            if (finder->selecrect == 1) {
                finder->x1 -= chsize * 3;
                finder->y1 += chsize;
                finder->x2 += chsize;
                finder->y2 += chsize * 3;
                finder->x3 -= chsize;
                finder->y3 -= chsize * 3;
                finder->x4 += chsize * 3;
                finder->y4 -= chsize;
                finder->size2 -= chsize;
            }

            //상하좌우, 바람개비
            else if (finder->selecrect >= 2) {
                finder->x1 -= chsize * 3;
                finder->y1 -= chsize;
                finder->x2 -= chsize;
                finder->y2 += chsize * 3;
                finder->x3 += chsize;
                finder->y3 -= chsize * 3;
                finder->x4 += chsize * 3;
                finder->y4 += chsize;
                finder->size2 -= chsize;
            }

            if (finder->size2 < 0)
                finder = delete_node(finder);
            else
                finder = finder->next;
        }
        else 
            finder = finder->next;
    }
    glutPostRedisplay();
    glutTimerFunc(100, Timerm1, 1);
}

void Mouse(int button, int state, int x, int y)
{
    float mx, my;
    mx = (x - WIN_W / 2.0f) / (WIN_W / 2.0f);
    my = ((WIN_H / 2.0f) - y) / (WIN_H / 2.0f);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (Rect* finder = head; finder != nullptr; finder = finder->next) {
            cout << "x =" << finder->x << endl << "y =" << finder->y << endl;
            if ((finder->x - finder->size) < mx && mx < (finder->x + finder->size) && // 선택
                (finder->y - finder->size) < my && my < (finder->y + finder->size)) {
                finder->selecbol = true; //선택
                
            }
        }
    }
    glutPostRedisplay();
}

GLvoid drawScene() {
    glClearColor(0.745f, 0.745f, 0.745f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (Rect* finder = head; finder != nullptr; finder = finder->next) {
        glColor3f(finder->r, finder->g, finder->b);

        if (finder->selecbol) {
            glRectf(finder->x1 - finder->size2, finder->y1 - finder->size2,
                finder->x1 + finder->size2, finder->y1 + finder->size2);
            glRectf(finder->x2 - finder->size2, finder->y2 - finder->size2,
                finder->x2 + finder->size2, finder->y2 + finder->size2);
            glRectf(finder->x3 - finder->size2, finder->y3 - finder->size2,
                finder->x3 + finder->size2, finder->y3 + finder->size2);
            glRectf(finder->x4 - finder->size2, finder->y4 - finder->size2,
                finder->x4 + finder->size2, finder->y4 + finder->size2);
            
            //추가로그릴거
            if (finder->selecrect == 3) {
                glRectf(finder->x1 - finder->size2, finder->y2 - finder->size2,
                    finder->x1 + finder->size2, finder->y2 + finder->size2);

                glRectf(finder->x4 - finder->size2, finder->y2 - finder->size2,
                    finder->x4 + finder->size2, finder->y2 + finder->size2);

                glRectf(finder->x1 - finder->size2, finder->y3 - finder->size2,
                    finder->x1 + finder->size2, finder->y3 + finder->size2);

                glRectf(finder->x4 - finder->size2, finder->y3 - finder->size2,
                    finder->x4 + finder->size2, finder->y3 + finder->size2);
            }
        }
        else {
            glRectf(finder->x - finder->size, finder->y - finder->size,
                finder->x + finder->size, finder->y + finder->size);
        }
    }

    glutSwapBuffers();
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
    for (int i = 0; i < 5; i++) {
        addRectangle(); //사각형 추가
    }
    //---
    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutMouseFunc(Mouse); // 마우스 입력
    glutTimerFunc(50, Timerm1, 1);

    glutMainLoop(); // 이벤트 처리 시작

}
