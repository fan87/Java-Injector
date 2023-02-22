plugins {
    id("java")
}

group = "com.github.fan87.Java-Injector"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
}
java {
}

tasks.jar {
    manifest {
        attributes["Main-Class"] = "Main"
    }
}