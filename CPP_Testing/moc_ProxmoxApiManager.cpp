/****************************************************************************
** Meta object code from reading C++ file 'ProxmoxApiManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "ProxmoxApiManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProxmoxApiManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProxmoxApiManager_t {
    QByteArrayData data[23];
    char stringdata0[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProxmoxApiManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProxmoxApiManager_t qt_meta_stringdata_ProxmoxApiManager = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ProxmoxApiManager"
QT_MOC_LITERAL(1, 18, 12), // "loginSuccess"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 12), // "loginFailure"
QT_MOC_LITERAL(4, 45, 6), // "reason"
QT_MOC_LITERAL(5, 52, 11), // "vmListReady"
QT_MOC_LITERAL(6, 64, 11), // "QVector<Vm>"
QT_MOC_LITERAL(7, 76, 3), // "vms"
QT_MOC_LITERAL(8, 80, 13), // "actionSuccess"
QT_MOC_LITERAL(9, 94, 7), // "message"
QT_MOC_LITERAL(10, 102, 7), // "doLogin"
QT_MOC_LITERAL(11, 110, 4), // "host"
QT_MOC_LITERAL(12, 115, 8), // "username"
QT_MOC_LITERAL(13, 124, 5), // "realm"
QT_MOC_LITERAL(14, 130, 8), // "password"
QT_MOC_LITERAL(15, 139, 11), // "fetchVmList"
QT_MOC_LITERAL(16, 151, 11), // "setVmFolder"
QT_MOC_LITERAL(17, 163, 4), // "vmid"
QT_MOC_LITERAL(18, 168, 10), // "folderName"
QT_MOC_LITERAL(19, 179, 15), // "performVmAction"
QT_MOC_LITERAL(20, 195, 6), // "action"
QT_MOC_LITERAL(21, 202, 2), // "Vm"
QT_MOC_LITERAL(22, 205, 7) // "vm_data"

    },
    "ProxmoxApiManager\0loginSuccess\0\0"
    "loginFailure\0reason\0vmListReady\0"
    "QVector<Vm>\0vms\0actionSuccess\0message\0"
    "doLogin\0host\0username\0realm\0password\0"
    "fetchVmList\0setVmFolder\0vmid\0folderName\0"
    "performVmAction\0action\0Vm\0vm_data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProxmoxApiManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    1,   55,    2, 0x06 /* Public */,
       5,    1,   58,    2, 0x06 /* Public */,
       8,    1,   61,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    4,   64,    2, 0x0a /* Public */,
      15,    0,   73,    2, 0x0a /* Public */,
      16,    2,   74,    2, 0x0a /* Public */,
      19,    3,   79,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   11,   12,   13,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   17,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, 0x80000000 | 21,   20,   17,   22,

       0        // eod
};

void ProxmoxApiManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProxmoxApiManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->loginSuccess(); break;
        case 1: _t->loginFailure((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->vmListReady((*reinterpret_cast< const QVector<Vm>(*)>(_a[1]))); break;
        case 3: _t->actionSuccess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->doLogin((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 5: _t->fetchVmList(); break;
        case 6: _t->setVmFolder((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: _t->performVmAction((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const Vm(*)>(_a[3]))); break;
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
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Vm >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProxmoxApiManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProxmoxApiManager::loginSuccess)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProxmoxApiManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProxmoxApiManager::loginFailure)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProxmoxApiManager::*)(const QVector<Vm> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProxmoxApiManager::vmListReady)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProxmoxApiManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProxmoxApiManager::actionSuccess)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProxmoxApiManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ProxmoxApiManager.data,
    qt_meta_data_ProxmoxApiManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProxmoxApiManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProxmoxApiManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProxmoxApiManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProxmoxApiManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ProxmoxApiManager::loginSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ProxmoxApiManager::loginFailure(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProxmoxApiManager::vmListReady(const QVector<Vm> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProxmoxApiManager::actionSuccess(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
