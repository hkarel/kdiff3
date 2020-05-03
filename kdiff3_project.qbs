import qbs
import "qbs/imports/QbsUtl/qbsutl.js" as QbsUtl

Project {
    name: "KDiff3"
    minimumQbsVersion: "1.15.0"
    qbsSearchPaths: ["qbs"]

    readonly property string minimumQtVersion: "5.12.5"
    //readonly property bool conversionWarnEnabled: true

    readonly property var projectVersion: projectProbe.projectVersion
    readonly property string projectGitRevision: projectProbe.projectGitRevision

    Probe {
        id: projectProbe
        property var projectVersion;
        property string projectGitRevision;

        readonly property string projectBuildDirectory:  buildDirectory
        readonly property string projectSourceDirectory: sourceDirectory

        configure: {
            projectVersion = QbsUtl.getVersions(projectSourceDirectory + "/VERSION");
            projectGitRevision = QbsUtl.gitRevision(projectSourceDirectory);
            //if (File.exists(projectBuildDirectory + "/package_build_info"))
            //    File.remove(projectBuildDirectory + "/package_build_info")
        }
    }

    property var cppDefines: {
        var def = [
            "KDIFF3_VERSION_STRING=\"" + projectVersion[0] + "\"",
            "KDIFF3_VERSION_MAJOR=" + projectVersion[1],
            "KDIFF3_VERSION_MINOR=" + projectVersion[2],
            "KDIFF3_VERSION_PATCH=" + projectVersion[3],
            "KDIFF3_VERSION ((KDIFF3_VERSION_MAJOR<<16)|(KDIFF3_VERSION_MINOR<<8)|(KDIFF3_VERSION_PATCH))",
            "GIT_REVISION=\"" + projectGitRevision + "\"",
        ];

        if (qbs.buildVariant === "release")
            def.push("NDEBUG");

        return def;
    }

    property var cxxFlags: {
        var cxx = [
            "-Wall",
            "-Wextra",
            "-Wno-trigraphs",
            "-Wduplicated-cond",
            "-Wduplicated-branches",
            "-Wshadow",
            //"-Wno-non-virtual-dtor",
            //"-Wno-long-long",
            //"-pedantic",
        ];
        if (qbs.buildVariant === "debug")
            cxx.push("-ggdb3");
        else
            cxx.push("-s");

        //if (project.conversionWarnEnabled)
        //    cxx.push("-Wconversion");

        return cxx;
    }
    property string cxxLanguageVersion: "c++11"

    references: [
        "src/kdiff3.qbs",
    ]
}
