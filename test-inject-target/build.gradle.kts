plugins {
    id("java")
}

group = "me.fan87"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
}

tasks.jar {
    manifest {
        attributes["Main-Class"] = "Main"
    }
}