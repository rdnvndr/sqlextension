#include <QtCore/QtGlobal>

#if defined(SQLEXTENSION_LIBRARY)
#  define SQLEXTENSIONLIB Q_DECL_EXPORT
#else
#  define SQLEXTENSIONLIB Q_DECL_IMPORT
#endif
