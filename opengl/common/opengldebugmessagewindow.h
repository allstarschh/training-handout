#ifndef OPENGLDEBUGMESSAGEWINDOW_H
#define OPENGLDEBUGMESSAGEWINDOW_H

#include <QDialog>

class OpenGLDebugMessageModel;
class QPushButton;
class QTreeView;

class OpenGLDebugMessageWindow : public QDialog
{
    Q_OBJECT
public:
    explicit OpenGLDebugMessageWindow(QWidget *parent = nullptr);
    void setModel(OpenGLDebugMessageModel *model);
private:
    QTreeView *m_view;
    QPushButton *m_clearButton;
    OpenGLDebugMessageModel *m_model;
};

#endif // OPENGLDEBUGMESSAGEWINDOW_H
