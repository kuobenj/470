/****************************************************************************
** Meta object code from reading C++ file 'UserThread.h'
**
** Created: Wed Nov 18 22:09:49 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../UserThread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UserThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      35,   30,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      87,   53,   11,   11, 0x0a,
     134,   30,   11,   11, 0x0a,
     171,  153,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UserThread[] = {
    "UserThread\0\0imagesProcessed()\0text\0"
    "showText(QString)\0color,gray,bottomLeft,bottomRight\0"
    "processImages(QImage*,QImage*,QImage*,QImage*)\0"
    "textInput(QString)\0mousex,mousey,pix\0"
    "mousePress(int,int,QRgb)\0"
};

void UserThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UserThread *_t = static_cast<UserThread *>(_o);
        switch (_id) {
        case 0: _t->imagesProcessed(); break;
        case 1: _t->showText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->processImages((*reinterpret_cast< QImage*(*)>(_a[1])),(*reinterpret_cast< QImage*(*)>(_a[2])),(*reinterpret_cast< QImage*(*)>(_a[3])),(*reinterpret_cast< QImage*(*)>(_a[4]))); break;
        case 3: _t->textInput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->mousePress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QRgb(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UserThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UserThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_UserThread,
      qt_meta_data_UserThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UserThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UserThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UserThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserThread))
        return static_cast<void*>(const_cast< UserThread*>(this));
    return QThread::qt_metacast(_clname);
}

int UserThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void UserThread::imagesProcessed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void UserThread::showText(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
