#include "gameopenglwidget.h"

gameOpenGLWidget::gameOpenGLWidget(QWidget *parent) : QGLWidget (parent)
{

}

void gameOpenGLWidget::initializeGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.9098f, 0.9098f, 1.0f, 0);

    glShadeModel(GL_SMOOTH);

    updateBuff();
}

void gameOpenGLWidget::resizeGL(int w, int h)
{

}

void gameOpenGLWidget::paintGL()
{

}

void gameOpenGLWidget::drawUnits(Stone** in)
{
    drawBoard();

    for (int i=0; i<gameConstant::BOARD_SIZE; i++)
    {
        for (int j=0; j<gameConstant::BOARD_SIZE; j++)
        {
            if (in[i][j].isValidStone())
            {
                if (in[i][j].getFraction() == stoneColors::BLACK)
                {
                    glColor3f(0.0f, 0.0f, 0.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }

                glRectf(-1.0f+0.07f+0.1f*i, 1.0f-0.07f-0.1f*j, -1.0f+0.07f+0.1f*i+0.06f, 1.0f-0.07f-0.1f*j-0.06f);
            }
            else
            {
                continue;
            }
        }
    }

    updateBuff();
}

void gameOpenGLWidget::test()
{
// drawBoard();
// updateBuff();

    drawUnits(0);
}

void gameOpenGLWidget::drawBoard()
{
    glColor3f(0.8275f, 0.6392f, 0.0f);
    glRectf(-1.0f, -1.0f, 1.0f, 1.0f);

    glColor3b(0, 0, 0);

    for (int i=0; i<19; i++)
    {
        glRectf(-1.0f+0.1f+0.1f*i,-1.0f,-1.0f+0.1f+0.1f*i+0.005f, 1.0);
        glRectf(-1.0f, -1.0f+0.1f+0.1f*i, 1.0f, -1.0f+0.1f+0.1f*i+0.005f);
    }
}

void gameOpenGLWidget::updateBuff()
{
    glSwapAPPLE();
}
