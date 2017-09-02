import "../main.qbs" as MainProduct

import qbs;
import qbs.Process;
import qbs.TextFile;

MainProduct {
   builtByDefault: false

   FileTagger {
       fileTags: ["docs"]
       patterns: ["*.qch"]
   }

   Group {
       name: "markdown"
       files: ["pages/*.markdown"]
       fileTags: ["markdown"]
   }

   Group {
       name: "doxygen"
       files: [product.sourceDirectory + "/Doxyfile"]
       fileTags: ["doxyconf"]
   }

   property string docName: "doc"
   Rule {
       multiplex: true
       inputs: ["doxyconf"]
       outputFileTags: ["docs"]
       outputArtifacts: [{ filePath: product.docName + ".qch" },
                         { filePath: product.docName + "/" }]
       prepare: {
           var cmd = new JavaScriptCommand ();
           cmd.description = "generating api documentation";
           cmd.highlight   = "doxygen";
           cmd.sourceCode  = function () {
               for (var idx = 0; idx < inputs["doxyconf"].length; idx++) {
                   var file = inputs["doxyconf"][idx].filePath;

                   var doxFile = new TextFile(file);
                   var content = doxFile.readAll();
                   doxFile.close();

                   var proc = new Process ();
                   proc.setWorkingDirectory (product.sourceDirectory);
                   proc.start ("doxygen",["-"]);
                   proc.write(content);
                   proc.writeLine("HTML_OUTPUT = " + product.docName + "/");
                   proc.writeLine("QCH_FILE = ../" + product.docName + ".qch");
                   proc.writeLine("OUTPUT_DIRECTORY = " + product.buildDirectory);
                   proc.closeWriteChannel();
                   if (!proc.waitForFinished())
                       proc.kill();
                   proc.close();
               }
           }
           return cmd;
       }
   }
}
