#pragma once

#include <thread>
#include <memory>

#include <QMainWindow>
#include <QFileInfo>

namespace monitors {
    class FileSystemMonitor;
}

namespace controllers {

    namespace Ui {
        class MainWindow;
    }

    using SharedMonitor = std::shared_ptr<monitors::FileSystemMonitor>;

    /*!
     * \brief The MainWindow class provides interface (controller functions) between
     *        auto generated ui class and directory monitor class.
     *
     *        It's just demo.
     *        In more complicated program you should add another class for model and controller.
     *        Also, you should use QAbstractTableModel for data management.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public slots:
        void setFilesCount(int count);
        void addFile(const QString &name);
        void deleteFile(const QString &name);
        void changeFile(const QString &name);

    private:
        void actionWatch();
        void actionFileOpen();

        SharedMonitor makeMonitor();
        void makeTable();
        QString fileSizeToString(qreal size) const;
        QStringList makeFileInfoList(const QFileInfo &f);

        Ui::MainWindow *ui;
        SharedMonitor m_Monitor;
        std::shared_ptr<std::thread> m_MonitorThread;
    };

}

