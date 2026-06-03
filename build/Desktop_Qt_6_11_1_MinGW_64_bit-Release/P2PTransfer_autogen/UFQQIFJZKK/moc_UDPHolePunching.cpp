/****************************************************************************
** Meta object code from reading C++ file 'UDPHolePunching.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/network/UDPHolePunching.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UDPHolePunching.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15UDPHolePunchingE_t {};
} // unnamed namespace

template <> constexpr inline auto UDPHolePunching::qt_create_metaobjectdata<qt_meta_tag_ZN15UDPHolePunchingE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "UDPHolePunching",
        "holePunchReceived",
        "",
        "peerId",
        "QHostAddress",
        "address",
        "port",
        "data",
        "peerDiscovered",
        "onReadyRead",
        "onCleanupTimer"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'holePunchReceived'
        QtMocHelpers::SignalData<void(const QString &, const QHostAddress &, quint16, const QByteArray &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 }, { QMetaType::UShort, 6 }, { QMetaType::QByteArray, 7 },
        }}),
        // Signal 'peerDiscovered'
        QtMocHelpers::SignalData<void(const QString &, const QHostAddress &, quint16)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 }, { QMetaType::UShort, 6 },
        }}),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCleanupTimer'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<UDPHolePunching, qt_meta_tag_ZN15UDPHolePunchingE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject UDPHolePunching::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15UDPHolePunchingE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15UDPHolePunchingE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15UDPHolePunchingE_t>.metaTypes,
    nullptr
} };

void UDPHolePunching::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UDPHolePunching *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->holePunchReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[4]))); break;
        case 1: _t->peerDiscovered((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[3]))); break;
        case 2: _t->onReadyRead(); break;
        case 3: _t->onCleanupTimer(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (UDPHolePunching::*)(const QString & , const QHostAddress & , quint16 , const QByteArray & )>(_a, &UDPHolePunching::holePunchReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (UDPHolePunching::*)(const QString & , const QHostAddress & , quint16 )>(_a, &UDPHolePunching::peerDiscovered, 1))
            return;
    }
}

const QMetaObject *UDPHolePunching::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UDPHolePunching::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15UDPHolePunchingE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UDPHolePunching::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void UDPHolePunching::holePunchReceived(const QString & _t1, const QHostAddress & _t2, quint16 _t3, const QByteArray & _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 1
void UDPHolePunching::peerDiscovered(const QString & _t1, const QHostAddress & _t2, quint16 _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
