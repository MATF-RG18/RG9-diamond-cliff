#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "image.h"

#define TIMER_ID1 0
#define ID_JUMP 1
#define PI 3.14

#define max(a, b) a > b ? a : b

/* Imena fajlova sa teksturama. */
#define SAND "sand.bmp"
#define SEA "sea.bmp"
#define GAME_OVER "game_over.bmp"
#define START_GAME "start.bmp"

/* Identifikatori tekstura. */
static GLuint names[4];

// indikator da li je postavljena prva ravan jer se prilikom pokretanja
// igrice prepreke postavljaju samo na drugi deo prve ravni
// a svaki sledeci put na celu ravan
static int first = 1;

// niz koji cuva y koordinate pri skoku loptice
static float jump_positions[180];
static int counter = 0;
static float speed = 0.45;

// brojac za ukupan skor koji je igrac postigao
static int score = 0;

// indikatori za pocetak i kraj igre
static int start = 0;
static int end = 0;
static int jump = 0;

// ugao za koji se noge i ruke pomeraju
static float angle = 0;

// duzina staza
static float lenght = 100;

// koordinate glavnog objekta
static float x_coord = 0;
static float y_coord = 0.75;
static float z_coord = 5;
float rotate_object = 0;

// dimenzije staze
static float x_plane = 10;
static float y_plane = 1;
static float z_plane = 50;
static float x_plane2 = 10;
static float y_plane2 = 1;
static float z_plane2 = 150;

static float width = 30;

// z koordinate ravni levo i desno od glavne ravni
static float z_coord_left_first = 50;
static float z_coord_right_first = 50;
static float z_coord_left_second = 150;
static float z_coord_right_second = 150;

// pomocni niz za kretanje igraca
static int possible_moves[] = {0, 0};

// svaka prepreka sadrzi kordinate i tip
// 0 - dijamant
// 1 - obicna kocka
typedef struct
{
    float x;
    float y;
    float z;
    int type;
} Obstacle;

// nizovi koji sadrze koordinate prepreka i njihov tip na svakoj ravni
Obstacle obstacles1[50];
Obstacle obstacles2[50];
static int pos1;
static int pos2;

// callback funkcije
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_release(unsigned char key, int x, int y);

// funkcije za inicijalizaciju
static void initialize(int argc, char **argv);
static void set_camera();
static void set_lights();

// funkcija koja obradjuje skok
static void on_jump();

// funkcije za postavljanje i iscrtavanje glavnog objekta i prepreka
static void draw_plane();
static void draw_main_object();
static void draw_obstacles(int type);
static void move_objects();
static void set_first();
static void set_obstacles(int type);

// funkcija koja proverava koliziju
static void resolve_collision();

// funkcije koje postavljaju pocetni i krajnji ekran
static void press_start();
static void game_over();

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

    // inicijalizacija niza iz koga ce se citati koordinate pri skoku
    int k = 0;
    for (float i = 0; i <= 18; i += 0.1)
        jump_positions[k++] = 1.5 * sin(i);

    Image *image;

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image, SAND);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(4, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Kreira se druga tekstura. */
    image_read(image, SEA);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    // Kreira se tekstura za pocetak igrice
    image_read(image, START_GAME);

    glBindTexture(GL_TEXTURE_2D, names[3]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    // Kreira se tekstura za kraj igrice
    image_read(image, GAME_OVER);

    glBindTexture(GL_TEXTURE_2D, names[2]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

    // Registruju se callback funkcije
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(press_start);
    glutKeyboardUpFunc(on_release);

    srand(time(NULL));

    glutFullScreen();

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
            // podesava se pozadina i poziva se od_display funkcija
            // kao i funkcija koja pomera objekte
            glClearColor(0.15, 0.5, 0.67, 0);
            glutDisplayFunc(on_display);
            glutTimerFunc(50, move_objects, 0);
            start = 1;
        }
        break;
    case 'a':
    case 'A':
        // pomeranje u levo
        // postavlja se indikator u pomocnom nizu
        // koji koristi on_release funkcija
        // i na taj nacin je kretanje tecnije
        possible_moves[0] = 1;
        glutPostRedisplay();
        break;
    case 'd':
    case 'D':
        // pomeranje u desno
        // takodje se postavlja indikator kao i za levo
        possible_moves[1] = 1;
        glutPostRedisplay();
        break;
    case 'w':
    case 'W':
        // skok
        // poziva funkciju on_jump koja obradjuje skok
        if (!jump)
        {
            glutTimerFunc(50, on_jump, ID_JUMP);
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

    // postavljaju se kamera i svetlo
    set_camera();
    set_lights();

    // prikaz trenutnog skora na ekranu
    glRasterPos3f(x_coord + 3, y_coord + 5, 5);
    char score_display[50] = "SCORE: ";
    char score_value[50];
    sprintf(score_value, " %d ", score);
    strcat(score_display, score_value);

    int len = (int)strlen(score_display);

    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score_display[i]);
    }

    // iscrtavaju se ravni i glavni objekat
    draw_plane();
    draw_main_object();

    // na pocetku se postavljaju prepreke na drugu ravan
    // i na drugi deo prve ravni koji se ne vidi pri pokretanju programa
    // a zatim se ostale prepreke postavljaju u tajmeru
    if (first)
    {
        set_first();
        set_obstacles(2);
        first = 0;
    }

    // crtaju se prepreke
    draw_obstacles(1);
    draw_obstacles(2);

    glutSwapBuffers();
}

static void on_release(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'a':
    case 'A':
        possible_moves[0] -= 1;
        break;
    case 'd':
    case 'D':
        possible_moves[1] -= 1;
        break;
    }
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
    // postavljanje prve ravni i tekstura na njoj
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    int first = 0;
    int second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_plane - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(5, -0.2, z_plane - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(0, -y_plane / 2, z_plane);
    glScalef(x_plane, y_plane / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    // druga ravan i njene teksture
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_plane2 - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(5, -0.2, z_plane2 - 50 + i * 20);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(0, -y_plane2 / 2, z_plane2);
    glScalef(x_plane2, y_plane2 / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    // prva ravan desno od glavne i postavljanje njene teksture
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_coord_right_first - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(-35, -0.2, z_coord_right_first - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(-20, -y_plane / 2, z_coord_right_first);
    glScalef(width, y_plane / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    // // druga ravan desno od glavne
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(-5, -0.2, z_coord_right_second - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(-35, -0.2, z_coord_right_second - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(-20, -y_plane / 2, z_coord_right_second);
    glScalef(width, y_plane / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    // prva ravan levo od glavne
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(5, -0.2, z_coord_left_first - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(35, -0.2, z_coord_left_first - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(20, -y_plane / 2, z_coord_left_first);
    glScalef(width, y_plane / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();

    // druga ravan levo od glavne
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUAD_STRIP);
    glNormal3f(0, 1, 0);
    first = 0;
    second = 0;
    for (int i = 0; i <= 5; i++)
    {
        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        glVertex3f(5, -0.2, z_coord_left_second - 50 + i * 20);

        glTexCoord2f(first, second);
        first = (first + 1) % 2;
        second = (second + 1) % 2;
        glVertex3f(35, -0.2, z_coord_left_second - 50 + i * 20);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(20, -y_plane / 2, z_coord_left_second);
    glScalef(width, y_plane / 2, lenght);
    glutSolidCube(1);
    glPopMatrix();
}

// iscrtava se covek
static void draw_main_object()
{
    GLfloat material_diffuse1[] = {0.5, 0.8, 1, 1};
    GLfloat material_diffuse2[] = {0.8, 0.8, 0.1, 1};
    GLfloat material_diffuse3[] = {1, 0.8, 0.59, 1};
    //glava
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse3);
    glPushMatrix();
    glTranslatef(x_coord, y_coord + 1.6, z_coord);
    glutSolidSphere(0.3, 10, 10);
    glPopMatrix();
    //telo
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse2);
    glPushMatrix();
    glTranslatef(x_coord, y_coord + .9, z_coord);
    glScalef(.65, .8, .4);
    glutSolidCube(1);
    glPopMatrix();
    //noge
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse1);
    glPushMatrix();
    glTranslatef(x_coord + .15, y_coord + .3, z_coord);
    glScalef(.5, 1.7, .3);
    glPushMatrix();
    glRotatef(20 * sin(30 * angle * PI / 180), 1, 0, 0);
    glutSolidCube(.5);
    glPopMatrix();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x_coord - .15, y_coord + .1, z_coord);

    glScalef(.5, 1.7, .1);
    glPushMatrix();
    glRotatef(20 * sin(30 * angle * PI / 180 + PI), 1, 0, 0);
    glutSolidCube(.5);
    glPopMatrix();
    glPopMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse2);
    //ruke
    glPushMatrix();
    glTranslatef(x_coord - .35, y_coord + 1, z_coord);
    glRotatef(-20, 0, 0, 1);
    glScalef(.2, .5, 0.1);
    glPushMatrix();
    glRotatef(20 * sin(30 * angle * PI / 180), 1, 0, 0);
    glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(x_coord + .35, y_coord + 1, z_coord);
    glRotatef(20, 0, 0, 1);
    glScalef(.2, .5, 0.1);
    glPushMatrix();
    glRotatef(20 * sin(30 * angle * PI / 180 + PI), 1, 0, 0);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();
}

static void move_objects(int value)
{
    if (value != 0)
        return;

    // pomeraju se z koordinate svih ravni
    z_plane -= speed;
    z_plane2 -= speed;
    z_coord_left_first -= speed;
    z_coord_left_second -= speed;
    z_coord_right_first -= speed;
    z_coord_right_second -= speed;

    // pomeraju se z koordinate prepreka
    for (int i = 0; i < pos1; i++)
        obstacles1[i].z -= speed;

    for (int i = 0; i < pos2; i++)
        obstacles2[i].z -= speed;

    // possible_moves[0] je indikator da je pritistunt taster za kretanje
    // levo i ako covek i dalje moze da se krece na tu stranu, a da ne dodje do kraja
    // staze, onda se pomera, inace ostaje u mestu, analogno za kretanje u desno
    if (possible_moves[0] && x_coord < 4.5)
        x_coord += 0.2;

    if (possible_moves[1] && x_coord > -4.5)
        x_coord -= 0.2;

    // uvecava se ugao rotacije nogu i ruku
    angle += 1;
    if (angle > 360)
        angle = 0;

    //kad jedna ravan izadje iz vidokruga kamere
    //vracamo je na kraj druge ravni i na toj ravni
    // ponovo postavljamo prepreke
    if (z_plane + 50 <= 0)
    {
        z_plane = 150;
        set_obstacles(1);
    }
    if (z_plane2 + 50 <= 0)
    {
        z_plane2 = 150;
        set_obstacles(2);
    }

    if (z_coord_left_first + 50 <= 0)
    {
        z_coord_left_first = 150;
    }
    if (z_coord_left_second + 50 <= 0)
    {
        z_coord_left_second = 150;
    }

    if (z_coord_right_first + 50 <= 0)
    {
        z_coord_right_first = 150;
    }
    if (z_coord_right_second + 50 <= 0)
    {
        z_coord_right_second = 150;
    }

    // rotiramo kuglu sve vreme
    rotate_object += 30;
    if (rotate_object >= 360)
        rotate_object += -360;

    // svaki put se proverava da li je doslo do kolizije
    resolve_collision();

    glutPostRedisplay();
    if (start && !end)
        glutTimerFunc(50, move_objects, 0);
}

// funkcija koja obradjuje skok
static void on_jump(int value)
{
    if (value != ID_JUMP)
        return;

    y_coord = 1 + jump_positions[counter++];
    // hardkodovano da ne ide dalje kada se covek spusti na stazu
    if (counter < 32)
    {
        glutTimerFunc(50, on_jump, ID_JUMP);
    }
    else
    {
        y_coord = 1;
        counter = 0;
        jump = 0;
    }
}

// icrtavamo prepreke razlicitih tipova
// koje su postavljene u nizovima obstacles1
// i obstacles2, type pokazuje na kojoj se ravni iscrtavaju prepreke
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
        else
        {
            glPushMatrix();
            glTranslatef(o.x, o.y, o.z);
            glScalef(2, 2, 2);
            glutSolidCube(1);
            glPopMatrix();
        }
    }
}

// funkcija koja postavlja koordinate prepreka
static void set_obstacles(int type)
{
    if (type == 1)
        pos1 = 0;
    else
        pos2 = 0;

    // u 10 redova na svakoj ravni se postavljaju prepreke
    // tako sto se uzima random broj od 0 do 4 i toliko prepreka se iscrtava
    // u svakom redu
    for (int i = 0; i < 10; i++)
    {
        int num = (int)rand() % 5;
        if (num == 0)
            num = 2;
        int diamond = 0;
        // niz koji oznacava koje su pozicije u redu slobodne
        int free_positions[] = {1, 1, 1, 1, 1};
        for (int j = 0; j < num; j++)
        {
            Obstacle o;
            // niz koji oznacava x koordinate pozicija koje odgovaraju slobodnim pozicijama
            // u nizu free_positions
            int positions[] = {4, 2, 0, -2, -4};
            int pos = (int)rand() % 5;
            if (free_positions[pos] == 1)
            {
                free_positions[pos] = 0;
                // random broj koji odredjuje da li ce se crtati kocka ili dijamant
                int t = (int)rand() % 2;
                if (t == 0 && !diamond)
                {
                    o.type = 0;
                    o.y = 0.5;
                    diamond = 1;
                }
                else
                {
                    o.type = 1;
                    o.y = 0.75;
                }

                o.x = positions[pos];
                if (type == 1)
                {
                    o.z = z_plane + 50 - i * 10;
                    obstacles1[pos1++] = o;
                }
                else
                {
                    o.z = z_plane2 + 50 - i * 10;
                    obstacles2[pos2++] = o;
                }
            }
        }
    }
}

// funkcija analogna funkciji set_obstacles
// samo sto se ona poziva samo na pocetku i postavlja prepreke samo
// na drugi deo ravni
static void set_first()
{
    for (int i = 0; i <= 2; i++)
    {
        int num = (int)rand() % 5;
        if (num < 4)
            num++;
        int diamond = 0;
        int free_positions[] = {0, 0, 0, 0, 0};
        for (int j = 0; j < num; j++)
        {
            Obstacle o;
            int positions[] = {4, 2, 0, -2, -4};
            int pos = (int)rand() % 5;
            if (free_positions[pos] == 0)
            {
                free_positions[pos] = 1;
                int t = (int)rand() % 2;
                if (t == 0 && !diamond)
                {
                    o.type = 0;
                    o.y = 0.5;
                    diamond = 1;
                }
                else
                {
                    o.type = 1;
                    o.y = 0.75;
                }
                o.x = positions[pos];
                o.z = z_plane + 50 - i * 20;
                obstacles1[pos1++] = o;
            }
        }
    }
}

// funkcija koja odredjuje razmak izmedju
// prepreka i coveka i koristi se u
// resavanju kolizije
static float distance(Obstacle o)
{
    float x = powf((o.x - x_coord), 2);
    float y = powf((o.y - y_coord), 2);
    float z = powf((o.z - z_coord), 2);

    return sqrt(x + y + z);
}

// u zavisnosti od toga na kojoj je ravni covek
// proverava se u svakom trenutku da li je doslo do kolizije sa
// preprekom ili dijamantom, tako sto se gleda razmak
// izmedju njih
static void resolve_collision()
{
    if (z_plane < z_plane2)
    {
        for (int i = 0; i < pos1; i++)
        {
            Obstacle o = obstacles1[i];
            if (distance(o) < 1.22)
            {
                // ako je dijamat, uvecava se skor
                if (obstacles1[i].type == 0)
                {
                    score += 10;
                    obstacles1[i].x = -100;
                }
                // ukoliko je doslo do sudara zavrsava se
                // igra i postavlja displej za kraj igre
                else if (obstacles1[i].type == 1)
                {
                    start = 0;
                    glutDisplayFunc(game_over);
                    glutPostRedisplay();
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < pos2; i++)
        {
            Obstacle o = obstacles2[i];
            if (distance(o) < 1.22)
            {
                if (obstacles2[i].type == 0)
                {
                    obstacles2[i].x = -100;
                    score += 10;
                }
                else if (obstacles2[i].type == 1)
                {
                    start = 0;
                    glutDisplayFunc(game_over);
                    glutPostRedisplay();
                }
            }
        }
    }
}

// funkcija koja postavlja teksture za kraj igre
void game_over(void)
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 0,
              0, 0, 0,
              1, 0, 0);

    GLfloat light_position[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0, 0);
    glVertex3f(-1, 0, -1);

    glTexCoord2f(0, 1);
    glVertex3f(1, 0, -1);

    glTexCoord2f(1, 1);
    glVertex3f(1, 0, 1);

    glTexCoord2f(1, 0);
    glVertex3f(-1, 0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);
    glRasterPos3f(0.6, 0.1, -0.1);
    char score_display[50] = "FINAL SCORE: ";
    char score_value[50];
    sprintf(score_value, " %d ", score);
    strcat(score_display, score_value);

    int len = (int)strlen(score_display);

    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score_display[i]);
    }

    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

// funkcija koja postavlja teksture na pocetku igre
void press_start(void)
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 2, 0,
              0, 0, 0,
              1, 0, 0);

    GLfloat light_position[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0, 0);
    glVertex3f(-1, 0, -1);

    glTexCoord2f(0, 1);
    glVertex3f(1, 0, -1);

    glTexCoord2f(1, 1);
    glVertex3f(1, 0, 1);

    glTexCoord2f(1, 0);
    glVertex3f(-1, 0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}