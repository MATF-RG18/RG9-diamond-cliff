#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

#define TIMER_ID1 0
#define PI 3.14

#define max(a, b) a > b ? a : b

// ugao za koji ce loptica da se pomera pri skoku
static float fi = 0;
static float jump_speed = 0.1;

// indikatori za pocetak i kraj igre
static int start = 0;
static int end = 0;
static int jump = 0;

// duzina staza
static float lenght = 100;

// koordinate glavnog objekta
static float x_coord = 0;
static float y_coord = 0;
static float z_coord = 5;
static float r = 1;
float rotate_object = 0;

// dimenzije staze
static float x_plane = 10;
static float y_plane = 1;
static float z_plane = 50;
static float x_plane2 = 10;
static float y_plane2 = 1;
static float z_plane2 = 150;

// svaka prepreka sadrzi kordinate i tip
// 0 - dijamant
// 1 - obicna kocka
// 2 - rupa
typedef struct
{
    float x;
    float y;
    float z;
    int type;
} Obstacle;

Obstacle obstacles1[50];
Obstacle obstacles2[50];
static int pos1;
static int pos2;

// callback funkcije
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);

static void initialize(int argc, char **argv);
static void set_camera();
static void set_lights();

static void on_jump();

static void draw_plane();
static void draw_main_object();
static void draw_obstacles(int type);
static void move_objects();
static void set_first();
static void set_obstacles(int type);

int main(int argc, char **argv)
{
    initialize(argc, argv);
    glutMainLoop();

    return 0;
}

static void initialize(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    // Kreiranje prozor
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Diamond cliff");

    // Registruju se callback funkcije
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    srand(time(NULL));

    // Inicijalizaciju  OpenGL-a
    glClearColor(0.15, 0.5, 0.67, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        // igra se zavrsava
        exit(0);
        break;
    case 'S':
    case 's':
        //igra se startuje
        if (!start && !end)
        {
            glutTimerFunc(50, move_objects, 0);
            start = 1;
        }
        break;
        // case 'p':
        //     // igra se pauzira
        //     start = 0;
        //     break;

    case 'a':
    case 'A':
        // pomeranje u levo
        x_coord += 1;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
        // pomeranje u desno
        x_coord -= 1;
        glutPostRedisplay();
        break;
    case 'w':
    case 'W':
        // skok
        if (!jump)
        {
            glutTimerFunc(50, on_jump, 0);
            jump = 1;
        }
        glutPostRedisplay();
        break;
    }
}

static void on_reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (double)width / height, 1, 50);
}

static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_camera();
    set_lights();

    draw_plane();
    draw_main_object();

    // na pocetku se postavljaju prepreke na drugu ravan
    // i na drugi deo prve ravni koji se ne vidi pri pokretanju programa
    // a zatim se ostale prepreke postavljaju u tajmeru
    if (!start)
    {
        printf("Postavljam prvi");
        set_first();
        set_obstacles(2);
    }

    // crtaju se prepreke
    draw_obstacles(1);
    draw_obstacles(2);

    glutSwapBuffers();
}

static void set_camera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x_coord, y_coord + 5, z_coord - 5, x_coord, y_coord, z_coord + 7, 0, 1, 0);
}

static void set_lights()
{
    GLfloat light_position[] = {0, 1, -0.3, 0};
    GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat light_diffuse[] = {0.9, 0.9, 0.9, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

static void draw_plane()
{
    // preuzeta boja sa neta
    GLfloat material_ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat material_diffuse[] = {0.1, 0.35, 0.1, 1.0};
    GLfloat material_specular[] = {0.45, 0.55, 0.45, 1.0};
    GLfloat material_emission[] = {0, 0, 0, 0};
    GLfloat shininess = 0.25;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    glTranslatef(0, -y_plane / 2, z_plane);
    glScalef(x_plane, -y_plane / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    GLfloat material_ambient1[] = {0.2125, 0.1275, 0.054, 1.0};
    GLfloat material_diffuse1[] = {0.714, 0.4284, 0.18144, 1.0};
    GLfloat material_specular1[] = {0.393548, 0.271906, 0.166721, 1.0};
    GLfloat shininess1 = 0.2;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess1);

    glPushMatrix();
    glTranslatef(0, -y_plane2 / 2, z_plane2);
    glScalef(x_plane2, -y_plane2 / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();
}

static void draw_main_object()
{
    // preuzeta bronzana boja sa neta
    GLfloat material_ambient[] = {0.2125, 0.1275, 0.054, 1.0};
    GLfloat material_diffuse[] = {0.714, 0.4284, 0.18144, 1.0};
    GLfloat material_specular[] = {0.393548, 0.271906, 0.166721, 1.0};
    GLfloat shininess = 60;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    glTranslatef(x_coord, r, z_coord + 2);
    glRotatef(rotate_object, 1, 0, 0);
    glutSolidSphere(r, 50, 50);
    glPopMatrix();
}

static void move_objects(int value)
{
    if (value != 0)
        return;

    z_plane -= 0.5;
    z_plane2 -= 0.5;

    for (int i = 0; i < pos1; i++)
        obstacles1[i].z -= 0.5;

    for (int i = 0; i < pos2; i++)
        obstacles2[i].z -= 0.5;

    //printf("%f %f\n", z_plane, z_plane2);

    //kad jedna ravan izadje iz vidokruga kamere
    //vracamo je na kraj druge ravni i na toj ravni
    // ponovo postavljamo prepreke
    if (z_plane + 50 <= 0)
    {
        z_plane = 150;
        set_obstacles(1);
        //printf("Menjam prvu ravan, koord1: %f koord2: %f\n", z_plane, z_plane2);
    }
    if (z_plane2 + 50 <= 0)
    {
        z_plane2 = 150;
        set_obstacles(2);
        //printf("Menjam drugu ravan, koord1: %f koord2: %f\n", z_plane, z_plane2);
    }

    // rotiramo kuglu sve vreme
    rotate_object += 30;
    if (rotate_object >= 360)
        rotate_object += -360;

    glutPostRedisplay();
    if (start && !end)
        glutTimerFunc(50, move_objects, 0);
}

// funkcija koja obradjuje skok
// !!! FIXME : pomera se samo kamera, ne i loptica
static void on_jump(int value)
{
    if (value != 0)
        return;

    if (jump)
    {
        fi += jump_speed;
        if (fi >= PI)
        {
            fi = 0;
            jump = 0;
        }
        y_coord = r + 2 * sin(fi);
        glutPostRedisplay();
    }

    glutPostRedisplay();
    if (start && !end)
        glutTimerFunc(50, on_jump, 0);
}

// icrtavamo prepreke razlicitih tipova
// koje su postavljene u nizovima obstacles1
// i obstacles2
static void draw_obstacles(int type)
{
    int len = 0;
    if (type == 1)
        len = pos1;
    else
        len = pos2;

    for (int i = 0; i < len; i++)
    {
        Obstacle o;
        if (type == 1)
            o = obstacles1[i];
        else
            o = obstacles2[i];
        // printf("%f %f %f\n", o.x, o.y, o.z);

        GLfloat material_ambient[] = {0.2125, 0.1275, 0.054, 1.0};
        GLfloat material_diffuse[] = {0.714, 0.4284, 0.18144, 1.0};
        GLfloat material_specular[] = {0.393548, 0.271906, 0.166721, 1.0};
        GLfloat shininess = 0.2;

        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // ako je tip prepreke 0 crta se dijamant
        if (o.type == 0)
        {
            glPushMatrix();
            glTranslatef(o.x, o.y, o.z);
            glScalef(0.6, 0.6, 0.6);
            glutSolidIcosahedron();
            glPopMatrix();
        }
        // ako je tip prepreke 1 crta se obicna kocka
        else if (o.type == 1)
        {
            glPushMatrix();
            glTranslatef(o.x, o.y, o.z);
            glScalef(2, 2, 2);
            glutSolidCube(1);
            glPopMatrix();
        }
        // ako je tip prepreke 2 crta se rupa
        else if (o.type == 2)
        {
            GLfloat material_ambient1[] = {0.0, 0.0, 0.0, 1.0};
            GLfloat material_diffuse1[] = {0.01, 0.01, 0.01, 1.0};
            GLfloat material_specular1[] = {0.5, 0.5, 0.5, 1.0};
            GLfloat shininess1 = 0.25;

            glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
            glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
            glMaterialf(GL_FRONT, GL_SHININESS, shininess1);

            //FIXME: popravi velicinu rupe kad namestis da se lepo iscrtava
            glPushMatrix();
            glTranslatef(o.x, o.y, o.z);
            glScalef(2, 2, 2);
            glutSolidCube(1);
            glPopMatrix();
        }
    }
}

static void set_obstacles(int type)
{
    if (type == 1)
        pos1 = 0;
    else
        pos2 = 0;

    for (int i = 0; i < 10; i++)
    {
        int num = (int)rand() % 5;
        if (num == 0)
            num = 2;
        int diamond = 0;
        int hole = 0;
        for (int j = 0; j < num; j++)
        {
            Obstacle o;
            int free_positions[] = {0, 0, 0, 0, 0};
            int positions[] = {4, 2, 0, -2, -4};
            int pos = (int)rand() % 5;
            if (free_positions[pos] == 0)
            {
                free_positions[pos] = 1;
                int t = (int)rand() % 3;
                if (t == 0 && !diamond)
                {
                    o.type = 0;
                    o.y = 0.5;
                    diamond = 1;
                }
                else if (t == 2 && !hole)
                {
                    o.type = 2;
                    o.y = -1;
                    hole = 1;
                }
                else
                {
                    o.type = 1;
                    o.y = 0.5;
                }

                o.x = positions[pos];
                if (type == 1)
                {
                    o.z = z_plane + 50 - i * 10;
                    obstacles1[pos1++] = o;
                    printf("1 : %f %f %f\n", o.x, o.y, o.z);
                }
                else
                {
                    o.z = z_plane2 + 50 - i * 10;
                    obstacles2[pos2++] = o;
                    printf("2 : %f %f %f\n", o.x, o.y, o.z);
                }
            }
        }
    }
}

static void set_first()
{
    for (int i = 0; i <= 2; i++)
    {
        int num = (int)rand() % 5;
        if (num < 4)
            num++;
        int diamond = 0;
        int hole = 0;
        for (int j = 0; j < num; j++)
        {
            Obstacle o;
            int free_positions[] = {0, 0, 0, 0, 0};
            int positions[] = {4, 2, 0, -2, -4};
            int pos = (int)rand() % 5;
            if (free_positions[pos] == 0)
            {
                free_positions[pos] = 1;
                int t = (int)rand() % 3;
                if (t == 0 && !diamond)
                {
                    o.type = 0;
                    o.y = 0.5;
                    diamond = 1;
                }
                else if (t == 2 && !hole)
                {
                    o.type = 2;
                    o.y = -1;
                    hole = 1;
                }
                else
                {
                    o.type = 1;
                    o.y = 0.5;
                }
                o.x = positions[pos];
                o.z = z_plane + 50 - i * 20;
                obstacles1[pos1++] = o;
                printf("Prva pozicija %f %f %f\n", o.x, o.y, o.z);
            }
        }
    }
}
