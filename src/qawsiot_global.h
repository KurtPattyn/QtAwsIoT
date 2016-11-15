#pragma once

#include <QtCore/QtGlobal>

#ifndef QT_STATIC
#  if defined(QT_BUILD_QTAWSIOT_LIB)
#    define QTAWSIOT_EXPORT Q_DECL_EXPORT
#    if defined(PRIVATE_TESTS_ENABLED)
#      define QTAWSIOT_AUTOTEST_EXPORT Q_DECL_EXPORT
#    endif
#  else
#    define QTAWSIOT_EXPORT Q_DECL_IMPORT
#    define QTAWSIOT_AUTOTEST_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QTAWSIOT_EXPORT
#  define QTAWSIOT_AUTOTEST_EXPORT
#endif
