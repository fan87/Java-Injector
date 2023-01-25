import java.io.File

plugins {
    id("java")
    id("maven-publish")
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
    if (System.getProperty("me.fan87.buildnative") == "true") {

        register<Copy>("copyInjectorLib") {
            group = "native"
            dependsOn(":native-injector:build")
            File(project(":native-injector").buildDir, "lib/main/debug/").listFiles()?.let { from(it.first()) }
            into(sourceSets.main.get().resources.sourceDirectories.first().absolutePath)
        }
        register<Copy>("copyInjectionTarget") {
            group = "native"
            if (isWindows()) {
                dependsOn(":injection-target-windows:build")
                File(project(":injection-target-windows").buildDir, "lib/main/debug").listFiles()
                    ?.let { from(it.first()) }
            } else {
                dependsOn(":injection-target-linux:build")
                File(project(":injection-target-linux").buildDir, "libs/").listFiles()?.let { from(it.first()) }
            }
            into(sourceSets.main.get().resources.sourceDirectories.first().absolutePath)

        }

        register<Copy>("copyNativeAgent") {
            group = "native"
            dependsOn(":native-agent-native:build")
            File(project(":native-agent-native").buildDir, "lib/main/debug").listFiles()
                ?.filter { it.name.endsWith(".dll") || it.name.endsWith(".so") }?.let { from(it.first()) }
            into(project("native-agent").sourceSets.main.get().resources.sourceDirectories.first().absolutePath)

        }
        register("copyAll") {
            group = "native"
            dependsOn("copyInjectorLib")
            dependsOn("copyInjectionTarget")
            dependsOn("copyNativeAgent")
        }
    }

}




publishing {
    publications {
        register<MavenPublication>("mavenJava") {
            from(components.getByName("java"))
        }
    }
}