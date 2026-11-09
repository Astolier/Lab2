#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class FindDialog;
class ReplaceDialog;
class AboutDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    bool on_actionSave_triggered();
    bool on_actionSaveAs_triggered();
    void on_actionExit_triggered();

    // 格式设置
    void on_actionWrap_triggered(bool checked);
    void on_actionLineNum_triggered(bool checked);
    void on_actionWordType_triggered();
    void on_actionWordColor_triggered();
    void on_actionWordBackGround_triggered();
    void on_actionToolBackGround_triggered();

    // 查看设置
    void on_actionTool_triggered(bool checked);
    void on_actionStatus_triggered(bool checked);

    // 查找与替换
    void on_actionFind_triggered();
    void on_actionReplace_triggered();
    void doFindNext(const QString &text, Qt::CaseSensitivity cs, bool forward);
    void doReplaceOne(const QString &findText, const QString &replaceText, Qt::CaseSensitivity cs, bool forward);
    void doReplaceAll(const QString &findText, const QString &replaceText, Qt::CaseSensitivity cs);

    // 帮助
    void on_actionHelp_triggered();

    // 状态与界面联动
    void updateStatusBar();
    void updateTitle(bool modified);
    void updateClipboardPasteState();

private:
    void initStatusBar();
    void initConnections();
    bool maybeSave();
    bool loadFile(const QString &fileName);
    bool saveFileToPath(const QString &fileName);
    void setCurrentFile(const QString &fileName);

private:
    Ui::MainWindow *ui;

    // 状态栏组件
    QLabel *statusInfoLabel;
    QLabel *statusUserLabel; // 右下角开发者信息标签

    // 对话框指针
    FindDialog *findDlg;
    ReplaceDialog *replaceDlg;

    // 当前文件信息
    QString curFile;
    bool isUntitled;
};

#endif // MAINWINDOW_H
