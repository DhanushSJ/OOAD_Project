#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ChessBoardWidget;
class ChessModel;
class QLabel;
class Move;
class QListWidget;
class CapturedPiecesWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleMoveAttempt(const Move& move);
    void startNewGame();

private:
    ChessBoardWidget *boardWidget = nullptr;
    ChessModel *chessModel = nullptr;
    QLabel *statusLabel = nullptr;
    QListWidget *moveHistoryWidget = nullptr;
    CapturedPiecesWidget *whiteCapturedWidget = nullptr;
    CapturedPiecesWidget *blackCapturedWidget = nullptr;

    int fullMoveNumber = 1;

    void setupUi();
    void setupConnections();
    void updateStatus();
    void showGameOverMessage(const QString& message);
    void updateMoveHistory(const Move& move, const std::string& sanBase);
};

#endif
