#include "filesystemmonitor.h"

#include <QDir>
#include <QDirIterator>

namespace monitors {

    /*!
     * \brief Class constructor.
     * \param parent pointer to parent object.
     */
    FileSystemMonitor::FileSystemMonitor(QObject *parent)
        : FileSystemMonitor("", parent)
    {}

    /*!
     * \brief Class constructor.
     * \param dir path to monitoring directory.
     * \param parent pointer to parent object.
     */
    FileSystemMonitor::FileSystemMonitor(const QString &dir, QObject *parent)
        : QObject(parent)
        , m_DirName(dir)
        , m_ProcessFlag(false)
    {
        if (!m_DirName.isEmpty()) emit fileListChanged(files());
    }

    /*!
     * \brief Run monitoring.
     *        Call protected method startMonitor(). You must override this method in inherited classes.
     */
    void FileSystemMonitor::start()
    {
        m_ProcessFlag = true;
        startMonitor();
    }

    /*!
     * \brief Stop monitoring.
     *        Call protected method startMonitor(). You must override this method in inherited classes.
     * \return true if program can stop process now and false in another cases.
     */
    bool FileSystemMonitor::stop()
    {
        m_ProcessFlag = false;
        return stopMonitor();
    }

    /*!
     * \brief Directory name getter.
     * \return dirirectory name.
     */
    QString FileSystemMonitor::dirName() const
    {
        return m_DirName;
    }

    /*!
     * \brief Directory name setter.
     * \param dirName directory name.
     */
    void FileSystemMonitor::setDirName(const QString &dirName)
    {
        if (m_ProcessFlag) m_ProcessFlag = false;
        m_DirName = dirName;
        if (!m_DirName.isEmpty()) emit fileListChanged(files());
    }

    /*!
     * \brief Error getter.
     *        For example, you can get error message, if directory doesn't exist.
     * \return error message.
     */
    QString FileSystemMonitor::lastError() const
    {
        return m_LastError;
    }

    /*!
     * \brief Method for check process status.
     * \return true if directory is monitored now, false if not.
     */
    bool FileSystemMonitor::inProcess() const
    {
        return m_ProcessFlag;
    }

    /*!
     * \brief Method for get directory file list.
     * \return file list.
     */
    Files FileSystemMonitor::files() const
    {
        Files result;

        if (!m_DirName.isEmpty() && QDir(m_DirName).exists()) {
            QDirIterator it(m_DirName); // NOTE: add QDirIterator::Subdirectories for scan nested directories
            QFileInfo f;
            while (it.hasNext()) {
                it.next();
                f.setFile(it.filePath());
                if (f.isFile()) result << f;
            }
        }

        return result;
    }

} // namespace monitors
