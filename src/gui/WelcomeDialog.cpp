#include "WelcomeDialog.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

WelcomeDialog::WelcomeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Chess Game"));
    setModal(true); 

    mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel(tr("Welcome to Chess!"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    newGameButton = new QPushButton(tr("Start New Game"), this);
    newGameButton->setMinimumSize(150, 40);
    QFont buttonFont = newGameButton->font();
    buttonFont.setPointSize(12);
    newGameButton->setFont(buttonFont);

    loadGameButton = new QPushButton(tr("Load Saved Game"), this);
    loadGameButton->setMinimumSize(150, 40);
    loadGameButton->setFont(buttonFont);

    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20); 
    mainLayout->addWidget(newGameButton, 0, Qt::AlignCenter);
    mainLayout->addWidget(loadGameButton, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);

    setLayout(mainLayout);
    setFixedSize(sizeHint());

    connect(newGameButton, &QPushButton::clicked, this, &WelcomeDialog::onNewGameClicked);
    connect(loadGameButton, &QPushButton::clicked, this, &WelcomeDialog::onLoadGameClicked);

}

WelcomeDialog::~WelcomeDialog()
{
}

void WelcomeDialog::onNewGameClicked()
{
    m_choice = UserChoice::NewGame;
    accept(); 
}

void WelcomeDialog::onLoadGameClicked()
{
    m_choice = UserChoice::LoadGame;
    accept(); 
}

WelcomeDialog::UserChoice WelcomeDialog::getChoice() const
{
    return m_choice;
}

void WelcomeDialog::closeEvent(QCloseEvent *event)
{
    if (result() != QDialog::Accepted) { 
        m_choice = UserChoice::Rejected;
    }
    QDialog::closeEvent(event);
}