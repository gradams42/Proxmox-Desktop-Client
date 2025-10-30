/****************************************************************************
** Meta object code from reading C++ file 'ProxmoxClientWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "ProxmoxClientWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProxmoxClientWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProxmoxClientWindow_t {
    QByteArrayData data[19];
    char stringdata0[317];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProxmoxClientWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProxmoxClientWindow_t qt_meta_stringdata_ProxmoxClientWindow = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ProxmoxClientWindow"
QT_MOC_LITERAL(1, 20, 18), // "handleLoginSuccess"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 18), // "handleLoginFailure"
QT_MOC_LITERAL(4, 59, 6), // "reason"
QT_MOC_LITERAL(5, 66, 17), // "handleVmListReady"
QT_MOC_LITERAL(6, 84, 11), // "QVector<Vm>"
QT_MOC_LITERAL(7, 96, 3), // "vms"
QT_MOC_LITERAL(8, 100, 19), // "handleActionSuccess"
QT_MOC_LITERAL(9, 120, 7), // "message"
QT_MOC_LITERAL(10, 128, 22), // "on_loginButton_clicked"
QT_MOC_LITERAL(11, 151, 21), // "on_listButton_clicked"
QT_MOC_LITERAL(12, 173, 25), // "on_treeView_doubleClicked"
QT_MOC_LITERAL(13, 199, 11), // "QModelIndex"
QT_MOC_LITERAL(14, 211, 5), // "index"
QT_MOC_LITERAL(15, 217, 24), // "on_startVmButton_clicked"
QT_MOC_LITERAL(16, 242, 29), // "on_createFolderButton_clicked"
QT_MOC_LITERAL(17, 272, 40), // "on_vmTreeView_customContextMe..."
QT_MOC_LITERAL(18, 313, 3) // "pos"

    },
    "ProxmoxClientWindow\0handleLoginSuccess\0"
    "\0handleLoginFailure\0reason\0handleVmListReady\0"
    "QVector<Vm>\0vms\0handleActionSuccess\0"
    "message\0on_loginButton_clicked\0"
    "on_listButton_clicked\0on_treeView_doubleClicked\0"
    "QModelIndex\0index\0on_startVmButton_clicked\0"
    "on_createFolderButton_clicked\0"
    "on_vmTreeView_customContextMenuRequested\0"
    "pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProxmoxClientWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    1,   65,    2, 0x08 /* Private */,
       5,    1,   68,    2, 0x08 /* Private */,
       8,    1,   71,    2, 0x08 /* Private */,
      10,    0,   74,    2, 0x08 /* Private */,
      11,    0,   75,    2, 0x08 /* Private */,
      12,    1,   76,    2, 0x08 /* Private */,
      15,    0,   79,    2, 0x08 /* Private */,
      16,    0,   80,    2, 0x08 /* Private */,
      17,    1,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   18,

       0        // eod
};

void ProxmoxClientWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProxmoxClientWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->handleLoginSuccess(); break;
        case 1: _t->handleLoginFailure((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->handleVmListReady((*reinterpret_cast< const QVector<Vm>(*)>(_a[1]))); break;
        case 3: _t->handleActionSuccess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->on_loginButton_clicked(); break;
        case 5: _t->on_listButton_clicked(); break;
        case 6: _t->on_treeView_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: _t->on_startVmButton_clicked(); break;
        case 8: _t->on_createFolderButton_clicked(); break;
        case 9: _t->on_vmTreeView_customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<Vm> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProxmoxClientWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ProxmoxClientWindow.data,
    qt_meta_data_ProxmoxClientWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProxmoxClientWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProxmoxClientWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProxmoxClientWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ProxmoxClientWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
