import "../main.qbs" as SqlExtensionProduct

SqlExtensionProduct {
    name: "tst_uuidtreemodel_mnl"

    productname: ""
    version:     "1.0.0"
    description: ""
    company:     "RTPTechGroup"
    copyright:   "Copyright (C) RTPTechGroup"

    Depends { name: "Qt"; submodules: ["core", "gui", "widgets", "sql"] }
    Depends { name: "sqlextension" }

    files: [
        "*.h",
        "*.cpp",
        "*.ui",
        "*.qrc"
    ]
    
    Group {
        name: "Runtime resources"
        files: "*.db"
        qbs.install: true
        qbs.installDir: "tests/manual/sqlextension/"
    }
}
