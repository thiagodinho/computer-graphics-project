#include <iostream>
#include <windows.h>
#include <gl/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include "glaux.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

GLfloat posX = 100.0f;
GLfloat posY = 150.0f;
GLsizei rsize = 5;  // Reduzindo o tamanho do objeto principal
GLfloat xstep = 3.5f;
GLfloat ystep = 3.5f;
GLfloat windowWidth;
GLfloat windowHeight;
GLfloat red = 1.0f;
GLfloat green = 0.0f;
GLfloat blue = 0.0f;
GLint pontuacaoCursor1 = 0;
GLint pontuacaoCursor2 = 0;
GLfloat cursor1Y1 = 100.0f;
GLfloat cursor1Y2 = 150.0f;
GLfloat cursor2Y1 = 100.0f;
GLfloat cursor2Y2 = 150.0f;
GLfloat y1bar = 75.0f;
GLfloat y2bar = 175.0f;
bool nerfAtivo = false;
time_t startTime;
time_t currentTime;
int tempoDecorrido;
bool jogoPausado = false;
GLuint texturaCursor1, texturaCursor2;


GLuint loadImage(const char *imagepath){
    GLuint textureID;
    int width, height, nrComponents;
    unsigned char *data = stbi_load(imagepath, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << imagepath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void CarregarTexturas() {
    texturaCursor1 = loadImage("C:\\Users\\Daniel\\Desktop\\UNISO\\ComputacaoGrafica\\ProjetoFinal\\Textura_madeira.jpg");
    texturaCursor2 = loadImage("C:\\Users\\Daniel\\Desktop\\UNISO\\ComputacaoGrafica\\ProjetoFinal\\Textura_preta.jpg");
}


void Desenha(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    // Desenhar os cursores (retângulos)
    glEnable(GL_TEXTURE_2D);

    // Desenhar o cursor 1
    glBindTexture(GL_TEXTURE_2D, texturaCursor1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(10.0f, windowHeight - cursor1Y1);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(10.0f, windowHeight - cursor1Y2);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(15.0f, windowHeight - cursor1Y2);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(15.0f, windowHeight - cursor1Y1);
    glEnd();

    // Desenhar o cursor 2
    glBindTexture(GL_TEXTURE_2D, texturaCursor2);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(windowWidth - 15.0f, windowHeight - cursor2Y1);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(windowWidth - 15.0f, windowHeight - cursor2Y2);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(windowWidth - 10.0f, windowHeight - cursor2Y2);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(windowWidth - 10.0f, windowHeight - cursor2Y1);
    glEnd();

    glDisable(GL_TEXTURE_2D);


    if (tempoDecorrido >= 30 && tempoDecorrido < 40){
        glBegin(GL_QUADS); // Barreira
        glColor3f(1.0f, 1.0f, 1.0f);  // Cor da Barreira
        glVertex2f((windowWidth/2) - 5, windowHeight - y1bar);
        glVertex2f((windowWidth/2) - 5, windowHeight - y2bar);
        glVertex2f((windowWidth/2) + 5, windowHeight - y2bar);
        glVertex2f((windowWidth/2) + 5, windowHeight - y1bar);
        glEnd();
    }


    // Desenhar o objeto principal (círculo)
    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
    {
        float theta = i * 3.1415926 / 180.0;
        float x = posX + rsize * cos(theta);
        float y = posY + rsize * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();

    // Desenha a pontuação na tela
    glColor3f(1.0f, 1.0f, 1.0f);
    int x1 = (windowWidth / 2) - 10;
    glRasterPos2f(x1, windowHeight - 20);
    char textoPontuacao[50];
    sprintf(textoPontuacao, "Placar: %d - %d", pontuacaoCursor1, pontuacaoCursor2);

    // Exibe cada caractere individualmente usando glutBitmapCharacter
    for (char *c = textoPontuacao; *c != '\0'; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    // Desenha o cronômetro
    time(&currentTime);
    tempoDecorrido = difftime(currentTime, startTime);
    /*glRasterPos2f(((windowWidth / 2) - 5), windowHeight - 40);
    char textoCronometro[50];
    sprintf(textoCronometro, "Tempo: %ds", tempoDecorrido);

    // Exibe cada caractere individualmente usando glutBitmapCharacter
    for (char *c = textoCronometro; *c != '\0'; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    */

    //Desenha efeito ativado
    if (tempoDecorrido >= 30 && tempoDecorrido < 40){
        glColor3f(1.0f, 1.0f, 1.0f);
        int x2 = (windowWidth / 2) - 20;
        glRasterPos2f(x2, windowHeight - 40);
        char textoEfeito[50];
        sprintf(textoEfeito, "Efeito Negativo Ativado");

        for (char *c = textoEfeito; *c != '\0'; ++c)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    if (jogoPausado) {
        glColor3f(1.0f, 1.0f, 1.0f);
        int xPause = (windowWidth / 2) - 30;
        glRasterPos2f(xPause, windowHeight / 2);
        char textoPause[] = "Jogo Pausado";

        for (char *c = textoPause; *c != '\0'; ++c)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

        glutSwapBuffers();
        return;
    }

    glutSwapBuffers();
}

void TrocarCoresETamanho()
{
    red = static_cast<GLfloat>(rand()) / RAND_MAX;
    green = static_cast<GLfloat>(rand()) / RAND_MAX;
    blue = static_cast<GLfloat>(rand()) / RAND_MAX;

    // Mudar o tamanho do objeto
    rsize = 5 + rand() % 10;

}

void RefletirCirculo(GLfloat *normal)
{
    GLfloat direcao[2] = {xstep, ystep};
    GLfloat reflexao[2];

    // Normaliza os vetores
    GLfloat normalizadoNormal[2] = {
        normal[0] / sqrt(normal[0] * normal[0] + normal[1] * normal[1]),
        normal[1] / sqrt(normal[0] * normal[0] + normal[1] * normal[1])
    };

    GLfloat normalizadoDirecao[2] = {
        direcao[0] / sqrt(direcao[0] * direcao[0] + direcao[1] * direcao[1]),
        direcao[1] / sqrt(direcao[0] * direcao[0] + direcao[1] * direcao[1])
    };

    // Calcula a reflexão
    reflexao[0] = normalizadoDirecao[0] - 2 * (normalizadoDirecao[0] * normalizadoNormal[0] + normalizadoDirecao[1] * normalizadoNormal[1]) * normalizadoNormal[0];
    reflexao[1] = normalizadoDirecao[1] - 2 * (normalizadoDirecao[0] * normalizadoNormal[0] + normalizadoDirecao[1] * normalizadoNormal[1]) * normalizadoNormal[1];

    // Mantém a velocidade constante multiplicando pelo tamanho desejado
    GLfloat velocidade = sqrt(xstep * xstep + ystep * ystep);
    xstep = reflexao[0] * velocidade;
    ystep = reflexao[1] * velocidade;

    // Se a bola estiver perto dos cursores, ajuste sua posição para evitar instabilidade
    if (posX - rsize < 15 && posX + rsize > 10 && posY - rsize < windowHeight - cursor1Y1 && posY + rsize > windowHeight - cursor1Y2)
    {
        posX = 15 + rsize;
    }

    if (posX - rsize < windowWidth - 10 && posX + rsize > windowWidth - 15 && posY - rsize < windowHeight - cursor2Y1 && posY + rsize > windowHeight - cursor2Y2)
    {
        posX = windowWidth - 15 - rsize;
    }
}


void Timer(int value)
{
    if (!jogoPausado) {
    // Inicia o cronômetro no primeiro frame
    if (value == 1)
        time(&startTime);

    // Atualiza o tempo atual a cada frame
    time(&currentTime);

    int tempoDecorrido = difftime(currentTime, startTime);

    // Verifica se passaram 30 segundos e reinicia o efeito negativo
    if (tempoDecorrido >= 30 && tempoDecorrido < 40)
    {
        if (!nerfAtivo)
        {
            nerfAtivo = true;
            // Lógica para aplicar o efeito negativo se necessário
        }
    }
    // Verifica se passaram 40 segundos e reinicia o ciclo
    else if (tempoDecorrido >= 40)
    {
        nerfAtivo = false;
        // Lógica para desfazer o efeito negativo se necessário
        time(&startTime); // Reinicia o cronômetro para começar um novo ciclo
    }

    if (posX > windowWidth - rsize)
    {
        // Bola atingiu a parede direita
        posX = windowWidth / 2;  // Retorna ao meio
        pontuacaoCursor1++;      // Incrementa a pontuação do cursor 1
        // Outras ações necessárias (como reiniciar a bola, etc.)
    }
    else if (posX < 0)
    {
        // Bola atingiu a parede esquerda
        posX = windowWidth / 2;  // Retorna ao meio
        pontuacaoCursor2++;      // Incrementa a pontuação do cursor 2
        // Outras ações necessárias (como reiniciar a bola, etc.)
    }

    if (posY > windowHeight - rsize || posY < 0)
    {
        GLfloat normal[2] = {0.0f, 1.0f};
        RefletirCirculo(normal);
        TrocarCoresETamanho();
    }

    // Testar colisão com o cursor 1
    if (posX - rsize < 15 && posX + rsize > 10 && posY - rsize < windowHeight - cursor1Y1 && posY + rsize > windowHeight - cursor1Y2)
    {
        GLfloat normal[2] = {1.0f, 0.0f};
        RefletirCirculo(normal);
        TrocarCoresETamanho();
    }

    // Testar colisão com o cursor 2
    if (posX - rsize < windowWidth - 10 && posX + rsize > windowWidth - 15 && posY - rsize < windowHeight - cursor2Y1 && posY + rsize > windowHeight - cursor2Y2)
    {
        GLfloat normal[2] = {-1.0f, 0.0f};
        RefletirCirculo(normal);
        TrocarCoresETamanho();
    }
    int x1bar = (windowWidth/2) - 5;
    int x2bar = (windowWidth/2) + 5;
    if (tempoDecorrido >= 30 && tempoDecorrido < 40){
        if (posX - rsize < x2bar && posX + rsize > x1bar && posY - rsize < windowHeight - y1bar && posY + rsize > windowHeight - y2bar)
        {
            GLfloat normal[2] = {-1.0f, 0.0f};
            RefletirCirculo(normal);
            TrocarCoresETamanho();
        }
    }
    if (posX > windowWidth - rsize)
        posX = windowWidth - rsize - 1;

    if (posY > windowHeight - rsize)
        posY = windowHeight - rsize - 1;

    posX += xstep;
    posY += ystep;
    }
    glutPostRedisplay();
    glutTimerFunc(33, Timer, 0);
}


void Inicializa(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Habilitando a luz ambiente
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    //Declarando valores para a luz ambiente
    float globalAmb[] = {1.f, 1.f, 1.f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

    // Defina as posições iniciais no centro da tela
    posX = windowWidth / 2;
    posY = windowHeight / 2;
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    if (h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w <= h)
    {
        windowHeight = 250.0f * h / w;
        windowWidth = 250.0f;
    }
    else
    {
        windowWidth = 250.0f * w / h;
        windowHeight = 250.0f;
    }

    gluOrtho2D(0.0f, windowWidth, 0.0f, windowHeight);

    // Atualize as posições iniciais ao centro da tela
    posX = windowWidth / 2;
    posY = windowHeight / 2;
}

void TeclasEspeciais(int key, int x, int y)
{
    if (jogoPausado)
        return;

    switch (key)
    {
    case GLUT_KEY_UP:
        if ((windowHeight - cursor2Y1) <= windowHeight && (windowHeight - cursor2Y2) <= windowHeight)
        {
            cursor2Y1 -= (nerfAtivo ? 1.0f : 5.0f);
            cursor2Y2 -= (nerfAtivo ? 1.0f : 5.0f);
        }
        break;
    case GLUT_KEY_DOWN:
        if (windowHeight - cursor2Y1 >= 0 && (windowHeight - cursor2Y2) >= 0)
        {
            cursor2Y1 += (nerfAtivo ? 1.0f : 5.0f);
            cursor2Y2 += (nerfAtivo ? 1.0f : 5.0f);
        }
        break;
    }
}

void Teclas(unsigned char key, int x, int y)
{
    if (jogoPausado && key == 'p') {
        jogoPausado = false;  // Se o jogo estiver pausado e a tecla 'p' for pressionada, retoma o jogo
        return;
    }

    if (jogoPausado)
        return;

    switch (key)
    {
    case 'w':
        if ((windowHeight - cursor1Y1) <= windowHeight && (windowHeight - cursor1Y2) <= windowHeight)
        {
            cursor1Y1 -= (nerfAtivo ? 1.0f : 5.0f);
            cursor1Y2 -= (nerfAtivo ? 1.0f : 5.0f);
        }
        break;
    case 's':
        if (windowHeight - cursor1Y1 >= 0 && (windowHeight - cursor1Y2) >= 0)
        {
            cursor1Y1 += (nerfAtivo ? 2.0f : 5.0f);
            cursor1Y2 += (nerfAtivo ? 2.0f : 5.0f);
        }
        break;
    case 'p':
        jogoPausado = !jogoPausado; // Inverte o estado de pausa ao pressionar a tecla 'p'
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Projeto prático - Daniel Luiz - Thiago Godinho");
    glutDisplayFunc(Desenha);
    CarregarTexturas();
    glutReshapeFunc(AlteraTamanhoJanela);
    glutTimerFunc(33, Timer, 1);
    glutSpecialFunc(TeclasEspeciais);
    glutKeyboardFunc(Teclas);
    Inicializa();
    srand(time(NULL));
    glutMainLoop();
    return 0;
}
