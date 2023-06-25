/****************************************************************************
** Meta object code from reading C++ file 'net_thread.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/controller/net/net_thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'net_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qt_elevencent__NetThread_t {
    const uint offsetsAndSize[40];
    char stringdata0[296];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__NetThread_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__NetThread_t qt_meta_stringdata_qt_elevencent__NetThread = {
    {
QT_MOC_LITERAL(0, 24), // "qt_elevencent::NetThread"
QT_MOC_LITERAL(25, 11), // "timerUpdate"
QT_MOC_LITERAL(37, 0), // ""
QT_MOC_LITERAL(38, 12), // "onReadyWrite"
QT_MOC_LITERAL(51, 7), // "pushOut"
QT_MOC_LITERAL(59, 19), // "NetModel::STATE_OUT"
QT_MOC_LITERAL(79, 5), // "state"
QT_MOC_LITERAL(85, 19), // "NetModel::StateCtx*"
QT_MOC_LITERAL(105, 10), // "pushServer"
QT_MOC_LITERAL(116, 22), // "NetModel::Server::Host"
QT_MOC_LITERAL(139, 1), // "h"
QT_MOC_LITERAL(141, 11), // "onConnected"
QT_MOC_LITERAL(153, 14), // "onDisConnected"
QT_MOC_LITERAL(168, 14), // "onStateChanged"
QT_MOC_LITERAL(183, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(212, 13), // "errorOccurred"
QT_MOC_LITERAL(226, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(255, 11), // "socketError"
QT_MOC_LITERAL(267, 11), // "onReadyRead"
QT_MOC_LITERAL(279, 16) // "reconnect2server"

    },
    "qt_elevencent::NetThread\0timerUpdate\0"
    "\0onReadyWrite\0pushOut\0NetModel::STATE_OUT\0"
    "state\0NetModel::StateCtx*\0pushServer\0"
    "NetModel::Server::Host\0h\0onConnected\0"
    "onDisConnected\0onStateChanged\0"
    "QAbstractSocket::SocketState\0errorOccurred\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "onReadyRead\0reconnect2server"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__NetThread[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x08,    1 /* Private */,
       3,    0,   75,    2, 0x0a,    2 /* Public */,
       4,    2,   76,    2, 0x0a,    3 /* Public */,
       8,    1,   81,    2, 0x0a,    6 /* Public */,
      11,    0,   84,    2, 0x0a,    8 /* Public */,
      12,    0,   85,    2, 0x0a,    9 /* Public */,
      13,    1,   86,    2, 0x0a,   10 /* Public */,
      15,    1,   89,    2, 0x0a,   12 /* Public */,
      18,    0,   92,    2, 0x0a,   14 /* Public */,
      19,    0,   93,    2, 0x0a,   15 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7,    6,    2,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,    2,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void qt_elevencent::NetThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->timerUpdate(); break;
        case 1: _t->onReadyWrite(); break;
        case 2: _t->pushOut((*reinterpret_cast< std::add_pointer_t<NetModel::STATE_OUT>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<NetModel::StateCtx*>>(_a[2]))); break;
        case 3: _t->pushServer((*reinterpret_cast< std::add_pointer_t<NetModel::Server::Host>>(_a[1]))); break;
        case 4: _t->onConnected(); break;
        case 5: _t->onDisConnected(); break;
        case 6: _t->onStateChanged((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketState>>(_a[1]))); break;
        case 7: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 8: _t->onReadyRead(); break;
        case 9: _t->reconnect2server(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketState >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject qt_elevencent::NetThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__NetThread.offsetsAndSize,
    qt_meta_data_qt_elevencent__NetThread,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__NetThread_t
, QtPrivate::TypeAndForceComplete<NetThread, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NetModel::STATE_OUT, std::false_type>, QtPrivate::TypeAndForceComplete<NetModel::StateCtx *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NetModel::Server::Host, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketState, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *qt_elevencent::NetThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::NetThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__NetThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int qt_elevencent::NetThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
