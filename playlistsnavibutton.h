#ifndef PLAYLISTSNAVIBUTTON_H
#define PLAYLISTSNAVIBUTTON_H

#include <QPushButton>
#include <QMenu>
#include <QMouseEvent>

class PlaylistsNaviButton : public QPushButton
{
    Q_OBJECT
public:
    PlaylistsNaviButton(const QString& text, QWidget* parent = nullptr) : QPushButton(text, parent) {}

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::RightButton) {
            // 创建右键菜单
            QMenu contextMenu(this);
            QAction *action1 = contextMenu.addAction("操作 1");
            QAction *action2 = contextMenu.addAction("操作 2");

            QAction *selectedAction = contextMenu.exec(event->globalPosition().toPoint());
            if (selectedAction == action1) {
                qDebug() << "右键点击 操作 1";
            } else if (selectedAction == action2) {
                qDebug() << "右键点击 操作 2";
            }
        } else {
            // 处理默认行为（左键点击等）
            QPushButton::mousePressEvent(event);
        }
    }
};


#endif // PLAYLISTSNAVIBUTTON_H
