#include "connectionmanager.h"

#include <cstdio>
#include <QString>
#include <QFileInfo>
#include <QIcon>
#include <QTreeWidgetItem>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

ConnectionManager::ConnectionManager(QString name, QString host, QString port, QString user, QString pass)
{
    _name = name;
    _host = host;
    _port = port;
    _user = user;
    _pass = pass;
}

bool ConnectionManager::start()
{
    /* options */
    int sock;
    struct sockaddr_in sin;
    const char *fingerprint;
    char host[200], ip[200], user[200], pass[200];
    int port = _port.toInt();
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
    int rc;

    /* read parameters */
    strcpy(host, _host.toLatin1().data());
    strcpy(user, _user.toLatin1().data());
    strcpy(pass, _pass.toLatin1().data());

    /* initialize libssh2 */
    rc = libssh2_init(0);
    if (rc != 0) {
        fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
        return false;
    }

    /* resolve host name */
    if ( (he = gethostbyname( host ) ) == nullptr)
    {
        herror("gethostbyname");
        return false;
    }
    addr_list = reinterpret_cast<struct in_addr **>(he->h_addr_list);
    for(i = 0; addr_list[i] != nullptr; i++)
    {
        /* return the first one; */
        strcpy(ip, inet_ntoa(*addr_list[i]) );
        break;
    }

    /*
     * create socket
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(static_cast<uint16_t>(port));
    sin.sin_addr.s_addr = inet_addr(ip);
    if (connect(sock, reinterpret_cast<struct sockaddr*>(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to open socket!\n");
        return -1;
    }

    /* create new ssh session */
    session = libssh2_session_init();
    if (!session) {
        fprintf(stderr, "failed to create new libssh2 session!\n");
        return false;
    }

    /* tell libssh2 we are blocking */
    libssh2_session_set_blocking(session, 1);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return false;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", static_cast<unsigned char>(fingerprint[i]));
    }
    fprintf(stderr, "\n");

    /* Authenticate via password */
    if (libssh2_userauth_password(session, user, pass)) {
        fprintf(stderr, "Authentication by password failed.\n");
        return false;
    }

    /* done */
    return true;
}

void ConnectionManager::disconnect() {

}

QIcon ConnectionManager::fileIcon(const QString &filename)
{
    QFileInfo info(filename);
    const QString ext=info.suffix().toLower();
    QIcon icon;
    if (ext == "pdf") {
        icon=QIcon::fromTheme("application-pdf");
    } else if (ext.startsWith("doc")) {
        icon=QIcon::fromTheme("application-msword");
    } else if (ext=="zip" ||
               ext=="gz"  ||
               ext=="bz2" ||
               ext=="tar") {
        icon=QIcon::fromTheme("application-x-archive");
    } else if (ext=="png" ||
                  ext=="jpg"  ||
                  ext=="gif"  ||
                  ext=="svg" ||
                  ext=="bmp") {
        icon=QIcon::fromTheme("image-x-generic");
    } else if (ext=="c") {
        icon=QIcon::fromTheme("text-x-csrc");
    } else if (ext=="cpp" || ext=="cc") {
        icon=QIcon::fromTheme("text-x-c++src");
    } else if (ext=="h") {
        icon=QIcon::fromTheme("text-x-chdr");
    } else if (ext=="ter") {
        icon=QIcon::fromTheme("application-x-blender");
    } else if (ext=="ti") {
        icon=QIcon::fromTheme("text-mathml");
    } else if (ext=="se") {
        icon=QIcon::fromTheme("text-xml");
    } else if (ext=="java") {
        icon=QIcon::fromTheme("text-x-java-source");
    } else if (ext=="class") {
        icon=QIcon::fromTheme("text-x-java");
    } else if (ext=="jar") {
        icon=QIcon::fromTheme("application-x-java-archive");
    } else if (ext=="sh") {
        icon=QIcon::fromTheme("text-x-script");
    } else if (ext=="txt" || ext=="md") {
        icon=QIcon::fromTheme("text-plain");
    } else if (ext=="py") {
        icon=QIcon::fromTheme("text-x-python");
    } else if (ext=="sql") {
        icon=QIcon::fromTheme("text-x-sql");
    } else if (ext=="pl") {
        icon=QIcon::fromTheme("application-x-perl");
    } else if (ext=="log") {
        icon=QIcon::fromTheme("text-x-log");
    } else {
        icon=QIcon::fromTheme("unknown");
    }
    return icon;
}

bool ConnectionManager::getDirectoryListing(QString dir, QTreeWidgetItem *qtree)
{

    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    char path[200];
    ssize_t rc;
    QTreeWidgetItem *curItem;

    /* resolve path */
    strcpy(path, dir.toLatin1().data());

    /* initialize SFTP session */
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        fprintf(stderr, "Unable to init SFTP session\n");
        return false;
    }

    /* open folder */
    sftp_handle = libssh2_sftp_opendir(sftp_session, path);
    if (!sftp_handle) {
        fprintf(stderr, "Unable to open file with SFTP: %ld\n",
                libssh2_sftp_last_error(sftp_session));
        libssh2_sftp_shutdown(sftp_session);
        return false;
    }

    /* receive listing */
    do {
        char mem[512];
        char longentry[512];
        LIBSSH2_SFTP_ATTRIBUTES attrs;

        /* loop until we fail */
        rc = libssh2_sftp_readdir_ex(sftp_handle, mem, sizeof(mem),
                                     longentry, sizeof(longentry), &attrs);
        if(rc > 0)
        {
            /* rc is the length of the file name in the mem
               buffer */
            mem[rc] = 0;
            if (!strcmp(mem, ".") || !strcmp(mem, "..")) {
                continue;
            }
            if (mem[0] == '.' && !_hidden) {
                continue;
            }
            curItem = new QTreeWidgetItem();
            curItem->setText(0, QString(mem));
            if (LIBSSH2_SFTP_S_ISDIR(attrs.permissions)) {
                curItem->setIcon(0, QIcon::fromTheme("folder"));
                curItem->addChild(new QTreeWidgetItem());
                curItem->setText(1, "D");
            } else {
                curItem->setIcon(0, fileIcon(QString(mem)));
                curItem->setText(1, "F");
            }
            curItem->setText(2, dir + QString("/") + QString(mem));
            qtree->addChild(curItem);
        }
        else
            break;

    } while (1);

    /* close file and session */
    libssh2_sftp_close(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

    /* done */
    return true;

}

QString ConnectionManager::getName()
{
    return _name;
}

QString ConnectionManager::getHost()
{
    return _host;
}

QString ConnectionManager::getPort()
{
    return _port;
}

QString ConnectionManager::getUser()
{
    return _user;
}

QString ConnectionManager::getPass()
{
    return _pass;
}

void ConnectionManager::setHidden(bool newVal)
{
    _hidden = newVal;
}
