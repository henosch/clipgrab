#include "youtube_dl.h"

YoutubeDl::YoutubeDl()
{

}

QString YoutubeDl::path = QString();

QString YoutubeDl::find(bool force) {
    if (!force && !path.isEmpty()) return path;

    // Prefer downloaded youtube-dl
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/youtube-dl";
    if (QFile::exists(localPath)) {
        QProcess* process = instance(localPath, QStringList() << "--version");
        process->start();
        process->waitForFinished();
        bool success = (process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0);
        process->deleteLater();
        if (success) {
            path = localPath;
            qDebug() << "Found youtube-dl in AppDataLocation at " << path;
            return path;
        }
    }

    // Try system-wide youtube-dl installation
    QString globalPath = QStandardPaths::findExecutable("youtube-dl");
    if (!globalPath.isEmpty()) {
        QProcess* process = instance(globalPath, QStringList() << "--version");
        process->start();
        process->waitForFinished();
        bool success = (process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0);
        process->deleteLater();
        if (success) {
            path = globalPath;
            qDebug() << "Found youtube-dl executable at " << path;
            return path;
        }
    }

    qDebug() << "Error: could not find youtube-dl";
    return "";
}

QProcess* YoutubeDl::instance(QStringList arguments) {
    return instance(find(), arguments);
}

QProcess* YoutubeDl::instance(QString path, QStringList arguments) {
    QProcess *process = new QProcess();

    QString execPath = QCoreApplication::applicationDirPath();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", execPath + ":/opt/homebrew/bin:/usr/local/bin:" + env.value("PATH"));
    process->setEnvironment(env.toStringList());

    #if defined Q_OS_WIN
        process->setProgram(execPath + "/python/python.exe");
    #else
        QString pythonPath = "/opt/homebrew/bin/python3";
        if (!QFile::exists(pythonPath)) {
            pythonPath = "/usr/local/bin/python3";
        }
        if (!QFile::exists(pythonPath)) {
            pythonPath = QStandardPaths::findExecutable("python3");
        }
        if (pythonPath.isEmpty()) {
            pythonPath = QStandardPaths::findExecutable("python");
        }
        process->setProgram(pythonPath);
    #endif

    QSettings settings;
    QStringList proxyArguments;
    if (settings.value("UseProxy", false).toBool()) {
        QUrl proxyUrl;

        proxyUrl.setHost(settings.value("ProxyHost", "").toString());
        proxyUrl.setPort(settings.value("ProxyPort", "").toInt());

        if (settings.value("ProxyType", false).toInt() == 0) {
            proxyUrl.setScheme("http");
        } else {
            proxyUrl.setScheme("socks5");
        }
        if (settings.value("ProxyAuthenticationRequired", false).toBool() == true) {
            proxyUrl.setUserName(settings.value("ProxyUsername", "").toString());
            proxyUrl.setPassword(settings.value("ProxyPassword").toString());
        }

        proxyArguments << "--proxy" << proxyUrl.toString();
    }

    QStringList networkArguments;
    if (settings.value("forceIpV4", false).toBool()) {
        networkArguments << "--force-ipv4";
    }

    QString locale = settings.value("Language", "auto").toString();
    if (locale == "auto") {
        locale = QLocale::system().name();
    }
    QString langCode = locale.split("_").first().toLower();
    if (langCode.isEmpty()) {
        langCode = "en";
    }

    QStringList langArguments;
    langArguments << "--extractor-args" << ("youtube:lang=" + langCode);

    process->setArguments(QStringList() << path << arguments << proxyArguments << networkArguments << langArguments);
    return process;
}

QString YoutubeDl::getVersion() {
    QProcess* youtubeDl = instance(QStringList("--version"));
    youtubeDl->start();
    youtubeDl->waitForFinished(10000);
    QString version = QString::fromUtf8(youtubeDl->readAllStandardOutput()).trimmed();
    youtubeDl->deleteLater();
    return version;
}

QString YoutubeDl::getPythonVersion() {
    QProcess* youtubeDl = instance(QStringList());
    youtubeDl->setArguments(QStringList("--version"));
    youtubeDl->start();
    youtubeDl->waitForFinished(10000);
    QString version = QString::fromUtf8(youtubeDl->readAllStandardOutput()).trimmed();
    youtubeDl->deleteLater();
    return version;
}

QString YoutubeDl::findPython() {
    QProcess* youtubeDl = instance(QStringList());
    QString program = youtubeDl->program();
    youtubeDl->deleteLater();
    return program;
}
