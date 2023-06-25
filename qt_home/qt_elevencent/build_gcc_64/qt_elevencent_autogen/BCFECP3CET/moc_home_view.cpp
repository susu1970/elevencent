/****************************************************************************
** Meta object code from reading C++ file 'home_view.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/view/home/home_view.h"
#include <QtGui/qtextcursor.h>
#include <QScreen>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'home_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qt_elevencent__HomeView_t {
    const uint offsetsAndSize[8];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__HomeView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__HomeView_t qt_meta_stringdata_qt_elevencent__HomeView = {
    {
QT_MOC_LITERAL(0, 23), // "qt_elevencent::HomeView"
QT_MOC_LITERAL(24, 15), // "onBottomClicked"
QT_MOC_LITERAL(40, 0), // ""
QT_MOC_LITERAL(41, 3) // "idx"

    },
    "qt_elevencent::HomeView\0onBottomClicked\0"
    "\0idx"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__HomeView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x0a,    1 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void qt_elevencent::HomeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HomeView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onBottomClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject qt_elevencent::HomeView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__HomeView.offsetsAndSize,
    qt_meta_data_qt_elevencent__HomeView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__HomeView_t
, QtPrivate::TypeAndForceComplete<HomeView, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>


>,
    nullptr
} };


const QMetaObject *qt_elevencent::HomeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::HomeView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__HomeView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int qt_elevencent::HomeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_qt_elevencent__HomeBottomView_t {
    const uint offsetsAndSize[8];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__HomeBottomView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__HomeBottomView_t qt_meta_stringdata_qt_elevencent__HomeBottomView = {
    {
QT_MOC_LITERAL(0, 29), // "qt_elevencent::HomeBottomView"
QT_MOC_LITERAL(30, 17), // "onBtnGroupClicked"
QT_MOC_LITERAL(48, 0), // ""
QT_MOC_LITERAL(49, 3) // "idx"

    },
    "qt_elevencent::HomeBottomView\0"
    "onBtnGroupClicked\0\0idx"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__HomeBottomView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x0a,    1 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void qt_elevencent::HomeBottomView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HomeBottomView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onBtnGroupClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject qt_elevencent::HomeBottomView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__HomeBottomView.offsetsAndSize,
    qt_meta_data_qt_elevencent__HomeBottomView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__HomeBottomView_t
, QtPrivate::TypeAndForceComplete<HomeBottomView, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>


>,
    nullptr
} };


const QMetaObject *qt_elevencent::HomeBottomView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::HomeBottomView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__HomeBottomView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int qt_elevencent::HomeBottomView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_qt_elevencent__HomeContentView_t {
    const uint offsetsAndSize[2];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__HomeContentView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__HomeContentView_t qt_meta_stringdata_qt_elevencent__HomeContentView = {
    {
QT_MOC_LITERAL(0, 30) // "qt_elevencent::HomeContentView"

    },
    "qt_elevencent::HomeContentView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__HomeContentView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void qt_elevencent::HomeContentView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject qt_elevencent::HomeContentView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__HomeContentView.offsetsAndSize,
    qt_meta_data_qt_elevencent__HomeContentView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__HomeContentView_t
, QtPrivate::TypeAndForceComplete<HomeContentView, std::true_type>



>,
    nullptr
} };


const QMetaObject *qt_elevencent::HomeContentView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::HomeContentView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__HomeContentView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int qt_elevencent::HomeContentView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
