import "../main.qbs" as TestTmpl

TestTmpl {
    name: "tst_test_tmpl_prf"

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
}
