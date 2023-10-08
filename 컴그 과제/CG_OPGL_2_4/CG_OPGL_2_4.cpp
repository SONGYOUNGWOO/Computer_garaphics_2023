#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <random>
#include <stdio.h>
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
    GLfloat sizex; // 크기
    GLfloat sizey; // 크기
    GLfloat r, g, b; // 색상
    int dx, dy, sizes;
    GLfloat mx, my;
    GLfloat len;
}Rect;
Rect rect[5];
int n = 0;

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

// 새로운 사각형 추가 함수
void addRectangle(int x, int y) {
    if (n == 5) {
        for (int i = 0; i < 4; ++i) {
            rect[i].x = rect[i + 1].x;
            rect[i].y = rect[i + 1].y;
            rect[i].sizex = rect[i + 1].sizex;
            rect[i].sizey = rect[i + 1].sizey;
            rect[i].r = rect[i + 1].r;
            rect[i].g = rect[i + 1].g;
            rect[i].b = rect[i + 1].b;
            rect[i].mx = rect[i + 1].mx;
            rect[i].my = rect[i + 1].my;
            rect[i].dx = rect[i + 1].dx;
            rect[i].dy = rect[i + 1].dy;
        }
        n--;
    }

    rect[n].mx = (x - WIN_W / 2.0f) / (WIN_W / 2.0f);
    rect[n].my = ((WIN_H / 2.0f) - y) / (WIN_H / 2.0f);
    rect[n].x = rect[n].mx;
    rect[n].y = rect[n].my;
    rect[n].sizex = 0.05f;
    rect[n].sizey = 0.05f;
    rect[n].sizes = 0;
    rect[n].r = randomnum(0.0, 1.0);
    rect[n].g = randomnum(0.0, 1.0);
    rect[n].b = randomnum(0.0, 1.0);
    rect[n].dx = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;
    rect[n].dy = randomnum(0.0, 1.0) < 0.5f ? 1 : -1;
    rect[n].len = 0.0f;
    n++;
}

GLvoid drawScene() {
    glClearColor(0.745f, 0.745f, 0.745f, 1.0f); // 회색을 바탕색을 설정
    glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

    for (int i = 0; i < n; ++i) {
        cout << rect[i].x << endl << rect[i].y << endl;

        glColor3f(rect[i].r, rect[i].g, rect[i].b);

        glRectf(rect[i].x - rect[i].sizex, rect[i].y - rect[i].sizey,
            rect[i].x + rect[i].sizex, rect[i].y + rect[i].sizey);
    }

    glutSwapBuffers(); // 화면에 출력하기
}

//마우스 클릭 콜백 함수
void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        addRectangle(x, y); //사각형 추가
    }
    glutPostRedisplay(); // 화면 다시 그리기 요청
}

bool boolai = false;
//a 사각형들은 각각 대각선으로 이동 벽에 닿으면 튕김
void TimerA(int value) {
    if (boolai == false) {
        return;
    }

    for (int i = 0; i < n; ++i) {
        rect[i].x += rect[i].dx * 0.01f;
        rect[i].y += rect[i].dy * 0.01f;
        if (rect[i].x - rect[i].sizex <= -1 || rect[i].x + rect[i].sizex >= 1) {
            rect[i].dx *= -1;
            rect[i].x += rect[i].dx * 0.01f;
        }
        if (rect[i].y - rect[i].sizey <= -1 || rect[i].y + rect[i].sizey >= 1) {
            rect[i].dy *= -1;
            rect[i].y += rect[i].dy * 0.01f;
        }

    }
    glutPostRedisplay();
    glutTimerFunc(10, TimerA, 1);
}
// i 지그재그
void TimerI(int value) {
    if (boolai == false) {
        return;
    }
    float a = 0.03f;


    for (int i = 0; i < n; ++i) {

        rect[i].x += a; // 일정하게 우측으로

        rect[i].y += a * rect[i].dy;
        rect[i].len += a * rect[i].dy;

        if (rect[i].len > 0.1 || rect[i].len < -0.1) {
            rect[i].dy *= -1;
            rect[i].y += a * rect[i].dy;
        }

        if (rect[i].x - rect[i].sizex >= 1.0) {
            rect[i].x = -1 + rect[i].sizex;
        }

    }
    glutPostRedisplay();
    glutTimerFunc(60, TimerI, 1);
}

bool boolc = false;
//c: 크기 변화  사각형의 크기가 다양하게 변한다 변하지 않는다
void TimerC(int value) {
    if (boolc == false) {
        return;
    }

    for (int i = 0; i < n; ++i) {

        if (rect[i].sizes == 0) {
            rect[i].sizex += 0.01f;
            rect[i].sizey -= 0.01f;
        }
        else {
            rect[i].sizex -= 0.01f;
            rect[i].sizey += 0.01f;
        }
        if (rect[i].sizey <= 0.01f || rect[i].sizex <= 0.02f) {
            rect[i].sizes = (rect[i].sizes + 1) % 2;
        }

    }
    glutPostRedisplay();
    glutTimerFunc(60, TimerC, 1);
}



bool boolo = false;
//o: 색상 변화  사각형의 색상이 랜덤하게 변한다 변하지 않는다
void TimerO(int value) {
    if (boolo == false) {
        return;
    }

    for (int i = 0; i < n; ++i) {
        rect[i].r = randomnum(0.0, 1.0);
        rect[i].g = randomnum(0.0, 1.0);
        rect[i].b = randomnum(0.0, 1.0);
    }

    glutPostRedisplay();
    glutTimerFunc(50, TimerO, 1);
}

//s: 모든 애니메이션이 멈춘다

//m: 원래 그린 위치로 사각형들이 이동한다
void TimerM(int value) {

    for (int i = 0; i < n; ++i) {
        rect[i].x = rect[i].mx;
        rect[i].y = rect[i].my;
    }
    glutPostRedisplay();

}
//r: 사각형들을 삭제하고 다시 마우스 입력을 받을 수 있다
GLvoid clear() {
    n = 0;
    glutPostRedisplay();
}

//q: 프로그램을 종료한다

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a':case 'A':
        boolai = boolai == true ? false : true;
        if (boolai == true) {
            glutTimerFunc(60, TimerA, 1);
        }
        break;

    case 'i':case 'I':
        boolai = boolai == true ? false : true;
        if (boolai == true) {
            glutTimerFunc(60, TimerI, 1);
        }
        break;

    case 'c':case 'C':
        boolc = boolc == true ? false : true;
        if (boolc == true) {
            glutTimerFunc(60, TimerC, 1);
        }
        break;

    case 'o':case 'O':
        boolo = boolo == true ? false : true;
        if (boolo == true) {
            glutTimerFunc(60, TimerO, 1);
        }
        break;

    case 's':case 'S':
        boolai = false;
        boolc = false;
        boolo = false;
        break;

    case 'm':case 'M':
        glutTimerFunc(60, TimerM, 1);
        break;

    case 'r':case 'R':
        clear();
        boolai = false;
        boolc = false;
        boolo = false;
        break;

    case 'q':case 'Q':
        exit(0);
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
    glutCreateWindow("Example4"); // 윈도우 생성(윈도우 이름)

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else std::cout << "GLEW Initialized\n";


    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutMouseFunc(Mouse); // 마우스 입력[
    glutKeyboardFunc(Keyboard); // 키보드 입력


    glutMainLoop(); // 이벤트 처리 시작
}

