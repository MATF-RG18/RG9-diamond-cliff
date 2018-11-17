#include <stdlib.h>
#include <GL/glut.h>

// Dimenzije prozora 
static int window_width, window_height;

// Deklaracije callback funkcija. 
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);

int main(int argc, char **argv)
{
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
    switch (key) {
    case 27:
        // Program se zavrsava
        exit(0);
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
            window_width/(float)window_height,
            1, 25);

    // Podesavamo tacku pogleda
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            2, 2, 2,
            0, 0, 0,
            0, 1, 0
        );

    glBegin(GL_LINES);
    	// X-osa 
        glColor3f(1,0,0); 
        glVertex3f(0,0,0);
        glVertex3f(8,0,0);
        
        // Y-osa
        glColor3f(0,0.5,0);
        glVertex3f(0,0,0);
        glVertex3f(0,8,0);
        
        // Z-osa
        glColor3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(0,0,8);
    glEnd();
    // Nova slika se salje na ekran
    glutSwapBuffers();
}
 
