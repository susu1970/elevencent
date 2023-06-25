/****************************************************************************
** Meta object code from reading C++ file 'net_controller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/controller/net/net_controller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'net_controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qt_elevencent__NetController_t {
    const uint offsetsAndSize[22];
    char stringdata0[141];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__NetController_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__NetController_t qt_meta_stringdata_qt_elevencent__NetController = {
    {
QT_MOC_LITERAL(0, 28), // "qt_elevencent::NetController"
QT_MOC_LITERAL(29, 16), // "reconnect2server"
QT_MOC_LITERAL(46, 0), // ""
QT_MOC_LITERAL(47, 10), // "pushServer"
QT_MOC_LITERAL(58, 22), // "NetModel::Server::Host"
QT_MOC_LITERAL(81, 1), // "h"
QT_MOC_LITERAL(83, 7), // "pushOut"
QT_MOC_LITERAL(91, 19), // "NetModel::STATE_OUT"
QT_MOC_LITERAL(111, 5), // "state"
QT_MOC_LITERAL(117, 19), // "NetModel::StateCtx*"
QT_MOC_LITERAL(137, 3) // "ctx"

    },
    "qt_elevencent::NetController\0"
    "reconnect2server\0\0pushServer\0"
    "NetModel::Server::Host\0h\0pushOut\0"
    "NetModel::STATE_OUT\0state\0NetModel::StateCtx*\0"
    "ctx"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__NetController[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    1 /* Public */,
       3,    1,   33,    2, 0x06,    2 /* Public */,
       6,    2,   36,    2, 0x06,    4 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 9,    8,   10,

       0        // eod
};

void qt_elevencent::NetController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->reconnect2server(); break;
        case 1: _t->pushServer((*reinterpret_cast< std::add_pointer_t<NetModel::Server::Host>>(_a[1]))); break;
        case 2: _t->pushOut((*reinterpret_cast< std::add_pointer_t<NetModel::STATE_OUT>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<NetModel::StateCtx*>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NetController::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetController::reconnect2server)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NetController::*)(NetModel::Server::Host );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetController::pushServer)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NetController::*)(NetModel::STATE_OUT , NetModel::StateCtx * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetController::pushOut)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject qt_elevencent::NetController::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseController::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__NetController.offsetsAndSize,
    qt_meta_data_qt_elevencent__NetController,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__NetController_t
, QtPrivate::TypeAndForceComplete<NetController, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NetModel::Server::Host, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<NetModel::STATE_OUT, std::false_type>, QtPrivate::TypeAndForceComplete<NetModel::StateCtx *, std::false_type>



>,
    nullptr
} };


const QMetaObject *qt_elevencent::NetController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::NetController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__NetController.stringdata0))
        return static_cast<void*>(this);
    return BaseController::qt_metacast(_clname);
}

int qt_elevencent::NetController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseController::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void qt_elevencent::NetController::reconnect2server()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void qt_elevencent::NetController::pushServer(NetModel::Server::Host _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qt_elevencent::NetController::pushOut(NetModel::STATE_OUT _t1, NetModel::StateCtx * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
