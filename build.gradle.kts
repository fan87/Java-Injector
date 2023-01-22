import java.io.File

plugins {
    id("java")
}

group = "me.fan87"
version = "1.0-SNAPSHOT"

fun isWindows(): Boolean = "Windows" in System.getProperty("os.name")

repositories {
    mavenCentral()
}

dependencies {
    implementation(project(":common"))
}


tasks {

    register<Copy>("copyInjectorLib") {
        group = "native"
        if (isWindows()) {
            TODO("Windows is not yet supported")
        } else {
            dependsOn(":native-injector-linux:build")
            File(project(":native-injector-linux").buildDir, "libs/").listFiles()?.let { from(it.first()) }
        }
        into(sourceSets.main.get().resources.sourceDirectories.first().absolutePath)
    }
    register<Copy>("copyInjectionTarget") {
        group = "native"
        if (isWindows()) {
            TODO("Windows is not yet supported")
        } else {
            dependsOn(":injection-target-linux:build")
            File(project(":injection-target-linux").buildDir, "libs/").listFiles()?.let { from(it.first()) }
        }
        into(sourceSets.main.get().resources.sourceDirectories.first().absolutePath)

    }

    register<Copy>("copyNativeAgent") {
        group = "native"
        if (isWindows()) {
            TODO("Windows is not yet supported")
        } else {
            dependsOn(":native-agent-linux:build")
            File(project(":native-agent-linux").buildDir, "libs/").listFiles()?.let { from(it.first()) }
        }
        into(project("native-agent").sourceSets.main.get().resources.sourceDirectories.first().absolutePath)

    }
    register("copyAll") {
        group = "native"
        dependsOn("copyInjectorLib")
        dependsOn("copyInjectionTarget")
        dependsOn("copyNativeAgent")
    }

//    register<JavaExec>("testInjection") {
//        dependsOn(":test-inject-target:classes")
//        dependsOn(":test-program:build")
//
//        doFirst {
//            javaexec {
//                classpath = files(*project(":test-inject-target").configurations.compileClasspath.map { it.files }.get().toTypedArray())
//                mainClass.set("Main")
//            }
//        }
//
//        classpath = files(project(":test-program").tasks.jar.get().archiveFile.get().asFile)
//        mainClass.set("InjectorMain")
//        args()
//    }

}