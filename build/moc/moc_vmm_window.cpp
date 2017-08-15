/****************************************************************************
** Meta object code from reading C++ file 'vmm_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/vmm_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vmm_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_vmm_window_t {
    QByteArrayData data[6];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vmm_window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vmm_window_t qt_meta_stringdata_vmm_window = {
    {
QT_MOC_LITERAL(0, 0, 10), // "vmm_window"
QT_MOC_LITERAL(1, 11, 18), // "updateChannelState"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 21), // "updateChannelVoltages"
QT_MOC_LITERAL(4, 53, 17), // "updateChannelADCs"
QT_MOC_LITERAL(5, 71, 14) // "updateSettings"

    },
    "vmm_window\0updateChannelState\0\0"
    "updateChannelVoltages\0updateChannelADCs\0"
    "updateSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vmm_window[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    1,   35,    2, 0x0a /* Public */,
       4,    1,   38,    2, 0x0a /* Public */,
       5,    0,   41,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void vmm_window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        vmm_window *_t = static_cast<vmm_window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateChannelState(); break;
        case 1: _t->updateChannelVoltages((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->updateChannelADCs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->updateSettings(); break;
        default: ;
        }
    }
}

const QMetaObject vmm_window::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_vmm_window.data,
      qt_meta_data_vmm_window,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *vmm_window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vmm_window::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_vmm_window.stringdata0))
        return static_cast<void*>(const_cast< vmm_window*>(this));
    return QWidget::qt_metacast(_clname);
}

int vmm_window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
