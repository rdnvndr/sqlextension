import "../main.qbs" as LibProduct
LibProduct {
    name: "test_tmpl"
    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: "tests/benchmarks/test_tmpl/"
    }
}
