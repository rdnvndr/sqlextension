import qbs.File
import qbs.FileInfo

import "../main.qbs" as SrcProduct
SrcProduct {
    name: "libs"
    type: "dynamiclibrary"
    
    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.pubIncludePath
        cpp.rpaths: [product.rLibraryPath, product.rLibraryTestPath]
    }


    Group {
        overrideTags: false
        fileTagsFilter: [
            "dynamiclibrary",
            "dynamiclibrary_symlink",
            "dynamiclibrary_import"
        ]
        qbs.install: true
        qbs.installDir: product.applicationPath + "/" + product.rLibraryPath
    }


    FileTagger {
        condition:  publicInclude.value.length > 0
        patterns: publicInclude.value
        fileTags: ["publicInclude"]
    }

    Group {
        fileTagsFilter: ["publicInclude"]
        qbs.install: true
        qbs.installDir: product.includePath
    }

    Probe {
        id: publicInclude
        property stringList value
        property string prjDir: project.sourceDirectory
        property string incDir: product.pubIncludePath + "/" + product.name

        property int hack: {
            //A bit of a hack to make qbs re-resolve (see QBS-996)
            return  File.lastModified(prjDir + "/.git/logs/HEAD");
        }

        configure: {
            var list = File.directoryEntries(incDir, File.Files);
            for(var dir in list) {
                list[dir] = FileInfo.fileName(list[dir]);
            }
            value = list;
        }
    }    
}
