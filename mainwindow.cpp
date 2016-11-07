#include "mainwindow.h"
#include <QLabel>
#include <QSignalMapper>
#include <QFileDialog>
#include "mdichild.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    actionSeparator=new QAction(this);
    actionSeparator->setSeparator(true);
    updateMenus();
    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateMenus()));

    //创建信号映射器
    windowMapper=new QSignalMapper(this);
    //映射器重新发射信号,根据信号设置活动窗口
    connect(windowMapper,SIGNAL(mapped(QWidget*)),this,SLOT(setActiveSubWindow(QWidget*)));
    //更新窗口菜单，并且设置窗口菜单将要显示的时候更新窗口菜单
    updateWindowMenu();
    connect(ui->menuW,SIGNAL(aboutToShow()),this,SLOT(updateWindowMenu()));

    readSettings();
    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //先执行多文档区域的关闭操作
    ui->mdiArea->closeAllSubWindows();
    //如果还有窗口没有关闭则忽略该事件
    if(ui->mdiArea->currentSubWindow())
        event->ignore();
    else
    {
        writeSettings();
        event->accept();
    }
}

void MainWindow::on_actionNew_triggered()
{
    MdiChild *child=createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::updateMenus()
{
    //设置是否有活动窗口来设置各个动作是否可用
    bool hasMdiChild=(activeMdiChild()!=0);
    ui->actionSave->setEnabled(hasMdiChild);
    ui->actionSaveAs->setEnabled(hasMdiChild);
    ui->actionPaste->setEnabled(hasMdiChild);
    ui->actionClose->setEnabled(hasMdiChild);
    ui->actionCloseAll->setEnabled(hasMdiChild);
    ui->actionTile->setEnabled(hasMdiChild);
    ui->actionCascade->setEnabled(hasMdiChild);
    ui->actionNext->setEnabled(hasMdiChild);
    ui->actionPrevious->setEnabled(hasMdiChild);
    //设置间隔是否显示
    actionSeparator->setVisible(hasMdiChild);
    //有活动窗口且有被选择的文本,剪切复制才可用
    bool hasSelection=(activeMdiChild()&&activeMdiChild()->textCursor().hasSelection());
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    //有活动窗口且文档有撤销操作时撤销动作可用
    ui->actionUndo->setEnabled(activeMdiChild()&&activeMdiChild()->document()->isRedoAvailable());
    //有活动窗口且文档有恢复操作时恢复动作可用
    ui->actionRedo->setEnabled(activeMdiChild()&&activeMdiChild()->document()->isRedoAvailable());
}

MdiChild *MainWindow::createMdiChild()
{
    //创建MdiChild部件
    MdiChild *child=new MdiChild;
    //向多文档区域添加子窗口，child为中心部件
    ui->mdiArea->addSubWindow(child);
    //根据QTextEdit类的是否可以复制信号设置剪切复制动作是否可用
    connect(child,SIGNAL(copyAvailable(bool)),ui->actionCut,SLOT(setEnabled(bool)));
    connect(child,SIGNAL(copyAvailable(bool)),ui->actionCopy,SLOT(setEnabled(bool)));
    //根据QTextDocument类的是否可以撤销恢复信号设置撤销恢复动作是否可以用
    connect(child->document(),SIGNAL(undoAvailable(bool)),ui->actionUndo,SLOT(setEnabled(bool)));
    connect(child->document(),SIGNAL(redoAvailable(bool)),ui->actionRedo,SLOT(setEnabled(bool)));

    connect(child,SIGNAL(cursorPositionChanged()),this,SLOT(showTextRowAndCol()));
    return child;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if(!window)
    {
        return;
    }
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));

}

MdiChild *MainWindow::activeMdiChild()
{
    if(QMdiSubWindow*activeSubWindow=ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild*>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath=QFileInfo(fileName).canonicalFilePath();
    //利用foreach语句遍历子窗口列表，如果其文件路径和要查找的路径相同，则返回该窗口
    foreach(QMdiSubWindow *window,ui->mdiArea->subWindowList())
    {
        MdiChild *mdiChild=qobject_cast<MdiChild*>(window->widget());
        if(mdiChild->currentFile()==canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::readSettings()
{
    QSettings settings("xgp","myMdi");
    QPoint pos=settings.value("pos",QPoint(200,200)).toPoint();
    QSize size=settings.value("size",QSize(400,400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("xgp","myMdi");
    //写入位置和大小信息
    settings.setValue("pos",pos());
    settings.setValue("size",size());
}

void MainWindow::initWindow()
{
    setWindowTitle(tr("多文档编辑器"));
    //在工具栏上右击时，可以关闭工具栏
    ui->mainToolBar->setWindowTitle(tr("工具栏"));

    //多文档区域的内容超出可视区域后，出现滚动条
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->statusBar->showMessage(tr("欢迎使用多文档编辑器"));
    QLabel *label=new QLabel(this);
    label->setFrameStyle(QFrame::Box|QFrame::Sunken);
    label->setText(
                tr("<a href=\"http://www.yafeilinux.com\">yafeilinux.com</a>"));
    //标签文本为富文本
    label->setTextFormat(Qt::RichText);
    //可以打开外部链接
    label->setOpenExternalLinks(true);
    ui->statusBar->addPermanentWidget(label);
    ui->actionNew->setStatusTip(tr("创建一个文件"));

}

void MainWindow::updateWindowMenu()
{
    //先清空菜单再添加各个菜单动作
    ui->menuW->clear();
    ui->menuW->addAction(ui->actionClose);
    ui->menuW->addAction(ui->actionCloseAll);
    ui->menuW->addSeparator();
    ui->menuW->addAction(ui->actionTile);
    ui->menuW->addAction(ui->actionCascade);
    ui->menuW->addSeparator();
    ui->menuW->addAction(ui->actionNext);
    ui->menuW->addAction(ui->actionPrevious);
    ui->menuW->addAction(actionSeparator);
    //如果有活动窗口则显示间隔器
    QList<QMdiSubWindow*>windows=ui->mdiArea->subWindowList();
    actionSeparator->setVisible(!windows.isEmpty());
    //遍历各个子窗口
    for(int i=0;i<windows.size();++i)
    {
        MdiChild *child=qobject_cast<MdiChild*>(windows.at(i)->widget());
        QString text;
        if(i<9)
        {
            text=tr("&%1 %2").arg(i+1).arg(child->userFriendlyCurrentFile());

        }else
        {
            text=tr("%1 %2").arg(i+1).arg(child->userFriendlyCurrentFile());

        }
        //添加动作菜单，设置动作菜单可以选择
        QAction *action=ui->menuW->addAction(text);
        action->setCheckable(true);
        //设置当前活动窗口为选中状态
        action->setChecked(child==activeMdiChild());
        //关联动作的触发信号到信号映射器的map()槽，这个槽会发射mapped()信号
        connect(action,SIGNAL(triggered(bool)),windowMapper,SLOT(map()));
        //将动作与相应的窗口部件进行映射
        //在发射mapped()信号时就会以这个窗口部件作为参数
        windowMapper->setMapping(action,windows.at(i));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    //获取文件路径
    QString fileName=QFileDialog::getOpenFileName(this);
    //如果路径不为空，则查看该文件是否已经打开
    if(!fileName.isEmpty())
    {
        QMdiSubWindow *existing=findMdiChild(fileName);
        if(existing)
        {
            ui->mdiArea->setActiveSubWindow(existing);
            return;
        }
    }
    //如果没有打开,则新建子窗口
    MdiChild *child=createMdiChild();
    if(child->loadFile(fileName))
    {
        ui->statusBar->showMessage(tr("打开文件成功"),2000);
        child->show();
    }else
    {
        child->close();
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(activeMdiChild()&&activeMdiChild()->save())
        ui->statusBar->showMessage(tr("文件保存成功"),2000);
}

void MainWindow::on_actionSaveAs_triggered()
{
    if(activeMdiChild()&&activeMdiChild()->saveAs())
        ui->statusBar->showMessage(tr("文件保存成功"),2000);
}

void MainWindow::on_actionUndo_triggered()
{
    if(activeMdiChild())activeMdiChild()->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    if(activeMdiChild())activeMdiChild()->redo();
}

void MainWindow::on_actionCut_triggered()
{
    if(activeMdiChild())activeMdiChild()->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    if(activeMdiChild())activeMdiChild()->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    if(activeMdiChild())activeMdiChild()->paste();
}

void MainWindow::on_actionClose_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::showTextRowAndCol()
{
    //如果有活动窗口则显示其中的光标的位置
    if(activeMdiChild())
    {
        int rowNum=activeMdiChild()->textCursor().blockNumber()+1;
        int colNum=activeMdiChild()->textCursor().columnNumber()+1;

        ui->statusBar->showMessage(tr("%1行 %2列").arg(rowNum).arg(colNum),2000);
    }
}
