#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T13:21:31
#
#-------------------------------------------------

QT       += core gui multimedia
QMAKE_CXXFLAGS = -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release

unix {
    LIBS += -L/lib -lfmod
}
win32 {
    INCLUDEPATH += C:/msys64/mingw32/include/
    LIBS += -L"C:/msys64/mingw32/lib/"

# adjust fmod location to your needs
#5cript mist hier ->
    INCLUDEPATH += "D:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api/lowlevel/inc"
    LIBS += -L"D:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api/lowlevel/lib/" -lfmod

#heradons mist hier ->
    INCLUDEPATH += "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api/lowlevel/inc"
    LIBS += -L"C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api/lowlevel/lib/" -lfmod
}

TARGET = touch-panel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    music.cpp \
    alarm.cpp \
    alarmpersistence.cpp \
    config.cpp \
    json_api/parse/jsd_generic_parser.cpp \
    json_api/parse/jsd_options.cpp \
    json_api/parse/jsd_string.cpp \
    json_api/stringify/jss_error.cpp \
    json_api/stringify/jss_object.cpp \
    json_api/stringify/jss_options.cpp \
    json_api/stringify/jss_pointer.cpp \
    json_api/stringify/jss_string.cpp \
    json_api/stringify/jss_void.cpp \
    json_api/stringify/jss_beautify.cpp \
    calendar.cpp \
    customplaylist.cpp \
    track.cpp \
    system_menu.cpp \
    system_info.cpp \
    fmodwrap.cpp \
    common_errors.cpp

HEADERS  += mainwindow.h \
    music.h \
    alarm.h \
    alarmpersistence.h \
    config.h \
    json_api/parse/jsd.h \
    json_api/parse/jsd_array.h \
    json_api/parse/jsd_atomic.h \
    json_api/parse/jsd_check.h \
    json_api/parse/jsd_container.h \
    json_api/parse/jsd_convenience.h \
    json_api/parse/jsd_core.h \
    json_api/parse/jsd_fundamental.h \
    json_api/parse/jsd_fusion_adapted_struct.h \
    json_api/parse/jsd_generic_parser.h \
    json_api/parse/jsd_map.h \
    json_api/parse/jsd_options.h \
    json_api/parse/jsd_pair.h \
    json_api/parse/jsd_set.h \
    json_api/parse/jsd_string.h \
    json_api/parse/jsd_unique_ptr.h \
    json_api/stringify/container_traits.h \
    json_api/stringify/integer_of_size.h \
    json_api/stringify/iterator_traits.h \
    json_api/stringify/jss.h \
    json_api/stringify/jss_array.h \
    json_api/stringify/jss_atomic.h \
    json_api/stringify/jss_bidirectional.h \
    json_api/stringify/jss_bitset.h \
    json_api/stringify/jss_check.h \
    json_api/stringify/jss_container.h \
    json_api/stringify/jss_convenience.h \
    json_api/stringify/jss_core.h \
    json_api/stringify/jss_deque.h \
    json_api/stringify/jss_error.h \
    json_api/stringify/jss_extended.h \
    json_api/stringify/jss_forward.h \
    json_api/stringify/jss_forward_list.h \
    json_api/stringify/jss_fundamental.h \
    json_api/stringify/jss_fusion_adapted_struct.h \
    json_api/stringify/jss_fusion_vector.h \
    json_api/stringify/jss_initializer_list.h \
    json_api/stringify/jss_iterator.h \
    json_api/stringify/jss_list.h \
    json_api/stringify/jss_map.h \
    json_api/stringify/jss_mutex.h \
    json_api/stringify/jss_object.h \
    json_api/stringify/jss_options.h \
    json_api/stringify/jss_pair.h \
    json_api/stringify/jss_pointer.h \
    json_api/stringify/jss_queue.h \
    json_api/stringify/jss_random_access.h \
    json_api/stringify/jss_set.h \
    json_api/stringify/jss_shared_ptr.h \
    json_api/stringify/jss_smart_ptr.h \
    json_api/stringify/jss_stack.h \
    json_api/stringify/jss_string.h \
    json_api/stringify/jss_struct.h \
    json_api/stringify/jss_tuple.h \
    json_api/stringify/jss_undefine.h \
    json_api/stringify/jss_unique_ptr.h \
    json_api/stringify/jss_valarray.h \
    json_api/stringify/jss_vector.h \
    json_api/stringify/jss_void.h \
    json_api/stringify/jss_weak_ptr.h \
    json_api/stringify/jss_beautify.h \
    calendar.h \
    customplaylist.h \
    track.h \
    system_menu.h \
    system_info.h \
    fmodresult.h \
    fmodwrap.h \
    common_errors.h

FORMS    += mainwindow.ui \
    music.ui \
    alarm.ui \
    calendar.ui \
    system_menu.ui \
    system_info.ui

TRANSLATIONS    += \
    shima_de.ts \
    shima_en.ts

RESOURCES += \
    include.qrc

DISTFILES += \
    COMPILE.txt \
    stylesheet.qss \
    keepalive.sh
