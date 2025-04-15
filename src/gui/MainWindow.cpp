#include "gui/MainWindow.h"
#include "ChessBoardWidget.h"
#include "model/ChessModel.h"
#include "CapturedPiecesWidget.h" 
#include "core/Utils.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout> 
#include <QSplitter>   
#include <QListWidget> 
#include <QMessageBox>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QDebug>
#include <QSpacerItem> 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), fullMoveNumber(1) 
{
    setupUi();
    setupConnections();
    updateStatus();
}

MainWindow::~MainWindow()
{
    delete chessModel;
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Chess"));
    resize(850, 650); 

    // Menu Bar
    QMenuBar *menuBar = this->menuBar();
    QMenu *gameMenu = menuBar->addMenu(tr("&Game"));
    QAction *newGameAction = gameMenu->addAction(tr("&New Game"));
    gameMenu->addSeparator();
    QAction *quitAction = gameMenu->addAction(tr("&Quit"));

    // Central Widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Main Horizontal Layout (Board | Side Panel)
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Model
    chessModel = new ChessModel();
    chessModel->setupStartingPosition();

    // Board Widget
    boardWidget = new ChessBoardWidget(chessModel, centralWidget);

    // Side Panel (Vertical Layout: Captured White, History, Captured Black, Status)
    QWidget *sidePanelWidget = new QWidget(centralWidget);
    QVBoxLayout *sidePanelLayout = new QVBoxLayout(sidePanelWidget);
    sidePanelLayout->setContentsMargins(5, 0, 0, 0); 

    blackCapturedWidget = new CapturedPiecesWidget(chessModel, false, sidePanelWidget);
    blackCapturedWidget->setToolTip("Pieces captured by White"); 
    blackCapturedWidget->setMinimumHeight(40);

    moveHistoryWidget = new QListWidget(sidePanelWidget);
    moveHistoryWidget->setAlternatingRowColors(true);
    moveHistoryWidget->setMaximumWidth(180);
    moveHistoryWidget->setMinimumWidth(120);

    whiteCapturedWidget = new CapturedPiecesWidget(chessModel, true, sidePanelWidget);
    whiteCapturedWidget->setToolTip("Pieces captured by Black"); 
    whiteCapturedWidget->setMinimumHeight(40);

    statusLabel = new QLabel("Status", sidePanelWidget);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true); 
    statusLabel->setStyleSheet("font-weight: bold;");


    sidePanelLayout->addWidget(whiteCapturedWidget);
    sidePanelLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    sidePanelLayout->addWidget(new QLabel("Move History:", sidePanelWidget));
    sidePanelLayout->addWidget(moveHistoryWidget, 1);
    sidePanelLayout->addWidget(blackCapturedWidget);
    sidePanelLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
    sidePanelLayout->addWidget(statusLabel);
    sidePanelLayout->addStretch(0); 

    sidePanelWidget->setLayout(sidePanelLayout);


    // Use QSplitter for Resizeable Board/Side Panel
    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);
    splitter->addWidget(boardWidget);
    splitter->addWidget(sidePanelWidget);
    splitter->setStretchFactor(0, 1); 
    splitter->setStretchFactor(1, 0); 
    splitter->setCollapsible(0, false); 
    splitter->setCollapsible(1, false); 
    splitter->setHandleWidth(5); 


    // Add splitter to main layout
    mainLayout->addWidget(splitter);
    centralWidget->setLayout(mainLayout);

    
    // Connect Menu Actions
    connect(newGameAction, &QAction::triggered, this, &MainWindow::startNewGame);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
}


void MainWindow::setupConnections()
{
    connect(boardWidget, &ChessBoardWidget::moveAttempted, this, &MainWindow::handleMoveAttempt);
}

void MainWindow::handleMoveAttempt(const Move& move) {
    if (!chessModel || !boardWidget || chessModel->isGameOver()) return;

    std::string sanBase = Utils::moveToSAN(move, *chessModel);
    qDebug() << "Attempting move:" << QString::fromStdString(sanBase);

    bool success = chessModel->makeMove(move);

    if (success) {
        qDebug() << "Move successful:" << QString::fromStdString(sanBase);

        boardWidget->resetInteractionState(false);

        boardWidget->update();
        whiteCapturedWidget->update();
        blackCapturedWidget->update();
        updateMoveHistory(move, sanBase);
        updateStatus();

        if (chessModel->isGameOver()) {
             QString endMessage;
             if (chessModel->getIsCheckmate()) {
                 endMessage = QString("Checkmate! %1 wins.")
                                  .arg(chessModel->isWhiteToMove() ? "Black" : "White");
             } else if (chessModel->getIsStalemate()) {
                 endMessage = QString("Stalemate! Game is a draw.");
             }
             showGameOverMessage(endMessage);
        }

    } else {
        qDebug() << "Invalid move rejected by model.";
        statusBar()->showMessage(tr("Invalid move."), 2000);
    }
}

void MainWindow::updateMoveHistory(const Move& move, const std::string& sanBase) {
    if (!moveHistoryWidget || !chessModel) return;

    std::string finalSan = sanBase;
    if (chessModel->getIsCheckmate()) {
        finalSan += '#';
    } else if (chessModel->isInCheck()) {
        finalSan += '+';
    }

    QString historyEntry;
    Piece* movedPiece = chessModel->getPiece(move.to.row, move.to.col);

    if (movedPiece && !movedPiece->isWhite) {
        if (moveHistoryWidget->count() > 0) {
            QListWidgetItem* lastItem = moveHistoryWidget->item(moveHistoryWidget->count() - 1);
            historyEntry = lastItem->text() + "  " + QString::fromStdString(finalSan);
            lastItem->setText(historyEntry);
        } else {
             historyEntry = QString("%1. ... %2").arg(fullMoveNumber).arg(QString::fromStdString(finalSan));
             moveHistoryWidget->addItem(historyEntry);
        }
        fullMoveNumber++;
    } else {
        historyEntry = QString("%1. %2").arg(fullMoveNumber).arg(QString::fromStdString(finalSan));
        moveHistoryWidget->addItem(historyEntry);
    }

    moveHistoryWidget->scrollToBottom();
}

void MainWindow::updateStatus() {
    if (!chessModel || !statusLabel) return;

    QString statusText;
    if (chessModel->isGameOver()) {
        if (chessModel->getIsCheckmate()) {
            statusText = QString("CHECKMATE! %1 wins.")
                             .arg(chessModel->isWhiteToMove() ? "Black" : "White");
        } else if (chessModel->getIsStalemate()) {
            statusText = "STALEMATE! Draw.";
        }
    } else {
        statusText = QString("%1 to move.")
                         .arg(chessModel->isWhiteToMove() ? "White" : "Black");
        if (chessModel->isInCheck()) {
            statusText += " (Check!)";
        }
    }
    statusLabel->setText(statusText);
}

void MainWindow::showGameOverMessage(const QString& message) {
     QMessageBox::information(this, tr("Game Over"), message);
}

void MainWindow::startNewGame() {
     fullMoveNumber = 1;
     if (chessModel) {
         chessModel->setupStartingPosition();
     }
     if (boardWidget) {
         boardWidget->resetInteractionState();
         boardWidget->setEnabled(true);
         boardWidget->update();
     }
     if (moveHistoryWidget) {
         moveHistoryWidget->clear();
     }
     if (whiteCapturedWidget) whiteCapturedWidget->update();
     if (blackCapturedWidget) blackCapturedWidget->update();
     updateStatus();
     qDebug() << "New game started.";
     statusBar()->showMessage(tr("New Game Started."), 3000);
}