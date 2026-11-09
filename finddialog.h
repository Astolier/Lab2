#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    QString getFindText() const;
    void setFindText(const QString &text);

signals:
    void findNext(const QString &text, Qt::CaseSensitivity cs, bool forward);

private slots:
    void on_btnFindNext_clicked();
    void on_btnCancel_clicked();

private:
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H
