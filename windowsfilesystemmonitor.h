#pragma once

#include "filesystemmonitor.h"

namespace monitors {

    /*! \brief File monitor for Windows. */
    class WindowsFileSystemMonitor : public FileSystemMonitor
    {
        Q_OBJECT

    public:
        explicit WindowsFileSystemMonitor(QObject *parent = nullptr);
        WindowsFileSystemMonitor(const QString &dir, QObject *parent = nullptr);

    signals:

    protected:
        void startMonitor() override;
        bool stopMonitor() override;
};

} // namespace monitors
