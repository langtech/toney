#include "soxplayer.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include "configfile.h"

SoxPlayer::SoxPlayer(QObject *parent):
    Player(parent),
    _proc(this)
{
    _find_play_command();

    connect(&_proc,
            SIGNAL(stateChanged(QProcess::ProcessState)),
            SLOT(_finished_playing(QProcess::ProcessState)));
}

SoxPlayer::~SoxPlayer()
{
}

void SoxPlayer::play(const QString &path, double start, double end)
{
    stop();

    _path = path;
    _start = start;
    _end = end;
    _cmd = QString("%1 %2 trim %3 =%4 fade 0.05 0 0.05 gain %5")
            .arg(_play).arg(path).arg(start).arg(end).arg(_volume_level);
   _proc.start(_cmd, QIODevice::NotOpen);
}

void SoxPlayer::stop()
{
    _proc.disconnect(this);
    while (_proc.state() != QProcess::NotRunning) {
        _proc.kill();
        _proc.waitForFinished();
    }
    connect(&_proc,
            SIGNAL(stateChanged(QProcess::ProcessState)),
            SLOT(_finished_playing(QProcess::ProcessState)));
}

void SoxPlayer::_finished_playing(QProcess::ProcessState state)
{
    if (state == QProcess::NotRunning) {
        emit finishedPlaying(_path, _start, _end);
    }
}

void SoxPlayer::_find_play_command()
{
    QStringList search_paths;

    // from config file
    ConfigFile cf;
    search_paths.append(cf.getAsList("SOX_PLAY_PATH"));

    // directory containing the executable
    search_paths.append(QCoreApplication::applicationDirPath());

    // from PATH variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains("PATH")) {
#ifdef Q_OS_WIN
        char sep = ';';
#else
        char sep = ':';
#endif
        search_paths.append(env.value("PATH").split(sep));
    }

    // default search paths
    search_paths.append("/opt/local/bin"); // for macports
    search_paths.append("/usr/local/bin");
    search_paths.append("/usr/bin");
    search_paths.append("/bin");
    search_paths.append("/opt/bin");

    // search
    _play = "play";  // default value
    foreach (QString path, search_paths) {
        if (QDir(path).exists("play")) {
            _play = path + QDir::separator() + "play";
            break;
        }
    }
}
