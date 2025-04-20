#ifndef CHATBUBBLE_H
#define CHATBUBBLE_H

#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QRect>

class ChatBubble : public QWidget {
    Q_OBJECT
public:
    ChatBubble(const QString &text, bool isOutgoing, QWidget *parent = nullptr);
    ~ChatBubble();
};

#endif // CHATBUBBLE_H
