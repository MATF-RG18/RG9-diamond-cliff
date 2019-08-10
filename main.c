#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>

#define TIMER_ID1 0

// Dimenzije prozora
static int window_width, window_height;

//indikatori za pocetak i kraj igre
static int start;
static int end;

//tajmeri
static int timer_active;

//Koordinate ravni po kojoj se krece
static float x_plane1 = 3;
static float y_plane1 = 0;
static float z_plane1 = 10;
static float x_plane2 = 3;
static float y_plane2 = 0;
static float z_plane2 = 40;

//koordinate glavnog objekta
static float x_coord = 3;
static float y_coord = 1;
static float z_coord = 0;

// niz za opisivanje kretanja glavnog objekta
static int possible_moves[] = {0, 0};

// Deklaracije callback funkcija.
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);

//funkcija za pokretanje ravni
static void move_planes(int value);

int main(int argc, char **argv)
{
    srand(time(NULL));
    // Inicijalizacija GLUT-a
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

    // Inicijalizaciju  OpenGL-a
    glClearColor(0.15, 0.5, 0.67, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.5);

    // Glavna petlja
    glutMainLoop();

    return 0;
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
        start = 1;
        if (!end)
        {
            if (!timer_active)
            {
                glutTimerFunc(50, move_planes, timer_active);
                timer_active = 1;
            }
        };
        break;
    case 'p':
        // igra se pauzira
        timer_active = 0;
        break;

    case 'a':
    case 'A':
        // pomeranje u levo
        start = 1;
        possible_moves[0] = 1;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
        // pomeranje u desno
        start = 1;
        possible_moves[1] = 1;
        glutPostRedisplay();
        break;
    }
}

static void on_reshape(int width, int height)
{
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}

static void on_display(void)
{
    // Brise se prethodni sadrzaj prozora
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Podesava se viewport
    glViewport(0, 0, window_width, window_height);

    // Podesavamo projekciju
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60,
        window_width / (float)window_height,
        1, 50);

    // Podesavamo tacku pogleda
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x_coord, y_coord + 5, z_coord - 7,
              x_coord, y_coord, z_coord + 5,
              0.0, 1.0, 0.0);

    //iscrtavanje glavne ravni
    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    for (int i = 0; i <= 35; i += 7)
    {
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(0, 0.51, z_plane1 - 15 + i);
        glVertex3f(6, 0.51, z_plane1 - 15 + i);
    }
    glEnd();

    glTranslatef(x_plane1, y_plane1, z_plane1);
    glScalef(6, 1, 30);
    glutSolidCube(1);
    glPopMatrix();

    //iscrtavanje glavnog objekta(lopte trenutno)
    glPushMatrix();
    glTranslatef(x_coord, y_coord + 1.2, z_coord - 3.5);
    glColor3f(1, 0, 0);
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();

    // Nova slika se salje na ekran
    glutSwapBuffers();
}

static void move_planes(int value)
{
    if (value)
        return;

    //kretanje glavnog objekta
    if (possible_moves[0] && x_coord < 5.5)
    {
        x_coord += 0.1;
    }
    if (possible_moves[1] && x_coord > 1.5)
    {
        x_coord -= 0.1;
    }

    // pomeranje glavne ravni
    z_plane1 -= 0.25;

    glutPostRedisplay();
    if (timer_active)
    {
        glutTimerFunc(30, move_planes, 0);
    }
}