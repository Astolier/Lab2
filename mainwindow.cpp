#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QClipboard>
#include <QTextStream>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , statusInfoLabel(nullptr)
    , statusUserLabel(nullptr)
    , findDlg(nullptr)
    , replaceDlg(nullptr)
    , curFile(tr("新建文本文件"))
    , isUntitled(true)
{
    ui->setupUi(this);

    // 设置窗口图标
    setWindowIcon(QIcon(":/img/note.png"));

    // 初始化状态栏
    initStatusBar();

    // 初始化信号槽连接
    initConnections();

    // 初始化当前文件状态
    setCurrentFile("");
    ui->textEdit->document()->setModified(false);

    // 初始 Action 使能状态联动
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    updateClipboardPasteState();

    // 默认开启自动换行与显示行号
    ui->actionWrap->setChecked(true);
    ui->actionLineNum->setChecked(true);
    ui->actionTool->setChecked(true);
    ui->actionStatus->setChecked(true);

    // 初始更新状态栏信息
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStatusBar()
{
    statusInfoLabel = new QLabel(this);
    statusUserLabel = new QLabel(this);

    statusUserLabel->setText(tr("温庆华"));

    // 将标签加入状态栏右侧
    ui->statusbar->addPermanentWidget(statusInfoLabel);
    ui->statusbar->addPermanentWidget(statusUserLabel);
}

void MainWindow::initConnections()
{
    // 编辑器基本操作
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QPlainTextEdit::paste);
    connect(ui->action_SelectAll, &QAction::triggered, ui->textEdit, &QPlainTextEdit::selectAll);

    // 使能状态联动
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::redoAvailable, ui->actionRedo, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updateClipboardPasteState);

    // 状态栏与标题联动
    connect(ui->textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    connect(ui->textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::updateStatusBar);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged, this, &MainWindow::updateTitle);
}

void MainWindow::updateClipboardPasteState()
{
    const QString text = QApplication::clipboard()->text();
    ui->actionPaste->setEnabled(!text.isEmpty());
}

void MainWindow::updateStatusBar()
{
    int length = ui->textEdit->toPlainText().length();
    int lines = ui->textEdit->document()->blockCount();
    QTextCursor cursor = ui->textEdit->textCursor();
    int ln = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;

    QString info = QString("length: %1   lines: %2   Ln: %3   Col: %4   ")
                       .arg(length)
                       .arg(lines)
                       .arg(ln)
                       .arg(col);
    statusInfoLabel->setText(info);
}

void MainWindow::updateTitle(bool modified)
{
    QString showName = isUntitled ? tr("新建文本文件") : QFileInfo(curFile).fileName();
    if (modified) {
        setWindowTitle(QString("* %1 - 编辑器").arg(showName));
    } else {
        setWindowTitle(QString("%1 - 编辑器").arg(showName));
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    isUntitled = fileName.isEmpty();
    updateTitle(ui->textEdit->document()->isModified());
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified()) {
        return true;
    }

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        tr("记事本"),
        tr("是否将更改保存到 %1？").arg(isUntitled ? tr("新建文本文件") : curFile),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (ret == QMessageBox::Save) {
        return on_actionSave_triggered();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

// ----------------- 文件操作 -----------------
void MainWindow::on_actionNew_triggered()
{
    if (maybeSave()) {
        ui->textEdit->clear();
        setCurrentFile("");
        ui->textEdit->document()->setModified(false);
        updateStatusBar();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("打开文件"),
            QString(),
            tr("文本文件 (*.txt);;C++源文件 (*.cpp *.h);;所有文件 (*.*)"));
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法读取文件 %1:\n%2.").arg(fileName, file.errorString()));
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    ui->textEdit->document()->setModified(false);
    updateStatusBar();
    return true;
}

bool MainWindow::on_actionSave_triggered()
{
    if (isUntitled) {
        return on_actionSaveAs_triggered();
    } else {
        return saveFileToPath(curFile);
    }
}

bool MainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("另存为"),
        isUntitled ? tr("新建文本文件.txt") : curFile,
        tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (fileName.isEmpty()) {
        return false;
    }

    return saveFileToPath(fileName);
}

bool MainWindow::saveFileToPath(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法写入文件 %1:\n%2.").arg(fileName, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    ui->textEdit->document()->setModified(false);
    return true;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

// ----------------- 格式设置 -----------------
void MainWindow::on_actionWrap_triggered(bool checked)
{
    ui->textEdit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void MainWindow::on_actionLineNum_triggered(bool checked)
{
    ui->textEdit->setLineNumberVisible(checked);
}

void MainWindow::on_actionWordType_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, ui->textEdit->font(), this, tr("选择字体"));
    if (ok) {
        ui->textEdit->setFont(font);
    }
}

void MainWindow::on_actionWordColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, tr("选择字体颜色"));
    if (color.isValid()) {
        QTextCursor cursor = ui->textEdit->textCursor();
        if (cursor.hasSelection()) {
            QTextCharFormat fmt;
            fmt.setForeground(color);
            cursor.mergeCharFormat(fmt);
        } else {
            // 未选中文本则改变编辑器默认文本颜色
            QPalette p = ui->textEdit->palette();
            p.setColor(QPalette::Text, color);
            ui->textEdit->setPalette(p);
        }
    }
}

void MainWindow::on_actionWordBackGround_triggered()
{
    QColor color = QColorDialog::getColor(Qt::yellow, this, tr("选择字体背景色"));
    if (color.isValid()) {
        QTextCursor cursor = ui->textEdit->textCursor();
        if (cursor.hasSelection()) {
            QTextCharFormat fmt;
            fmt.setBackground(color);
            cursor.mergeCharFormat(fmt);
        }
    }
}

void MainWindow::on_actionToolBackGround_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("选择编辑器背景色"));
    if (color.isValid()) {
        QPalette p = ui->textEdit->palette();
        p.setColor(QPalette::Base, color);
        ui->textEdit->setPalette(p);
    }
}

// ----------------- 查看设置 -----------------
void MainWindow::on_actionTool_triggered(bool checked)
{
    ui->toolBar->setVisible(checked);
}

void MainWindow::on_actionStatus_triggered(bool checked)
{
    ui->statusbar->setVisible(checked);
}

// ----------------- 查找与替换 -----------------
void MainWindow::on_actionFind_triggered()
{
    if (!findDlg) {
        findDlg = new FindDialog(this);
        connect(findDlg, &FindDialog::findNext, this, &MainWindow::doFindNext);
    }
    // 若当前有选中文本，带入查找框
    QTextCursor cursor = ui->textEdit->textCursor();
    if (cursor.hasSelection()) {
        findDlg->setFindText(cursor.selectedText());
    }
    findDlg->show();
    findDlg->raise();
    findDlg->activateWindow();
}

void MainWindow::doFindNext(const QString &text, Qt::CaseSensitivity cs, bool forward)
{
    QTextDocument::FindFlags flags;
    if (cs == Qt::CaseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }

    bool found = ui->textEdit->find(text, flags);
    if (!found) {
        // 如果到了文档末尾，可循环或提示
        QMessageBox::information(this, tr("查找"), tr("找不到 \"%1\"").arg(text));
    }
}

void MainWindow::on_actionReplace_triggered()
{
    if (!replaceDlg) {
        replaceDlg = new ReplaceDialog(this);
        connect(replaceDlg, &ReplaceDialog::findNext, this, &MainWindow::doFindNext);
        connect(replaceDlg, &ReplaceDialog::replaceOne, this, &MainWindow::doReplaceOne);
        connect(replaceDlg, &ReplaceDialog::replaceAll, this, &MainWindow::doReplaceAll);
    }
    QTextCursor cursor = ui->textEdit->textCursor();
    if (cursor.hasSelection()) {
        replaceDlg->setFindText(cursor.selectedText());
    }
    replaceDlg->show();
    replaceDlg->raise();
    replaceDlg->activateWindow();
}

void MainWindow::doReplaceOne(const QString &findText, const QString &replaceText, Qt::CaseSensitivity cs, bool forward)
{
    QTextDocument::FindFlags flags;
    if (cs == Qt::CaseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }

    QTextCursor cursor = ui->textEdit->textCursor();
    QString selected = cursor.selectedText();

    bool isMatch = false;
    if (cs == Qt::CaseSensitive) {
        isMatch = (selected == findText);
    } else {
        isMatch = (selected.compare(findText, Qt::CaseInsensitive) == 0);
    }

    if (isMatch) {
        // 当前选中的正是目标内容，执行替换
        cursor.insertText(replaceText);
        // 替换成功后，尝试定位下一个匹配项；若后面已无匹配项，保留光标在当前替换位置，不弹窗误导用户
        ui->textEdit->find(findText, flags);
    } else {
        // 当前未选中匹配目标，先查找定位到下一个目标
        bool found = ui->textEdit->find(findText, flags);
        if (!found) {
            QMessageBox::information(this, tr("替换"), tr("找不到 \"%1\"").arg(findText));
        }
    }
}

void MainWindow::doReplaceAll(const QString &findText, const QString &replaceText, Qt::CaseSensitivity cs)
{
    QTextDocument::FindFlags flags;
    if (cs == Qt::CaseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    // 将光标移至文档起点开始全部替换
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->textEdit->setTextCursor(cursor);

    int count = 0;
    cursor.beginEditBlock();
    while (ui->textEdit->find(findText, flags)) {
        ui->textEdit->textCursor().insertText(replaceText);
        ++count;
    }
    cursor.endEditBlock();

    if (count > 0) {
        QMessageBox::information(this, tr("全部替换"), tr("已完成替换，共替换了 %1 处。").arg(count));
    } else {
        QMessageBox::information(this, tr("替换"), tr("找不到 \"%1\"").arg(findText));
    }
}

// ----------------- 帮助 -----------------
void MainWindow::on_actionHelp_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}
