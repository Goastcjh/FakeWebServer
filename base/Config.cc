#include "Config.h"

#define DEFAULT_THREADS 3                    //default number of io threads
#define DEFAULT_PORT "8888"                  //default listen port
#define DEFAULT_LISTENQ 2048                 //default tcp listen queue
#define DEFAULT_STORAGE "test/"              //default page storge
#define DEFAULT_KEEPALIVE 20000              //default http keep-alive time
#define DEFAULT_LOGFILE "log/WebServer.log"  //default logfile path
#define DEFAULT_CAPACITY 10                  //default cache size

inline void Conf::init(const char* path) {
    strncpy(conf, path, 99); //src->dest, 当src长度小于n时，dest的剩余部分将用空字节填充
    map<string, string> m;
    fstream file;
    file.open(conf, ios::in);
    string buf;
    while(!file.eof()) {
        getline(file, buf);
        pretreatment(buf);
        if("" == buf) {
            continue;
        }
        parse(m, buf);
    }
    file.close();

}

inline int Conf::getthreads() {
    return threads;
}

inline string Conf::getport() {
    return port;
}

inline int Conf::getlistenq() {
    return listenq;
}

inline string Conf::getstorage() {
    return storage;
}

inline string Conf::getlogfile() {
    return logfile;
}

inline int Conf::getkeepalive() {
    return keepalive;
}

inline int Conf::getcapacity() {
    return capacity;
}
//预处理，获取config文件中的配置信息，#后为注释，不读取
inline void Conf::pretreatment(string& buf) {
    int last = buf.find('#', 0);
    if (string::npos ==  last) {
        return;
    }
    buf = buf.substr(0, last);
    return;
}
//get position, 获取字符串头尾的指针位置，flag == true标识当前判断尾指针，flag == false标识当前判断头指针
inline int Conf::getpos(string& buf, int start, int end, bool flag) {
    for (; start < end; ++start) {
        if (flag) {
            if (' ' == buf[start] || '\t' == buf[start]) {
                return start;
            }
        }
        else {
            if (' ' != buf[start] || '\t' != buf[start]) {
                return start;
            }
        }
    }
}
//将获取得到的配置信息存入map中
inline void Conf::parse(map<string,string>& m, string& buf) {
    int keystart = getpos(buf, 0, buf.size(), false); //key-start
    int keyend = getpos(buf, keystart, buf.size(), true);//key-end
    int valstart = getpos(buf, keyend, buf.size(), false);//value-start
    int valend = getpos(buf, valstart, buf.size(), true);//value-end

    if (valstart != buf.size()) {
        m[buf.substr(keystart, keyend - keystart)] = buf.substr(valstart, valend - valstart);
    }
}

Conf& getconf() {
    static Conf conf;
    return conf;
}
