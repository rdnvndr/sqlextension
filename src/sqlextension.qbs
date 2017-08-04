import "./main.qbs" as LibProduct

LibProduct {
    name: "sqlextension"

    productname: "SqlExtension Library"
    version:     "1.0.0"
    description: "SqlExtension Library"
    company:     "RTPTechGroup"
    copyright:   "Copyright (C) RTPTechGroup"

    cpp.defines: base.concat(["SQLEXTENSION_LIBRARY"])
    Depends { name: "Qt"; submodules: ["widgets", "sql", "core"] }

    files: [
        "*.h",
        "*.cpp",
        "*.ui",
        "*.qrc"
    ]
}
