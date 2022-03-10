#include "calculator.h"
#include "ui_calculator.h"
#include "button.h"


#include <QGridLayout>
#include <QLineEdit>
#include <QtMath>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>


Calculator::Calculator(QWidget *parent)
    : QWidget(parent), sumSoFar(0.0)
    , factorSoFar(0.0), waitingForOperand(true)
{
     QGridLayout *mainLayout = new QGridLayout;

    QMenu *fileMenu = new QMenu("Функционал");
    QMenu *helpMenu = new QMenu("Помощь");

    QMenuBar * mainMenu = new QMenuBar;
    mainMenu->addMenu(fileMenu);
    mainMenu->addMenu(helpMenu);
    mainLayout->setMenuBar(mainMenu);


    fileMenu->addAction("Выход", this, SLOT(close()));
    fileMenu->addAction("Смена языка", this, SLOT(changeLanguage()));
    fileMenu->addAction("Очистка", this, SLOT(clearAll()));
    helpMenu->addAction("Информация о версии", this, SLOT(Info()));
    helpMenu->addAction("Как пользоваться", this, SLOT(info()));

    display = new QLineEdit("0");

    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    for (int i = 0; i < NumDigitButtons; ++i)
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));

    pointButton = createButton(tr("."), SLOT(pointClicked()));
    changeSignButton = createButton(tr("\302\261"), SLOT(changeSignClicked()));

    clearAllButton = createButton(tr("Очистить"), SLOT(clearAll()));

    divisionButton = createButton(tr("\303\267"), SLOT(multiplicativeOperatorClicked()));
    timesButton = createButton(tr("\303\227"), SLOT(multiplicativeOperatorClicked()));
    minusButton = createButton(tr("-"), SLOT(additiveOperatorClicked()));
    plusButton = createButton(tr("+"), SLOT(additiveOperatorClicked()));

    squareRootButton = createButton(tr("Кв.кор"), SLOT(unaryOperatorClicked()));
    equalButton = createButton(tr("="), SLOT(equalClicked()));

   // QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(display, 0, 0, 1, 6);
    mainLayout->addWidget(clearAllButton, 2, 4, 1, 2);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 5, 1);
    mainLayout->addWidget(pointButton, 5, 2);
    mainLayout->addWidget(changeSignButton, 5, 3);

    mainLayout->addWidget(plusButton, 3, 4);
    mainLayout->addWidget(minusButton, 3, 5);
    mainLayout->addWidget(timesButton, 4, 4);
    mainLayout->addWidget(divisionButton, 4, 5);

    mainLayout->addWidget(equalButton, 5, 4);

    mainLayout->addWidget(squareRootButton, 5, 5);
    setLayout(mainLayout);

    setWindowTitle(tr("Калькулятор"));
    Language=1;
}

void Calculator::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if (display->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}

void Calculator::unaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = std::sqrt(operand);
    }
    display->setText(QString::number(result));
    waitingForOperand = true;
}

void Calculator::additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if (!clickedButton)
      return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::multiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    if (!clickedButton)
      return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::equalClicked()
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}

void Calculator::pointClicked()
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains('.'))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}

void Calculator::close()
{
    close();
}

void Calculator::changeLanguage()
{
    if (Language==1)
    {
            Language=2;
            setWindowTitle(tr("Calculator"));
            clearAllButton->setText("Clear all");
            squareRootButton->setText("sqrt");

    }
 else
    if (Language==2)
    {
            Language=1;
            setWindowTitle(tr("Калькулятор"));
            clearAllButton->setText("Очистить");
            squareRootButton->setText("кв.кор");
    }
}
void Calculator::Info()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Версия");
    if (Language==1)
    msgBox.setText("Калькулятор выполнен студенткой группы 3-41 Адамовой Александрой в 2022 \n "
                   "По всем вопросам: adamova.01@mail.ru");
    else     msgBox.setText("The calculator was made by a student of group 3-41 Adamova Alexandra in 2022 \n "
                            "For all questions: adamova.01@mail.ru");

    msgBox.exec();
}

void Calculator::info()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Использование");
    msgBox.setIcon(QMessageBox::Question);
     if (Language==1)
    msgBox.setText("Перед вами простейший калькулятор \n"
                   "С его помощью Вы можете производить вычисления  \n"
                   "1. Помните, что при попытке деления на 0 вы столкнетесь с ошибкой  \n"
                   "2. Калькулятор - не человек, вычисления он производит с помощью тех.средств,  \n"
                   "поэтому если Вы считаете, что 2+2=5, то нам не по пути \n"
                   "3. Не судите строго, я старалась");
     else     msgBox.setText("Here is a simple calculator \n"
                             "With its help you can perform calculations \n"
                             "1. Remember that when you try to divide by 0, you will encounter the error \n"
                             "2. The calculator is not a person, it performs calculations using those.means, \n"
                             "so if you think that 2+2=5, then we are not on the path \n"
                             "3. Do not judge strictly, I tried");

    msgBox.exec();
}

void Calculator::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}

void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}

void Calculator::keyPressEvent(QKeyEvent* pe) {
    if(pe->text() != "")
        emit digitButtons[pe->text().toInt()]->clicked();
}

Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("Error"));
}

bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\303\267")) {
        if (rightOperand == 0.0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}
