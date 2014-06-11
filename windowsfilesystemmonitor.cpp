#include "windowsfilesystemmonitor.h"
#include "constants.cpp"

#include <windows.h>

#include <QDir>
#include <QDebug>

namespace monitors {

    /*!
     * \brief Class constructor.
     * \param parent pointer to parent.
     */
    WindowsFileSystemMonitor::WindowsFileSystemMonitor(QObject *parent)
        : WindowsFileSystemMonitor("", parent)
    {
    }

    /*!
     * \brief Class constructor.
     * \param dir directory path.
     * \param parent pointer to parent.
     */
    WindowsFileSystemMonitor::WindowsFileSystemMonitor(const QString &dir, QObject *parent)
        : FileSystemMonitor(dir, parent)
    {

    }

    /*!
     * \brief Method for start monitoring process.
     *        This method use WinAPI for directory monitoring.
     */
    void WindowsFileSystemMonitor::startMonitor()
    {
        if (m_DirName.isEmpty()) {
            m_LastError = tr("Dirirectory name is empty.");
            return;
        }

        if (!QDir(m_DirName).exists()) {
            m_LastError = tr("Directory doesn't exsist.");
            return;
        }

        HANDLE hDir = CreateFile(
            m_DirName.toStdWString().c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
        );

        if (hDir == INVALID_HANDLE_VALUE) {
            m_LastError = tr("Can't make handle for directory.");
            return;
        }

        TCHAR szBuffer[BUFFER_SIZE];
        DWORD BytesReturned;
        while (ReadDirectoryChangesW(
            hDir,
            &szBuffer,
            sizeof(szBuffer),
            TRUE,
            FILE_NOTIFY_CHANGE_SECURITY |
            FILE_NOTIFY_CHANGE_CREATION |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_FILE_NAME,
            &BytesReturned,
            NULL,
            NULL
            )
        ) {
            if (!m_ProcessFlag) return;

            DWORD dwOffset = 0;
            FILE_NOTIFY_INFORMATION* pInfo = nullptr;
            auto current = szBuffer;
            do {
                pInfo = (FILE_NOTIFY_INFORMATION*)current;

                auto fname = pInfo->FileName;
                fname[pInfo->FileNameLength / 2] = 0;

                switch (pInfo->Action) {
                    case FILE_ACTION_ADDED:
                        emit fileAdded(QString::fromWCharArray(fname));
                        break;
                    case FILE_ACTION_REMOVED:
                        emit fileRemoved(QString::fromWCharArray(fname));
                        break;
                    case FILE_ACTION_MODIFIED:
                        emit fileModified(QString::fromWCharArray(fname));
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        emit oldFileName(QString::fromWCharArray(fname));
                        break;
                }

                dwOffset = pInfo->NextEntryOffset;
                current += dwOffset;
            } while (dwOffset != 0 && dwOffset < BUFFER_SIZE);
        }
    }

    /*!
     * \brief Method for stop monitor.
     * \return true if monitor stopped successfully and false if not.
     */
    bool WindowsFileSystemMonitor::stopMonitor()
    {
        // nothing special for Windows. always true
        return true;
    }

} // namespace monitors
