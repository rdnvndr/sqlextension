import "../main.qbs" as TestProduct
TestProduct {
    name: "manual"
    type: "application"
    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: "tests/manual/sqlextension/"
    }
}