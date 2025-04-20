#include "chatbubble.h"

ChatBubble::ChatBubble(const QString &text, bool isOutgoing, QWidget *parent) : QWidget(parent){
    setStyleSheet(isOutgoing ?
                      "font: 15px; color: black; background: #DCF8C6; border-radius: 10px; padding: 8px;" :
                      "font: 15px; color: black; background: white; border-radius: 10px; padding: 8px;"
                  );

    QLabel *messageLabel = new QLabel(text, this);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageLabel->setMaximumWidth(300);
    messageLabel->adjustSize();

    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm"), this);
    timeLabel->setStyleSheet("color:#666; font-size: 8px;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(messageLabel);
    layout->addWidget(timeLabel, 0, Qt::AlignRight);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    if(isOutgoing) {
        layout->setAlignment(Qt::AlignRight);
    } else {
        layout->setAlignment(Qt::AlignLeft);
    }
}

ChatBubble::~ChatBubble() {}
