#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE
class Button;


class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void clearAll();
    void close();
    void Info();
    void info();
    void changeLanguage();
private:
    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);
    double sumSoFar;
    double factorSoFar;
    int Language;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;
    void keyPressEvent(QKeyEvent* pe);
    QLineEdit *display;

    static const int NumDigitButtons = 10;
    Button *digitButtons[NumDigitButtons];

    Button *pointButton;
    Button *changeSignButton;

    Button *clearAllButton;

    Button *divisionButton;
    Button *timesButton;
    Button *minusButton;
    Button *plusButton;

    Button *squareRootButton;
    Button *equalButton;
};


#endif
