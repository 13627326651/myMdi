#ifndef MDICHILD_H
#define MDICHILD_H

#include <QWidget>
#include <QTextEdit>
class MdiChild : public QTextEdit
{
    Q_OBJECT
public:
    explicit MdiChild(QWidget*parent=0);
    void newFile();//新建
    bool loadFile(const QString &fileName);//加载
    bool save();//保存
    bool saveAs();//另存操作
    bool saveFile(const QString &fileName);//保存文件
    QString userFriendlyCurrentFile();//提取文件名
    QString currentFile(){return curFile;}//返回当前文件路径

protected:
    void closeEvent(QCloseEvent*event);//关闭事件
    void contextMenuEvent(QContextMenuEvent *e);//右键菜单事件

private slots:
    void documentWasModified();//现实更改状态

private:
    bool maybeSave();//是否需要保存
    void setCurrentFile(const QString &fileName);
    QString curFile;
    bool isUntitled;
};

#endif // MDICHILD_H
