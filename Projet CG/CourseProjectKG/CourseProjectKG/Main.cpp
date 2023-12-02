#include <Windows.h>
#include <GL/glut.h>
#include <string>
#include <iostream>

// Позиция на дрона и скорост
float dronePosition[3] = { 0.0f, 0.0f, -5.0f };
float droneSpeed = 0.05f;

// Позиция и размер на площадката за кацане
float landingPadPosition[3] = { 0.0f, -1.0f, -5.0f };
float landingPadSize = 1.0f;
bool landingSuccess = false;

// Ротация на дрона
float droneRotation = 0.0f;


float cameraDistance = 5.0f;

// Режим на камерата (гледка отстрани или отгоре)
enum CameraMode { SIDE, TOP };
CameraMode currentCameraMode = SIDE;

// Функция за рисуване на площадката за кацане
void drawLandingPad() {
    glPushMatrix();
    glTranslatef(landingPadPosition[0], landingPadPosition[1], landingPadPosition[2]);
    glColor3f(0.5f, 0.5f, 0.5f);
    glutSolidCube(landingPadSize);

    // Надпис "H"
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(-0.1f, 0.4f, 0.0f);
    std::string label = "H";
    for (char const& c : label) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopMatrix();
}

// Функция за рисуване на дрона
void drawDrone() {
    glPushMatrix();
    glTranslatef(dronePosition[0], dronePosition[1], dronePosition[2]);

    // Завъртане на дрона на 90 градуса
    glRotatef(180.0f, 0.0f, 1.0f, 1.0f);

    // Рисуване на тялото
    glColor3f(0.2f, 0.2f, 0.2f);  // Тъмно сив цвят
    glutSolidCube(0.5);


    // Рисуване на колелата (4 на всяка страна на дрона)
    glColor3f(1.0f, 0.5f, 0.0f);  // Оранжев цвят
    glPushMatrix();
    glTranslatef(0.25f, -0.35f, 0.25f);
    glutSolidTorus(0.05, 0.1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.25f, -0.35f, 0.25f);
    glutSolidTorus(0.05, 0.1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25f, 0.35f, 0.25f);
    glutSolidTorus(0.05, 0.1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.25f, 0.35f, 0.25f);
    glutSolidTorus(0.05, 0.1, 10, 10);
    glPopMatrix();

    glPopMatrix();
}


// Рисуване на земята
void drawGround() {
    glPushMatrix();
    glColor3f(0.2f, 0.8f, 0.2f); // Зелен цвят за земята
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -1.0f, -100.0f);
    glVertex3f(100.0f, -1.0f, -100.0f);
    glVertex3f(100.0f, -1.0f, 100.0f);
    glVertex3f(-100.0f, -1.0f, 100.0f);
    glEnd();
    glPopMatrix();
}

// Основна функция за рисуване
void display() {
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f); // Светло син цвят за фон
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Настройка на камерата в зависимост от режима
    if (currentCameraMode == SIDE) {
        gluLookAt(5.0f, 0.0f, 0.0f,
            dronePosition[0], dronePosition[1], dronePosition[2],
            0.0f, 1.0f, 0.0f);
    }
    else if (currentCameraMode == TOP) {
        gluLookAt(dronePosition[0], dronePosition[1] + cameraDistance, dronePosition[2],
            dronePosition[0], dronePosition[1], dronePosition[2],
            0.0f, 0.0f, -1.0f);
    }

    // Рисуване на земята, дрона и площадката за кацане
    drawGround();
    drawDrone();
    drawLandingPad();

    glutSwapBuffers();
}

// Превключване на режима на камерата
void switchCameraMode() {
    if (currentCameraMode == SIDE) {
        currentCameraMode = TOP;
        std::cout << "Switched to Top View\n";
    }
    else if (currentCameraMode == TOP) {
        currentCameraMode = SIDE;
        std::cout << "Switched to Side View\n";
    }
}

// Променя размерите на прозореца при промяна на размера на прозореца
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Обработка на клавишите за управление на дрона
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        dronePosition[2] -= droneSpeed; // Движение напред
        break;
    case 's':
        dronePosition[2] += droneSpeed; // Движение назад
        break;
    case 'i':
        dronePosition[1] += droneSpeed; // Движение нагоре
        break;
    case 'u':
        dronePosition[1] -= droneSpeed; // Движение надолу
        break;
    case 'a':
        dronePosition[0] -= droneSpeed; // Движение наляво
        break;
    case 'd':
        dronePosition[0] += droneSpeed; // Движение надясно
        break;
    case 'l':
        switchCameraMode(); // Превключване на режима на камерата
        break;
    }

    glutPostRedisplay(); // Прерисуване на прозореца
}

// Проверка за успешно кацане на дрона
void checkLanding() {
    float droneBottom = dronePosition[1] - 0.25f; // Предполага се, че долната част на дрона е на разстояние 0.25 от центъра му

    if (droneBottom <= landingPadPosition[1] &&
        dronePosition[0] >= landingPadPosition[0] - landingPadSize / 2.0f &&
        dronePosition[0] <= landingPadPosition[0] + landingPadSize / 2.0f &&
        dronePosition[2] >= landingPadPosition[2] - landingPadSize / 2.0f &&
        dronePosition[2] <= landingPadPosition[2] + landingPadSize / 2.0f) {
        landingSuccess = true;
        std::cout << "Landing successful!\n";
    }
    else {
        landingSuccess = false;
    }
}

// Функция за обновление на състоянието на сцената
void update(int value) {
    droneRotation += 1.0f; // Постоянно въртене на перката

    checkLanding(); // Проверка за успешно кацане

    glutPostRedisplay(); // Прерисуване на прозореца
    glutTimerFunc(25, update, 0); // Повторно извикване на функцията след 25 милисекунди
}

// Основна функция
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Drone Landing");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(25, update, 0); // Извикване на функцията за обновление с времева закъснителност от 25 милисекунди

    glutMainLoop();

    return 0;
}
