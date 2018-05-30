#ifndef GAMEOPENGLWIDGET_H
#define GAMEOPENGLWIDGET_H

#include <QGLWidget>
#include <globals.h>
#include <stone.h>

class gameOpenGLWidget : public QGLWidget
{
public:
    gameOpenGLWidget(QWidget *parent);

    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    void drawUnits(Stone** in);

    void test();

private:

    void drawBoard();

    void updateBuff();
};

#endif // GAMEOPENGLWIDGET_H
