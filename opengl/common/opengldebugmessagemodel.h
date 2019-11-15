#ifndef OPENGLDEBUGMESSAGEMODEL_H
#define OPENGLDEBUGMESSAGEMODEL_H

#include <QAbstractTableModel>
#include <QOpenGLDebugMessage>

class OpenGLDebugMessageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit OpenGLDebugMessageModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QList<QOpenGLDebugMessage> messages() const;

public slots:
    void clearMessages();
    void setMessages(const QList<QOpenGLDebugMessage> &messages);
    void appendMessage(const QOpenGLDebugMessage &message);

private:
    QList<QOpenGLDebugMessage> m_messages;
};

#endif // OPENGLDEBUGMESSAGEMODEL_H
