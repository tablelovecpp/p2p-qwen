#include <QApplication>
#include <QStyleFactory>
#include "ui/MainWindow.h"
#include "utils/Logger.h"
#include "utils/Config.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("P2P File Transfer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("P2PTransfer");

    // 加载配置
    Config::instance().loadFromFile();

    // 设置日志
    Logger::instance().setLogFile("p2p_transfer.log");
    LOG_INFO("Application starting...");

    // 设置深色主题
    app.setStyle(QStyleFactory::create("Fusion"));

    // 创建主窗口
    MainWindow window;
    window.show();

    LOG_INFO("Application started");

    return app.exec();
}