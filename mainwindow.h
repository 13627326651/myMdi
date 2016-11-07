#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class MdiChild;
class QMdiSubWindow;
class QSignalMapper;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_actionNew_triggered();
    void updateMenus();
    MdiChild *createMdiChild();
    void setActiveSubWindow(QWidget *window);//设置活动子窗口
    void on_actionOpen_triggered();
    void updateWindowMenu();    //更新窗口菜单

    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();

    void showTextRowAndCol();//显示行号和列号

private:
    Ui::MainWindow *ui;
    QAction *actionSeparator;
    QSignalMapper *windowMapper;    //信号映射器

    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);//查找子窗口

    void readSettings();//读取窗口设置
    void writeSettings();//写入窗口设置
    void initWindow();//初始化窗口


};

#endif // MAINWINDOW_H
