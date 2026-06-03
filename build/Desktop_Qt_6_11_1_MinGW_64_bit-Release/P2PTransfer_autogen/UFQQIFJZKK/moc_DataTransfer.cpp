/****************************************************************************
** Meta object code from reading C++ file 'DataTransfer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/network/DataTransfer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataTransfer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12DataTransferE_t {};
} // unnamed namespace

template <> constexpr inline auto DataTransfer::qt_create_metaobjectdata<qt_meta_tag_ZN12DataTransferE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DataTransfer",
        "transferStarted",
        "",
        "QUuid",
        "transferId",
        "transferProgress",
        "progress",
        "transferCompleted",
        "filePath",
        "transferFailed",
        "error",
        "transferCancelled",
        "onConnectionDataReceived",
        "data",
        "onConnectionConnected",
        "onConnectionDisconnected",
        "onConnectionError"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'transferStarted'
        QtMocHelpers::SignalData<void(const QUuid &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'transferProgress'
        QtMocHelpers::SignalData<void(const QUuid &, double)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Double, 6 },
        }}),
        // Signal 'transferCompleted'
        QtMocHelpers::SignalData<void(const QUuid &, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'transferFailed'
        QtMocHelpers::SignalData<void(const QUuid &, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'transferCancelled'
        QtMocHelpers::SignalData<void(const QUuid &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onConnectionDataReceived'
        QtMocHelpers::SlotData<void(const QByteArray &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 13 },
        }}),
        // Slot 'onConnectionConnected'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionDisconnected'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionError'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DataTransfer, qt_meta_tag_ZN12DataTransferE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DataTransfer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DataTransferE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DataTransferE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12DataTransferE_t>.metaTypes,
    nullptr
} };

void DataTransfer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DataTransfer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->transferStarted((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1]))); break;
        case 1: _t->transferProgress((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 2: _t->transferCompleted((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->transferFailed((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->transferCancelled((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1]))); break;
        case 5: _t->onConnectionDataReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 6: _t->onConnectionConnected(); break;
        case 7: _t->onConnectionDisconnected(); break;
        case 8: _t->onConnectionError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DataTransfer::*)(const QUuid & )>(_a, &DataTransfer::transferStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataTransfer::*)(const QUuid & , double )>(_a, &DataTransfer::transferProgress, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataTransfer::*)(const QUuid & , const QString & )>(_a, &DataTransfer::transferCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataTransfer::*)(const QUuid & , const QString & )>(_a, &DataTransfer::transferFailed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataTransfer::*)(const QUuid & )>(_a, &DataTransfer::transferCancelled, 4))
            return;
    }
}

const QMetaObject *DataTransfer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataTransfer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DataTransferE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataTransfer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DataTransfer::transferStarted(const QUuid & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DataTransfer::transferProgress(const QUuid & _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void DataTransfer::transferCompleted(const QUuid & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void DataTransfer::transferFailed(const QUuid & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void DataTransfer::transferCancelled(const QUuid & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
