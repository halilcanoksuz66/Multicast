/****************************************************************************
** Meta object code from reading C++ file 'audiomanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../audiomanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiomanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
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
struct qt_meta_tag_ZN12AudioManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto AudioManager::qt_create_metaobjectdata<qt_meta_tag_ZN12AudioManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AudioManager",
        "inputAudioReady",
        "",
        "audioData",
        "audioStreamStarted",
        "audioStreamStopped",
        "audioError",
        "errorMessage",
        "handleOutputAudio",
        "sourceAddress",
        "emitInputAudioReady"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'inputAudioReady'
        QtMocHelpers::SignalData<void(const QByteArray &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 3 },
        }}),
        // Signal 'audioStreamStarted'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'audioStreamStopped'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'audioError'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'handleOutputAudio'
        QtMocHelpers::SlotData<void(const QByteArray &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 3 }, { QMetaType::QString, 9 },
        }}),
        // Slot 'emitInputAudioReady'
        QtMocHelpers::SlotData<void(const QByteArray &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AudioManager, qt_meta_tag_ZN12AudioManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AudioManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AudioManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AudioManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12AudioManagerE_t>.metaTypes,
    nullptr
} };

void AudioManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AudioManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->inputAudioReady((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 1: _t->audioStreamStarted(); break;
        case 2: _t->audioStreamStopped(); break;
        case 3: _t->audioError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->handleOutputAudio((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->emitInputAudioReady((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AudioManager::*)(const QByteArray & )>(_a, &AudioManager::inputAudioReady, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AudioManager::*)()>(_a, &AudioManager::audioStreamStarted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AudioManager::*)()>(_a, &AudioManager::audioStreamStopped, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AudioManager::*)(const QString & )>(_a, &AudioManager::audioError, 3))
            return;
    }
}

const QMetaObject *AudioManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AudioManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AudioManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AudioManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void AudioManager::inputAudioReady(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AudioManager::audioStreamStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AudioManager::audioStreamStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AudioManager::audioError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
