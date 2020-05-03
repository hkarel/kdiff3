import qbs
import QbsUtl

Product {
    targetName: "kdiff3"

    type: "application"
    destinationDirectory: "./bin"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "widgets", "printsupport", "dbus", "network"] }

    cpp.defines: project.cppDefines

    cpp.cxxFlags: project.cxxFlags
    cpp.cxxLanguageVersion: project.cxxLanguageVersion

    cpp.includePaths: [
        "./",
    ]

    cpp.systemIncludePaths: QbsUtl.concatPaths(
        Qt.core.cpp.includePaths, // Suppression a Qt warnings
        "/usr/include/KF5",
        "/usr/include/KF5/KI18n",
        "/usr/include/KF5/KParts",
        "/usr/include/KF5/KCrash",
        "/usr/include/KF5/KIOCore",
        "/usr/include/KF5/KConfigCore",
        "/usr/include/KF5/KConfigGui",
        "/usr/include/KF5/KCoreAddons",
        "/usr/include/KF5/KXmlGui",
        "/usr/include/KF5/KIconThemes",
        "/usr/include/KF5/KWidgetsAddons",
        "/usr/include/KF5/KConfigWidgets",
        "/usr/include/KF5/KTextWidgets",
        "/usr/include/KF5/SonnetUi",
        "/usr/include/KF5/KIOWidgets",
        "/usr/include/KF5/KJobWidgets"
    )

//        Group {
//            name: "resources"
//            files: {
//                var files = [
//                    "icons.qrc"
//                ];
////                if (qbs.targetOS.contains('windows')
////                    && qbs.toolchain && qbs.toolchain.contains('msvc'))
////                    files.push("app_icon.rc");

////                if (qbs.targetOS.contains('macos') &&
////                    qbs.toolchain && qbs.toolchain.contains('gcc'))
////                    files.push("resources/app_icon.rc");

//                return files;
//            }
//        }

    cpp.dynamicLibraries: [
        "pthread",
        "KF5KIOCore",
        "KF5ConfigCore",
        "KF5CoreAddons",
        "KF5ConfigWidgets",
        "KF5TextWidgets",
        "KF5WidgetsAddons",
        "KF5Parts",
        "KF5IconThemes",
        "KF5XmlGui",
        "KF5Crash",
        "KF5I18n",
    ]

    Group {
        name: "icons"
        files: [
            "xpm/*.xpm",
        ]
    }

    files: [
        "*.cpp",
        "*.h",
        "*.ui",
    ]
//    excludeFiles: [
//        "kdiff3_part.cpp",
//        "kdiff3_part.h",
//    ]

//    property var test: {
//        console.info("=== Qt.core.version ===");
//        console.info(Qt.core.version);
//        console.info("=== VERSION_PROJECT ===");
//        console.info(project.projectVersion[0]);
//    }

}
