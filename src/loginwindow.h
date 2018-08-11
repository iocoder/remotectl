#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_radioNew_toggled(bool checked);

    void on_radioOpen_toggled(bool checked);

    void on_btnQuit_pressed();

    void on_btnLogin_pressed();

    void on_lstOpen_currentRowChanged(int currentRow);

    void on_btnDelete_pressed();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
