/*
 * KDiff3 - Text Diff And Merge Tool
 * 
 * SPDX-FileCopyrightText: 2002-2011 Joachim Eibl, joachim.eibl at gmx.de
 * SPDX-FileCopyrightText: 2018-2020 Michael Reeves reeves.87@gmail.com
 * SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FILEACCESS_H
#define FILEACCESS_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QSharedPointer>
#include <QTemporaryFile>
#include <QUrl>

#include <KIO/UDSEntry>
#include <KJob>

#include <type_traits>

namespace KIO {
class Job;
}

class t_DirectoryList;

class FileAccess
{
  public:
    FileAccess() = default;
    explicit FileAccess(const QString& name, bool bWantToWrite = false); // name: local file or dirname or url (when supported)
    void setFile(const QString& name, bool bWantToWrite = false);
    void setFile(const QUrl& url, bool bWantToWrite = false);
    void setFile(FileAccess* pParent, const QFileInfo& fi);

    void loadData();

    bool isNormal() const;
    bool isValid() const;
    bool isFile() const;
    bool isDir() const;
    bool isSymLink() const;
    bool exists() const;
    qint64 size() const;     // Size as returned by stat().
    qint64 sizeForReading(); // If the size can't be determined by stat() then the file is copied to a local temp file.
    bool isReadable() const;
    bool isWritable() const;
    bool isExecutable() const;
    bool isHidden() const;
    QString readLink() const;

    QDateTime lastModified() const;

    QString fileName(bool needTmp = false) const; // Just the name-part of the path, without parent directories
    QString fileRelPath() const;                  // The path relative to base comparison directory
    QString prettyAbsPath() const;
    QUrl url() const;
    void setUrl(const QUrl& inUrl) { m_url = inUrl; }
    QString absoluteFilePath() const;

    bool isLocal() const;

    bool readFile(void* pDestBuffer, qint64 maxLength);
    bool writeFile(const void* pSrcBuffer, qint64 length);
    bool listDir(t_DirectoryList* pDirList, bool bRecursive, bool bFindHidden,
                 const QString& filePattern, const QString& fileAntiPattern,
                 const QString& dirAntiPattern, bool bFollowDirLinks, bool bUseCvsIgnore);
    bool copyFile(const QString& destUrl);
    bool createBackup(const QString& bakExtension);

    QString getTempName() const;
    bool createLocalCopy();
    static void createTempFile(QTemporaryFile&);
    bool removeFile();
    static bool makeDir(const QString&);
    static bool removeDir(const QString&);
    static bool exists(const QString&);
    static QString cleanPath(const QString&);

    //bool chmod( const QString& );
    bool rename(const FileAccess&);
    static bool symLink(const QString& linkTarget, const QString& linkLocation);

    void addPath(const QString& txt);
    const QString& getStatusText() const;

    FileAccess* parent() const; // !=0 for listDir-results, but only valid if the parent was not yet destroyed.

    void doError();
    void filterList(t_DirectoryList* pDirList, const QString& filePattern,
                    const QString& fileAntiPattern, const QString& dirAntiPattern,
                    const bool bUseCvsIgnore);

    QDir getBaseDirectory() const { return m_baseDir; }

    bool open(const QFile::OpenMode flags);

    qint64 read(char* data, const qint64 maxlen);
    void close();

    const QString& errorString() const;

  private:
    friend class FileAccessJobHandler;
    void setFromUdsEntry(const KIO::UDSEntry& e, FileAccess* parent);
    void setStatusText(const QString& s);

    void reset();

    bool interruptableReadFile(void* pDestBuffer, qint64 maxLength);

    QUrl m_url;
    bool m_bValidData = false;

    //long m_fileType; // for testing only
    FileAccess* m_pParent = nullptr;

    QDir m_baseDir;
    QFileInfo m_fileInfo;
    QString m_linkTarget;
    QString m_name;
    QString m_localCopy;
    QSharedPointer<QTemporaryFile> tmpFile = QSharedPointer<QTemporaryFile>::create();
    QSharedPointer<QFile> realFile = nullptr;

    qint64 m_size = 0;
    QDateTime m_modificationTime = QDateTime::fromMSecsSinceEpoch(0);
    bool m_bSymLink = false;
    bool m_bFile = false;
    bool m_bDir = false;
    bool m_bExists = false;
    bool m_bWritable = false;
    bool m_bReadable = false;
    bool m_bExecutable = false;
    bool m_bHidden = false;

    QString m_statusText; // Might contain an error string, when the last operation didn't succeed.
};

static_assert(std::is_move_assignable<FileAccess>::value, "FileAccess must be move assignable.");
static_assert(std::is_move_constructible<FileAccess>::value, "FileAccess must be move constructable.");

class t_DirectoryList : public std::list<FileAccess>
{
};

class FileAccessJobHandler : public QObject
{
    Q_OBJECT
  public:
    explicit FileAccessJobHandler(FileAccess* pFileAccess);

    bool get(void* pDestBuffer, long maxLength);
    bool put(const void* pSrcBuffer, long maxLength, bool bOverwrite, bool bResume = false, int permissions = -1);
    bool stat(short detailLevel = 2, bool bWantToWrite = false);
    bool copyFile(const QString& dest);
    bool rename(const FileAccess& dest);
    bool listDir(t_DirectoryList* pDirList, bool bRecursive, bool bFindHidden,
                 const QString& filePattern, const QString& fileAntiPattern,
                 const QString& dirAntiPattern, bool bFollowDirLinks, bool bUseCvsIgnore);
    bool mkDir(const QString& dirName);
    bool rmDir(const QString& dirName);
    bool removeFile(const QUrl& fileName);
    bool symLink(const QUrl& linkTarget, const QUrl& linkLocation);

  private:
    FileAccess* m_pFileAccess = nullptr;
    bool m_bSuccess = false;

    // Data needed during Job
    qint64 m_transferredBytes = 0;
    char* m_pTransferBuffer = nullptr; // Needed during get or put
    qint64 m_maxLength = 0;

    QString m_filePattern;
    QString m_fileAntiPattern;
    QString m_dirAntiPattern;
    t_DirectoryList* m_pDirList = nullptr;
    bool m_bFindHidden = false;
    bool m_bRecursive = false;
    bool m_bFollowDirLinks = false;

    bool scanLocalDirectory(const QString& dirName, t_DirectoryList* dirList);

  private Q_SLOTS:
    void slotStatResult(KJob*);
    void slotSimpleJobResult(KJob* pJob);
    void slotPutJobResult(KJob* pJob);

    void slotGetData(KJob*, const QByteArray&);
    void slotPutData(KIO::Job*, QByteArray&);

    void slotListDirProcessNewEntries(KIO::Job*, const KIO::UDSEntryList& l);
};

#endif
