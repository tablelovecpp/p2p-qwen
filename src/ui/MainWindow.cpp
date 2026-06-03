#include "MainWindow.h"
#include "../utils/Logger.h"
#include "../utils/Config.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_p2pManager(new P2PManager(this))
    , m_serviceRunning(false)
{
    setupUI();
    setupConnections();

    setWindowTitle("P2P File Transfer");
    resize(800, 600);

    LOG_INFO("Main window created");
}

MainWindow::~MainWindow()
{
    if (m_serviceRunning) {
        m_p2pManager->stop();
    }
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 服务控制区域
    QGroupBox* serviceGroup = new QGroupBox("Service Control");
    QVBoxLayout* serviceLayout = new QVBoxLayout(serviceGroup);

    QHBoxLayout* portLayout = new QHBoxLayout();
    portLayout->addWidget(new QLabel("TCP Port:"));
    m_portEdit = new QLineEdit("8888");
    portLayout->addWidget(m_portEdit);
    serviceLayout->addLayout(portLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_startButton = new QPushButton("Start Service");
    m_stopButton = new QPushButton("Stop Service");
    m_stopButton->setEnabled(false);
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_stopButton);
    serviceLayout->addLayout(buttonLayout);

    mainLayout->addWidget(serviceGroup);

    // 对等节点连接区域
    QGroupBox* connectGroup = new QGroupBox("Connect to Peer");
    QVBoxLayout* connectLayout = new QVBoxLayout(connectGroup);

    QHBoxLayout* addressLayout = new QHBoxLayout();
    addressLayout->addWidget(new QLabel("Address:"));
    m_peerAddressEdit = new QLineEdit("127.0.0.1");
    addressLayout->addWidget(m_peerAddressEdit);

    addressLayout->addWidget(new QLabel("Port:"));
    m_peerPortEdit = new QLineEdit("8888");
    addressLayout->addWidget(m_peerPortEdit);

    m_connectButton = new QPushButton("Connect");
    addressLayout->addWidget(m_connectButton);

    connectLayout->addLayout(addressLayout);
    mainLayout->addWidget(connectGroup);

    // 文件传输区域
    QGroupBox* transferGroup = new QGroupBox("File Transfer");
    QVBoxLayout* transferLayout = new QVBoxLayout(transferGroup);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    m_selectFileButton = new QPushButton("Select File");
    m_fileLabel = new QLabel("No file selected");
    m_sendFileButton = new QPushButton("Send File");
    m_sendFileButton->setEnabled(false);

    fileLayout->addWidget(m_selectFileButton);
    fileLayout->addWidget(m_fileLabel, 1);
    fileLayout->addWidget(m_sendFileButton);
    transferLayout->addLayout(fileLayout);

    m_progressBar = new QProgressBar();
    m_progressBar->setValue(0);
    transferLayout->addWidget(m_progressBar);

    mainLayout->addWidget(transferGroup);

    // 对等节点列表
    QGroupBox* peersGroup = new QGroupBox("Online Peers");
    QVBoxLayout* peersLayout = new QVBoxLayout(peersGroup);
    m_peerList = new QListWidget();
    peersLayout->addWidget(m_peerList);
    mainLayout->addWidget(peersGroup);

    // 日志区域
    QGroupBox* logGroup = new QGroupBox("Log");
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    m_logList = new QListWidget();
    logLayout->addWidget(m_logList);
    mainLayout->addWidget(logGroup);

    // 状态栏
    m_statusLabel = new QLabel("Ready");
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::setupConnections()
{
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartService);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopService);
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectToPeer);
    connect(m_selectFileButton, &QPushButton::clicked, this, &MainWindow::onSelectFile);
    connect(m_sendFileButton, &QPushButton::clicked, this, &MainWindow::onSendFile);

    connect(m_p2pManager, &P2PManager::peerConnected,
            this, &MainWindow::onPeerConnected);
    connect(m_p2pManager, &P2PManager::peerDisconnected,
            this, &MainWindow::onPeerDisconnected);
    connect(m_p2pManager, &P2PManager::transferProgress,
            this, &MainWindow::onTransferProgress);
    connect(m_p2pManager, &P2PManager::transferCompleted,
            this, &MainWindow::onTransferCompleted);
    connect(m_p2pManager, &P2PManager::transferFailed,
            this, &MainWindow::onTransferFailed);
}

void MainWindow::onStartService()
{
    bool ok;
    quint16 port = m_portEdit->text().toUShort(&ok);

    if (!ok || port == 0) {
        QMessageBox::warning(this, "Error", "Invalid port number");
        return;
    }

    if (m_p2pManager->start(port, port + 1)) {
        m_serviceRunning = true;
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_statusLabel->setText("Service running on port " + QString::number(port));
        logMessage("Service started on port " + QString::number(port));
    } else {
        QMessageBox::critical(this, "Error", "Failed to start service");
    }
}

void MainWindow::onStopService()
{
    m_p2pManager->stop();
    m_serviceRunning = false;
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_statusLabel->setText("Service stopped");
    m_peerList->clear();
    logMessage("Service stopped");
}

void MainWindow::onConnectToPeer()
{
    if (!m_serviceRunning) {
        QMessageBox::warning(this, "Error", "Service not running");
        return;
    }

    QString address = m_peerAddressEdit->text();
    bool ok;
    quint16 port = m_peerPortEdit->text().toUShort(&ok);

    if (address.isEmpty() || !ok || port == 0) {
        QMessageBox::warning(this, "Error", "Invalid address or port");
        return;
    }

    if (m_p2pManager->connectToPeer(address, port)) {
        logMessage(QString("Connecting to %1:%2...").arg(address).arg(port));
    } else {
        QMessageBox::critical(this, "Error", "Failed to connect");
    }
}

void MainWindow::onSelectFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File");

    if (!filePath.isEmpty()) {
        m_selectedFilePath = filePath;
        m_fileLabel->setText(QFileInfo(filePath).fileName());
        m_sendFileButton->setEnabled(true);
        logMessage(QString("Selected file: %1").arg(filePath));
    }
}

void MainWindow::onSendFile()
{
    if (m_selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "No file selected");
        return;
    }

    if (m_peerList->currentItem() == nullptr) {
        QMessageBox::warning(this, "Error", "No peer selected");
        return;
    }

    QString peerId = m_peerList->currentItem()->data(Qt::UserRole).toString();

    QUuid transferId = m_p2pManager->sendFile(m_selectedFilePath, peerId);

    if (!transferId.isNull()) {
        logMessage(QString("Sending file to %1...").arg(peerId));
        m_progressBar->setValue(0);
    } else {
        QMessageBox::critical(this, "Error", "Failed to start transfer");
    }
}

void MainWindow::onPeerConnected(const QString& peerId, const QString& address)
{
    QListWidgetItem* item = new QListWidgetItem(QString("%1 (%2)").arg(peerId.left(8)).arg(address));
    item->setData(Qt::UserRole, peerId);
    m_peerList->addItem(item);

    logMessage(QString("Peer connected: %1").arg(peerId.left(8)));
}

void MainWindow::onPeerDisconnected(const QString& peerId)
{
    for (int i = 0; i < m_peerList->count(); ++i) {
        QListWidgetItem* item = m_peerList->item(i);
        if (item->data(Qt::UserRole).toString() == peerId) {
            delete m_peerList->takeItem(i);
            break;
        }
    }

    logMessage(QString("Peer disconnected: %1").arg(peerId.left(8)));
}

void MainWindow::onTransferProgress(const QUuid& transferId, double progress)
{
    Q_UNUSED(transferId);
    m_progressBar->setValue(static_cast<int>(progress));
    m_statusLabel->setText(QString("Transferring: %1%").arg(progress, 0, 'f', 1));
}

void MainWindow::onTransferCompleted(const QUuid& transferId, const QString& filePath)
{
    Q_UNUSED(transferId);
    m_progressBar->setValue(100);
    m_statusLabel->setText("Transfer completed");
    logMessage(QString("Transfer completed: %1").arg(filePath));

    QMessageBox::information(this, "Success", "File transfer completed!");
}

void MainWindow::onTransferFailed(const QUuid& transferId, const QString& error)
{
    Q_UNUSED(transferId);
    m_progressBar->setValue(0);
    m_statusLabel->setText("Transfer failed");
    logMessage(QString("Transfer failed: %1").arg(error));

    QMessageBox::critical(this, "Error", "Transfer failed: " + error);
}

void MainWindow::updatePeerList()
{
    // 定期更新对等节点列表
    QList<PeerNode> peers = m_p2pManager->getOnlinePeers();

    // 清除现有项
    m_peerList->clear();

    // 添加对等节点
    for (const PeerNode& peer : peers) {
        QListWidgetItem* item = new QListWidgetItem(
            QString("%1 (%2:%3)").arg(peer.peerId.left(8)).arg(peer.address).arg(peer.tcpPort));
        item->setData(Qt::UserRole, peer.peerId);
        m_peerList->addItem(item);
    }
}

void MainWindow::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logList->addItem(QString("[%1] %2").arg(timestamp).arg(message));
    m_logList->scrollToBottom();
}