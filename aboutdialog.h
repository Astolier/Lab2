#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

    // 设置开发者相关信息的接口（亦可在代码直接修改）
    void setDeveloperInfo(const QString &developer, const QString &studentId, const QString &email);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
