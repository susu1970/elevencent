/****************************************************************************
** Meta object code from reading C++ file 'msg_controller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/controller/msg/msg_controller.h"
#include <QtGui/qtextcursor.h>
#include <QScreen>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'msg_controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qt_elevencent__MsgController_t {
    const uint offsetsAndSize[12];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__MsgController_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__MsgController_t qt_meta_stringdata_qt_elevencent__MsgController = {
    {
QT_MOC_LITERAL(0, 28), // "qt_elevencent::MsgController"
QT_MOC_LITERAL(29, 16), // "onMsgUserClicked"
QT_MOC_LITERAL(46, 0), // ""
QT_MOC_LITERAL(47, 11), // "QModelIndex"
QT_MOC_LITERAL(59, 13), // "reqMsgContent"
QT_MOC_LITERAL(73, 11) // "reqMsgUsers"

    },
    "qt_elevencent::MsgController\0"
    "onMsgUserClicked\0\0QModelIndex\0"
    "reqMsgContent\0reqMsgUsers"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__MsgController[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x0a,    1 /* Public */,
       4,    0,   35,    2, 0x0a,    3 /* Public */,
       5,    0,   36,    2, 0x0a,    4 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void qt_elevencent::MsgController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MsgController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMsgUserClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 1: _t->reqMsgContent(); break;
        case 2: _t->reqMsgUsers(); break;
        default: ;
        }
    }
}

const QMetaObject qt_elevencent::MsgController::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseController::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__MsgController.offsetsAndSize,
    qt_meta_data_qt_elevencent__MsgController,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__MsgController_t
, QtPrivate::TypeAndForceComplete<MsgController, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QModelIndex, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *qt_elevencent::MsgController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::MsgController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__MsgController.stringdata0))
        return static_cast<void*>(this);
    return BaseController::qt_metacast(_clname);
}

int qt_elevencent::MsgController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
