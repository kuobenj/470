/****************************************************************************
** Meta object code from reading C++ file 'ConsoleGUI.h'
**
** Created: Wed Nov 18 22:09:49 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ConsoleGUI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ConsoleGUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConsoleGUI[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,
      56,   38,   11,   11, 0x05,
     117,   83,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     162,   11,   11,   11, 0x0a,
     177,   12,   11,   11, 0x0a,
     198,   11,   11,   11, 0x0a,
     217,  210,   11,   11, 0x0a,
     234,   11,   11,   11, 0x0a,
     248,   11,   11,   11, 0x0a,
     262,   11,   11,   11, 0x0a,
     279,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ConsoleGUI[] = {
    "ConsoleGUI\0\0text\0textEntered(QString)\0"
    "mousex,mousey,pix\0mousePressed(int,int,QRgb)\0"
    "color,gray,bottomLeft,bottomRight\0"
    "imagesReady(QImage*,QImage*,QImage*,QImage*)\0"
    "receiveEnter()\0displayLine(QString)\0"
    "cleanExit()\0status\0consoleExit(int)\0"
    "snapPicture()\0loadPicture()\0"
    "toggleAutoSnap()\0refreshPictures()\0"
};

void ConsoleGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ConsoleGUI *_t = static_cast<ConsoleGUI *>(_o);
        switch (_id) {
        case 0: _t->textEntered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->mousePressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QRgb(*)>(_a[3]))); break;
        case 2: _t->imagesReady((*reinterpret_cast< QImage*(*)>(_a[1])),(*reinterpret_cast< QImage*(*)>(_a[2])),(*reinterpret_cast< QImage*(*)>(_a[3])),(*reinterpret_cast< QImage*(*)>(_a[4]))); break;
        case 3: _t->receiveEnter(); break;
        case 4: _t->displayLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->cleanExit(); break;
        case 6: _t->consoleExit((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->snapPicture(); break;
        case 8: _t->loadPicture(); break;
        case 9: _t->toggleAutoSnap(); break;
        case 10: _t->refreshPictures(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ConsoleGUI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ConsoleGUI::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ConsoleGUI,
      qt_meta_data_ConsoleGUI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConsoleGUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConsoleGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConsoleGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConsoleGUI))
        return static_cast<void*>(const_cast< ConsoleGUI*>(this));
    return QWidget::qt_metacast(_clname);
}

int ConsoleGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void ConsoleGUI::textEntered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ConsoleGUI::mousePressed(int _t1, int _t2, QRgb _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ConsoleGUI::imagesReady(QImage * _t1, QImage * _t2, QImage * _t3, QImage * _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
