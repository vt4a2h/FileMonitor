#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windowsfilesystemmonitor.h"

#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>

namespace controllers {

    /*!
     * \brief Class constructor.
     * \param parent pointer to parent object.
     */
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
        , m_Monitor(nullptr)
        , m_MonitorThread(nullptr)
    {
        ui->setupUi(this);
        ui->tw_Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->le_Path->setFocus();

        m_Monitor = makeMonitor();

        connect(ui->pb_Watch, &QPushButton::toggled,
                this, &MainWindow::actionWatch);
        connect(ui->pb_Path, &QPushButton::clicked,
                this, &MainWindow::actionFileOpen);
        connect(m_Monitor.get(), &monitors::FileSystemMonitor::fileAdded,
                this, &MainWindow::addFile);
        connect(m_Monitor.get(), &monitors::FileSystemMonitor::fileRemoved,
                this, &MainWindow::deleteFile);
        connect(m_Monitor.get(), &monitors::FileSystemMonitor::fileModified,
                this, &MainWindow::changeFile);
        connect(m_Monitor.get(), &monitors::FileSystemMonitor::oldFileName,
                this, &MainWindow::deleteFile);
    }

    /*!
     * \brief Class destructor.
     */
    MainWindow::~MainWindow()
    {
        delete ui;
        if (m_MonitorThread) m_MonitorThread->detach();
    }

    /*!
     * \brief Methods for set files count.
     * \param count new files count.
     */
    void MainWindow::setFilesCount(int count)
    {
        ui->lbl_FilesCount->setText(QString::number(count));
    }

    /*!
     * \brief Method for add new file to the table.
     * \param name new file name.
     */
    void MainWindow::addFile(const QString &name)
    {
        QFileInfo f(m_Monitor->dirName() + "/" + name);
        QStringList fInfo(makeFileInfoList(f));
        QTableWidgetItem *item(nullptr);

        ui->tw_Table->insertRow(ui->tw_Table->rowCount());
        ui->lbl_FilesCount->setText(QString::number(ui->lbl_FilesCount->text().toLong() + 1));

        for (int i = 0; i < ui->tw_Table->columnCount(); ++i) {
            ui->tw_Table->setItem(ui->tw_Table->rowCount()-1, i, new QTableWidgetItem());
            item = ui->tw_Table->item(ui->tw_Table->rowCount()-1, i);
            item->setData(Qt::DisplayRole, fInfo[i]);
        }
    }

    /*!
     * \brief Method for delete file from the table.
     * \param name file name.
     */
    void MainWindow::deleteFile(const QString &name)
    {
        QFileInfo f(m_Monitor->dirName() + "/" + name);
        auto itemsList = ui->tw_Table->findItems(f.filePath().remove(m_Monitor->dirName() + "/"),
                                                 Qt::MatchFixedString);
        for (auto &&item : itemsList) {
            ui->tw_Table->removeRow(ui->tw_Table->row(item));
            ui->lbl_FilesCount->setText(QString::number(ui->lbl_FilesCount->text().toLong() - 1));
        }
    }

    /*!
     * \brief Method for change view if file changed.
     * \param name file name.
     */
    void MainWindow::changeFile(const QString &name)
    {
        QFileInfo f(m_Monitor->dirName() + "/" + name);

        if (f.exists()) {
            auto itemsList = ui->tw_Table->findItems(f.filePath().remove(m_Monitor->dirName() + "/"),
                                                     Qt::MatchFixedString);

            QStringList fInfo;
            for (auto &&item : itemsList) {
                fInfo = std::move(makeFileInfoList(f));
                for (int i = 0; i < ui->tw_Table->columnCount(); ++i) {
                    item = ui->tw_Table->item(ui->tw_Table->row(item), i);
                    item->setData(Qt::DisplayRole, fInfo[i]);
                }
            }

            if (itemsList.isEmpty()) addFile(name);
        }
    }

    /*!
     * \brief Method for starting (or stopping) watching process.
     *        Method creates new thread for monitoring.
     */
    void MainWindow::actionWatch()
    {
        if (ui->pb_Watch->isChecked() && !m_MonitorThread && m_Monitor && !m_Monitor->inProcess()) {
            QString path = ui->le_Path->text();
            if (path.isEmpty() || !QDir(path).exists()) {
                ui->pb_Watch->setChecked(false);
                QMessageBox::warning(this,
                                     tr("Внимание!"),
                                     tr("Очень плохое имя директории.\nПопробуйте еще раз!"),
                                     QMessageBox::Ok);
                return;
            }

            m_Monitor->setDirName(QFileInfo(path).filePath());
            makeTable();
            m_MonitorThread = std::make_shared<std::thread>(&monitors::FileSystemMonitor::start,
                                                            m_Monitor);
        } else if (!ui->pb_Watch->isChecked() && m_MonitorThread) {
            m_Monitor->stop();
            while (m_Monitor->inProcess());
            m_MonitorThread->detach();
            m_MonitorThread.reset();
        }
    }

    /*!
     * \brief Method for choose a directory.
     */
    void MainWindow::actionFileOpen()
    {
        QString dir = QFileDialog::getExistingDirectory(this,
                                                        tr("Выберите директорию"),
                                                        "",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) ui->le_Path->setText(dir);
    }

    /*!
     * \brief Method for create file monitor.
     *        Monitor type depends on operation system.
     * \return
     */
    SharedMonitor MainWindow::makeMonitor()
    {
#ifdef Q_OS_WIN
        return std::make_shared<monitors::WindowsFileSystemMonitor>();
#else
        throw();
#endif
    }

    /*!
     * \brief Method makes table for files list.
     */
    void MainWindow::makeTable()
    {
        auto table = ui->tw_Table;
        int columnCount = 3;
        table->clear();

        auto files = m_Monitor->files();
        setFilesCount(files.count());

        table->setColumnCount(columnCount);
        table->setRowCount(files.count());
        for (int i = 0, maxI = table->columnCount(); i < maxI; ++i)
            for (int j = 0, maxJ = table->rowCount(); j < maxJ; ++j)
                table->setItem(j, i, new QTableWidgetItem());
        table->setHorizontalHeaderLabels({tr("Имя"), tr("Изменён"), tr("Размер")});

        QTableWidgetItem *item(nullptr);
        QStringList fInfo;
        for (auto &&f : files) {
            fInfo = std::move(makeFileInfoList(f));
            for (int i = 0; i < columnCount; ++i) {
                item = table->item(files.indexOf(f), i);
                item->setData(Qt::DisplayRole, fInfo[i]);
            }
        }
    }

    /*!
     * \brief Method converts file size to string representation.
     * \param size file size.
     * \return file size string representation.
     */
    QString MainWindow::fileSizeToString(qreal size) const
    {
        static const QStringList symbols {tr("Б"), tr("кБ"), tr("МБ"), tr("ГБ"),
                                          tr("ТБ"), tr("ПБ"), tr("ЭБ"), tr("ЗБ"),
                                          tr("ЙБ")};
        int index(0);

        while (size >= 1024.0 && index < symbols.count()) {
            size /= 1024.0;
            ++index;
        }

        return tr("%1 %2").arg(QString::number(size, 'g', 3), symbols[index]);
    }

    /*!
     * \brief Method makes list with actual information about file.
     * \param f detailed file information.
     * \return list with important file information.
     */
    QStringList MainWindow::makeFileInfoList(const QFileInfo &f)
    {
        QStringList result;

        if (f.exists())
            result << f.filePath().remove(m_Monitor->dirName().append("/"))
                   << f.lastModified().toString("hh:mm:ss - dd.MM.yyyy")
                   << fileSizeToString(f.size());

        return result;
    }

} // namespace controllers
