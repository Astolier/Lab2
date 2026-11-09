#include "finddialog.h"
#include "ui_finddialog.h"
#include <QMessageBox>

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/search_24dp_1F1F1F_FILL0_wght400_GRAD0_opsz24.png"));
    setFixedSize(this->width(), this->height());

    connect(ui->lineEditFind, &QLineEdit::returnPressed, this, &FindDialog::on_btnFindNext_clicked);
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::getFindText() const
{
    return ui->lineEditFind->text();
}

void FindDialog::setFindText(const QString &text)
{
    ui->lineEditFind->setText(text);
    ui->lineEditFind->selectAll();
}

void FindDialog::on_btnFindNext_clicked()
{
    QString text = ui->lineEditFind->text();
    if (text.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("查找内容不能为空！"));
        return;
    }

    Qt::CaseSensitivity cs = ui->chkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool forward = ui->radioDown->isChecked();

    emit findNext(text, cs, forward);
}

void FindDialog::on_btnCancel_clicked()
{
    close();
}
