#pragma once

#include <QString>
#include <QObject>
#include <QFileInfo>
#include <QList>
#include <atomic>

/*! \brief File monitors. */
namespace monitors {

    using Files = QList<QFileInfo>;

    /*! \brief Basic class for all file monitors. */
    class FileSystemMonitor : public QObject
    {
        Q_OBJECT

    public:
        explicit FileSystemMonitor(QObject *parent = nullptr);
        FileSystemMonitor(const QString &dir, QObject *parent = nullptr);

        QString dirName() const;
        void setDirName(const QString &dirName);

        QString lastError() const;
        bool inProcess() const;

        Files files() const;

    public slots:
        void start();
        bool stop();

    signals:
        void fileAdded(const QString &file);
        void fileRemoved(const QString &file);
        void oldFileName(const QString &name);
        void newFileName(const QString &name);
        void fileModified(const QString &fileName);
        void fileListChanged(const Files &files);

    protected:
        virtual void startMonitor() = 0;
        virtual bool stopMonitor() = 0;

        QString m_DirName;
        QString m_LastError;
        std::atomic<bool> m_ProcessFlag;
    };

} // namespace monitors

