/****************************************************************************
** Meta object code from reading C++ file 'P2PManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/network/P2PManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'P2PManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10P2PManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto P2PManager::qt_create_metaobjectdata<qt_meta_tag_ZN10P2PManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "P2PManager",
        "peerConnected",
        "",
        "peerId",
        "address",
        "peerDisconnected",
        "fileReceived",
        "filePath",
        "fromPeer",
        "transferProgress",
        "QUuid",
        "transferId",
        "progress",
        "transferCompleted",
        "transferFailed",
        "error",
        "onNewConnection",
        "onPeerDataReceived",
        "data",
        "onPeerConnected",
        "onPeerDisconnected",
        "onPeerError",
        "onHolePunchReceived",
        "QHostAddress",
        "port",
        "onDataTransferProgress",
        "onDataTransferCompleted",
        "onDataTransferFailed"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'peerConnected'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'peerDisconnected'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'fileReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'transferProgress'
        QtMocHelpers::SignalData<void(const QUuid &, double)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::Double, 12 },
        }}),
        // Signal 'transferCompleted'
        QtMocHelpers::SignalData<void(const QUuid &, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'transferFailed'
        QtMocHelpers::SignalData<void(const QUuid &, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::QString, 15 },
        }}),
        // Slot 'onNewConnection'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPeerDataReceived'
        QtMocHelpers::SlotData<void(const QByteArray &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 18 },
        }}),
        // Slot 'onPeerConnected'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPeerDisconnected'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPeerError'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'onHolePunchReceived'
        QtMocHelpers::SlotData<void(const QString &, const QHostAddress &, quint16, const QByteArray &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 23, 4 }, { QMetaType::UShort, 24 }, { QMetaType::QByteArray, 18 },
        }}),
        // Slot 'onDataTransferProgress'
        QtMocHelpers::SlotData<void(const QUuid &, double)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::Double, 12 },
        }}),
        // Slot 'onDataTransferCompleted'
        QtMocHelpers::SlotData<void(const QUuid &, const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onDataTransferFailed'
        QtMocHelpers::SlotData<void(const QUuid &, const QString &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::QString, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<P2PManager, qt_meta_tag_ZN10P2PManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject P2PManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10P2PManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10P2PManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10P2PManagerE_t>.metaTypes,
    nullptr
} };

void P2PManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<P2PManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->peerConnected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->peerDisconnected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->fileReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->transferProgress((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 4: _t->transferCompleted((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->transferFailed((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->onNewConnection(); break;
        case 7: _t->onPeerDataReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 8: _t->onPeerConnected(); break;
        case 9: _t->onPeerDisconnected(); break;
        case 10: _t->onPeerError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onHolePunchReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[4]))); break;
        case 12: _t->onDataTransferProgress((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 13: _t->onDataTransferCompleted((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->onDataTransferFailed((*reinterpret_cast<std::add_pointer_t<QUuid>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (P2PManager::*)(const QString & , const QString & )>(_a, &P2PManager::peerConnected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (P2PManager::*)(const QString & )>(_a, &P2PManager::peerDisconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (P2PManager::*)(const QString & , const QString & )>(_a, &P2PManager::fileReceived, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (P2PManager::*)(const QUuid & , double )>(_a, &P2PManager::transferProgress, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (P2PManager::*)(const QUuid & , const QString & )>(_a, &P2PManager::transferCompleted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (P2PManager::*)(const QUuid & , const QString & )>(_a, &P2PManager::transferFailed, 5))
            return;
    }
}

const QMetaObject *P2PManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *P2PManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10P2PManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int P2PManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void P2PManager::peerConnected(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void P2PManager::peerDisconnected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void P2PManager::fileReceived(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void P2PManager::transferProgress(const QUuid & _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void P2PManager::transferCompleted(const QUuid & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void P2PManager::transferFailed(const QUuid & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}
QT_WARNING_POP
