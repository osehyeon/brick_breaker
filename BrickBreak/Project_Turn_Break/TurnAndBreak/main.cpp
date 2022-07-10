#include <Windows.h>
#include <gl/glut.h>
#include <gl/glu.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define width           600
#define height          600
#define	polygon_num		50
#define total           81
#define PI              3.141592

int		left = 0;
int		bottom = 0;

float theta = 0.0;


int score = 0;

char state[64];
char text[] = "SCORE:";
char made_by[] = "made by SeHyeon";
char state1[] = "Press S To Start";
char state2[] = "Game In Progress";
char state3[] = "End Game [ Press D To Menu ]";

typedef struct _Point {
    float	x;
    float	y;
} Point;

Point stick, circle_velocity, bar_a, bar_b, bar_c, bar_d;

float stick_velocity = 0.1;

float radius = 10.0;

float block_x = -200.0;
float block_y = -250.0;

struct c_Point {
    float x, y;
    int collision;
};

c_Point* block;

c_Point circle;

void init_Block(void) {
    block = new c_Point[total];
    int num = 0;
    for (int i = 0; i < total; i++) {
        if (i % 9 == 0) {
            num = 0;
            block_y += 50;
        }
        block[i].x = block_x + 50 * num;
        block[i].y = block_y;

        block[i].collision = 0;

        num++;
    }
}

void init() {

    circle.x = -200.0;
    circle.y = -400.0;
    circle_velocity.x = 0;
    circle_velocity.y = 0.;

    strcpy_s(state, state1);

    circle.collision = -1;

    init_Block();
}



void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(-width, width, -height, height);

}

void Modeling_Circle(c_Point c, float r) {
    if (c.collision == 0) {
        float delta = 2.0 * PI / polygon_num;
        glBegin(GL_POLYGON);
        glColor3f(1.0, 1.0, 0);
        for (int i = 0; i < polygon_num; i++) {
            glVertex2f(c.x + r * cos(delta * i), c.y + r * sin(delta * i));
        }
        glEnd();
    }
}

void Modeling_Big_Circle(float x, float y, float r) {

    float delta = 2.0 * PI / polygon_num;
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.5, 0.5);
    for (int i = 0; i < polygon_num; i++) {
        glVertex2f(x + r * cos(delta * i), y + r * sin(delta * i));
    }
    glEnd();
}

void Modeling_Square() {
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(-600, -600);
    glVertex2f(600, -600);
    glVertex2f(600, 600);
    glVertex2f(-600, 600);
    glEnd();
}

void renderBitmapCharacterString(float x, float y, void* font, char* string) {
    char* c;
    glRasterPos2f(x, y);

    for (c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

void renderBitmapCharacterInt(float x, float y, void* font, int score) {
    char c[100];
    sprintf_s(c, "%d", score);
    glRasterPos2f(x, y);

    for (int i = 0; c[i] != '\0'; i++)
        glutBitmapCharacter(font, c[i]);
}


void Modeling_Stick(void) {

    if (theta < 0)
        theta += 2 * PI;

    if (theta > 2 * PI)
        theta -= 2 * PI;

    bar_a.x = 500 * sin(theta) + 50.0 * cos(theta);
    bar_a.y = -500 * cos(theta) + 50.0 * sin(theta);

    bar_b.x = 500 * sin(theta) + 50.0 * cos(theta) + 25.0 * sin(theta);
    bar_b.y = -500 * cos(theta) + 50 * sin(theta) - 25.0 * cos(theta);

    bar_c.x = 500 * sin(theta) - 50.0 * cos(theta) + 25.0 * sin(theta);
    bar_c.y = -500 * cos(theta) - 50.0 * sin(theta) - 25.0 * cos(theta);

    bar_d.x = 500 * sin(theta) - 50.0 * cos(theta);
    bar_d.y = -500 * cos(theta) - 50.0 * sin(theta);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(bar_a.x, bar_a.y);
    glVertex2f(bar_b.x, bar_b.y);
    glVertex2f(bar_c.x, bar_c.y);
    glVertex2f(bar_d.x, bar_d.y);
    glEnd();
}

void Modeling_Block(void) {
    for (int i = 0; i < total; i++) {
        if (block[i].collision == 0) {
            glColor3f(1.0, 1.0, 1.0);
            if (i == total / 2)
                glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
            glVertex2f(block[i].x + 20, block[i].y + 20);
            glVertex2f(block[i].x + 20, block[i].y - 20);
            glVertex2f(block[i].x - 20, block[i].y - 20);
            glVertex2f(block[i].x - 20, block[i].y + 20);
            glEnd();

        }

    }

}

int Collision_Detection_to_Bar(Point a, Point b, Point c, Point d, c_Point circle) {

    float dist[101];

    float start_x = a.x;
    float start_y = a.y;
    float delta_x = (d.x - a.x) / 100;
    float delta_y = (d.y - a.y) / 100;

    float total_x = 0.0;
    float total_y = 0.0;

    for (int i = 0; i <= 100; i++) {
        total_x = start_x + i * delta_x;
        total_y = start_y + i * delta_y;

        dist[i] = sqrt((circle.x - total_x) * (circle.x - total_x) + (circle.y - total_y) * (circle.y - total_y));
    }

    for (int i = 0; i <= 100; i++) {
        if (dist[i] < radius)
            return 1;
    }

    return 0;
}

int Collision_Detection_to_Circle(float a, float b, c_Point c) {
    float distance_c = 0.0;
    distance_c = sqrt((a - c.x) * (a - c.x) + (b - c.y) * (b - c.y));

    if (distance_c < 520)
        return 0;
    else
        return 1;
}


void Collision_Detection(void) {

    float nx = 0.0;
    float ny = 0.0;
    float ndx = 0.0;
    float ndy = 0.0;
    float speed = 0.0;
    float distance = 0.0;
    float a = 0.0;

    speed = sqrt(circle_velocity.x * circle_velocity.x + circle_velocity.y * circle_velocity.y);

    distance = sqrt(circle.x * circle.x + circle.y * circle.y);

    nx = -circle.x / distance;
    ny = -circle.y / distance;

    ndx = -circle_velocity.x / speed;
    ndy = -circle_velocity.y / speed;

    a = ndx * nx + ndy * ny;

    if (Collision_Detection_to_Bar(bar_a, bar_b, bar_c, bar_d, circle) == 1) {
        circle_velocity.x = (2.0 * a * nx - ndx) * speed;
        circle_velocity.y = (2.0 * a * ny - ndy) * speed;
    }

    if (Collision_Detection_to_Circle(0.0, 0.0, circle) == 1) {
        circle_velocity.x = (2.0 * a * nx - ndx) * speed;
        circle_velocity.y = (2.0 * a * ny - ndy) * speed;
        score -= 5;
    }

}


int Collision_Detection_x(c_Point ball, float ball_r, c_Point square) {
    if (ball.y + ball_r < square.y - 25)
        return 0;
    else if (ball.y - ball_r > square.y + 25)
        return 0;
    else if (ball.x < square.x - 25)
        return 0;
    else if (ball.x > square.x + 25)
        return 0;
    else
        return 1;

}

int Collision_Detection_y(c_Point ball, float ball_r, c_Point square) {
    if (ball.y < square.y - 25)
        return 0;
    else if (ball.y > square.y + 25)
        return 0;
    else if (ball.x + ball_r < square.x - 25)
        return 0;
    else if (ball.x - ball_r > square.x + 25)
        return 0;
    else
        return 1;

}

void Collision_Detection_to_Block(void) {

    circle.x += circle_velocity.x;
    circle.y += circle_velocity.y;

    for (int i = 0; i < total; i++) {
        if (block[i].collision == 0) {
            if (Collision_Detection_x(circle, radius, block[i]) == 1) {
                block[i].collision = 1;
                circle_velocity.y *= -1.0;
                score++;
                break;
            }
            else if (Collision_Detection_y(circle, radius, block[i]) == 1) {
                block[i].collision = 1;
                circle_velocity.x *= -1.0;
                score++;
                break;
            }
        }

    }
}


void ClearGame(void) {
    if (block[total / 2].collision == 1) {
        circle_velocity.x = 0;
        circle_velocity.y = 0;
        circle.collision = 1;
        strcpy_s(state, state3);
    }
}


void RenderScene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    Modeling_Square();
    Modeling_Big_Circle(0, 0, 530);
    Modeling_Stick();
    Modeling_Block();
    Modeling_Circle(circle, radius);

    renderBitmapCharacterString(325, 525, GLUT_BITMAP_TIMES_ROMAN_24, text);
    renderBitmapCharacterString(340, -580, GLUT_BITMAP_8_BY_13, made_by);
    renderBitmapCharacterInt(525, 525, GLUT_BITMAP_TIMES_ROMAN_24, score);
    renderBitmapCharacterString(-575, -575, GLUT_BITMAP_HELVETICA_18, state);

    Collision_Detection_to_Block();
    Collision_Detection();
    ClearGame();

    glFlush();
    glutSwapBuffers();

}

void Special_Key(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT: theta -= stick_velocity; break;
    case GLUT_KEY_RIGHT: theta += stick_velocity; break;
    }

    glutPostRedisplay();
}

void My_Key(unsigned char key, int x, int y) {
    if (circle.collision == -1) {
        switch (key) {

        case 's': circle_velocity.x = 2;
            circle_velocity.y = 3;
            circle.collision = 0;
            strcpy_s(state, state2);
            break;

        }
    }
    if (circle.collision == 1) {
        switch (key) {

        case 'd':    circle.x = -200.0;
            circle.y = -400.0;
            circle.collision = -1;
            strcpy_s(state, state1);
            score = 0;
            for (int i = 0; i < total; i++)
                block[i].collision = 0;
            break;


        }
    }
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Turn, and Break");
    init();
    glutDisplayFunc(RenderScene);
    glutIdleFunc(RenderScene);
    glutReshapeFunc(MyReshape);
    glutKeyboardFunc(My_Key);
    glutSpecialFunc(Special_Key);
    glutMainLoop();
}