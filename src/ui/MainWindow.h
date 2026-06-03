#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include "../network/P2PManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onStartService();
    void onStopService();
    void onConnectToPeer();
    void onSelectFile();
    void onSendFile();
    void onPeerConnected(const QString& peerId, const QString& address);
    void onPeerDisconnected(const QString& peerId);
    void onTransferProgress(const QUuid& transferId, double progress);
    void onTransferCompleted(const QUuid& transferId, const QString& filePath);
    void onTransferFailed(const QUuid& transferId, const QString& error);
    void updatePeerList();

private:
    void setupUI();
    void setupConnections();
    void logMessage(const QString& message);

    P2PManager* m_p2pManager;

    // UI组件
    QLineEdit* m_portEdit;
    QLineEdit* m_peerAddressEdit;
    QLineEdit* m_peerPortEdit;
    QPushButton* m_startButton;
    QPushButton* m_stopButton;
    QPushButton* m_connectButton;
    QPushButton* m_selectFileButton;
    QPushButton* m_sendFileButton;
    QLabel* m_fileLabel;
    QListWidget* m_peerList;
    QListWidget* m_logList;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;

    QString m_selectedFilePath;
    bool m_serviceRunning;
};

#endif // MAINWINDOW_H