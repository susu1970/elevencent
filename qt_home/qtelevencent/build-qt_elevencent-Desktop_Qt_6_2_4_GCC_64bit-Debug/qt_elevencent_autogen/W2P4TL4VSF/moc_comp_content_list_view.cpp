/****************************************************************************
** Meta object code from reading C++ file 'comp_content_list_view.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../qt_elevencent/src/view/comp/comp_content_list_view.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'comp_content_list_view.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qt_elevencent__CompContentListModel_t {
    const uint offsetsAndSize[2];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__CompContentListModel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__CompContentListModel_t qt_meta_stringdata_qt_elevencent__CompContentListModel = {
    {
QT_MOC_LITERAL(0, 35) // "qt_elevencent::CompContentLis..."

    },
    "qt_elevencent::CompContentListModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__CompContentListModel[] = {

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

void qt_elevencent::CompContentListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject qt_elevencent::CompContentListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__CompContentListModel.offsetsAndSize,
    qt_meta_data_qt_elevencent__CompContentListModel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__CompContentListModel_t
, QtPrivate::TypeAndForceComplete<CompContentListModel, std::true_type>



>,
    nullptr
} };


const QMetaObject *qt_elevencent::CompContentListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::CompContentListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__CompContentListModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int qt_elevencent::CompContentListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_qt_elevencent__CompContentListView_t {
    const uint offsetsAndSize[2];
    char stringdata0[35];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_qt_elevencent__CompContentListView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_qt_elevencent__CompContentListView_t qt_meta_stringdata_qt_elevencent__CompContentListView = {
    {
QT_MOC_LITERAL(0, 34) // "qt_elevencent::CompContentLis..."

    },
    "qt_elevencent::CompContentListView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qt_elevencent__CompContentListView[] = {

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

void qt_elevencent::CompContentListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject qt_elevencent::CompContentListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QListView::staticMetaObject>(),
    qt_meta_stringdata_qt_elevencent__CompContentListView.offsetsAndSize,
    qt_meta_data_qt_elevencent__CompContentListView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_qt_elevencent__CompContentListView_t
, QtPrivate::TypeAndForceComplete<CompContentListView, std::true_type>



>,
    nullptr
} };


const QMetaObject *qt_elevencent::CompContentListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qt_elevencent::CompContentListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qt_elevencent__CompContentListView.stringdata0))
        return static_cast<void*>(this);
    return QListView::qt_metacast(_clname);
}

int qt_elevencent::CompContentListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
