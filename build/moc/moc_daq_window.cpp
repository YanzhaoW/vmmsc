/****************************************************************************
** Meta object code from reading C++ file 'daq_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/daq_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'daq_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_daq_window_t {
    QByteArrayData data[12];
    char stringdata0[218];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_daq_window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_daq_window_t qt_meta_stringdata_daq_window = {
    {
QT_MOC_LITERAL(0, 0, 10), // "daq_window"
QT_MOC_LITERAL(1, 11, 19), // "on_Box_fec1_clicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 19), // "on_Box_fec2_clicked"
QT_MOC_LITERAL(4, 52, 19), // "on_Box_fec3_clicked"
QT_MOC_LITERAL(5, 72, 19), // "on_Box_fec4_clicked"
QT_MOC_LITERAL(6, 92, 19), // "on_Box_fec5_clicked"
QT_MOC_LITERAL(7, 112, 19), // "on_Box_fec6_clicked"
QT_MOC_LITERAL(8, 132, 19), // "on_Box_fec7_clicked"
QT_MOC_LITERAL(9, 152, 19), // "on_Box_fec8_clicked"
QT_MOC_LITERAL(10, 172, 22), // "on_Button_load_clicked"
QT_MOC_LITERAL(11, 195, 22) // "on_Button_save_clicked"

    },
    "daq_window\0on_Box_fec1_clicked\0\0"
    "on_Box_fec2_clicked\0on_Box_fec3_clicked\0"
    "on_Box_fec4_clicked\0on_Box_fec5_clicked\0"
    "on_Box_fec6_clicked\0on_Box_fec7_clicked\0"
    "on_Box_fec8_clicked\0on_Button_load_clicked\0"
    "on_Button_save_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_daq_window[] = {

 // content:
       7,       // revision
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
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void daq_window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        daq_window *_t = static_cast<daq_window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_Box_fec1_clicked(); break;
        case 1: _t->on_Box_fec2_clicked(); break;
        case 2: _t->on_Box_fec3_clicked(); break;
        case 3: _t->on_Box_fec4_clicked(); break;
        case 4: _t->on_Box_fec5_clicked(); break;
        case 5: _t->on_Box_fec6_clicked(); break;
        case 6: _t->on_Box_fec7_clicked(); break;
        case 7: _t->on_Box_fec8_clicked(); break;
        case 8: _t->on_Button_load_clicked(); break;
        case 9: _t->on_Button_save_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject daq_window::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_daq_window.data,
      qt_meta_data_daq_window,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *daq_window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *daq_window::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_daq_window.stringdata0))
        return static_cast<void*>(const_cast< daq_window*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int daq_window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
