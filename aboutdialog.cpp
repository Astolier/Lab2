#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // 固定窗口大小，去掉问号帮助按钮
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(this->width(), this->height());

    // 点击 OK 按钮关闭对话框
    connect(ui->okBtn, &QPushButton::clicked, this, &QDialog::accept);

    ui->labelDev->setText(tr("开发者：温庆华"));
    ui->labelId->setText(tr("学号：2024414290330"));
    ui->labelEmail->setText(tr("email：3227416078@qq.com"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setDeveloperInfo(const QString &developer, const QString &studentId, const QString &email)
{
    ui->labelDev->setText(tr("开发者：%1").arg(developer));
    ui->labelId->setText(tr("学号：%1").arg(studentId));
    ui->labelEmail->setText(tr("email：%1").arg(email));
}
