import "../main.qbs" as DocProduct

DocProduct {
    name: "sqlextension-api"
    type: "docs"

    docName: "sqlextension-dev"
    Group {
        fileTagsFilter: ["docs"]
        qbs.install: true
        qbs.installDir: product.docPath
    }
}
