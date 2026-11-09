#include "replacedialog.h"
#include "ui_replacedialog.h"
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/change_circle_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.png"));
    setFixedSize(this->width(), this->height());

    connect(ui->lineEditFind, &QLineEdit::returnPressed, this, &ReplaceDialog::on_btnFindNext_clicked);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

QString ReplaceDialog::getFindText() const
{
    return ui->lineEditFind->text();
}

void ReplaceDialog::setFindText(const QString &text)
{
    ui->lineEditFind->setText(text);
    ui->lineEditFind->selectAll();
}

void ReplaceDialog::on_btnFindNext_clicked()
{
    QString findText = ui->lineEditFind->text();
    if (findText.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }

    Qt::CaseSensitivity cs = ui->chkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool forward = ui->radioDown->isChecked();

    emit findNext(findText, cs, forward);
}

void ReplaceDialog::on_btnReplace_clicked()
{
    QString findText = ui->lineEditFind->text();
    if (findText.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }

    QString replaceText = ui->lineEditReplace->text();
    Qt::CaseSensitivity cs = ui->chkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool forward = ui->radioDown->isChecked();

    emit replaceOne(findText, replaceText, cs, forward);
}

void ReplaceDialog::on_btnReplaceAll_clicked()
{
    QString findText = ui->lineEditFind->text();
    if (findText.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }

    QString replaceText = ui->lineEditReplace->text();
    Qt::CaseSensitivity cs = ui->chkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    emit replaceAll(findText, replaceText, cs);
}

void ReplaceDialog::on_btnCancel_clicked()
{
    close();
}
