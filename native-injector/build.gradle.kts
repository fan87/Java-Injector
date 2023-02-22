plugins {
    id("cpp-library")
}

group = "com.github.fan87.Java-Injector"
version = "1.0-SNAPSHOT"
fun isWindows(): Boolean = "Windows" in System.getProperty("os.name")



tasks {



}



tasks.withType(CppCompile::class.java).configureEach {

    val include = File(File(System.getProperty("java.home")).parentFile, "include")
    includes.from(include, *include.listFiles()!!)
    if (isWindows()) {
        compilerArgs.add("/MD")
    }

}
tasks.withType(LinkSharedLibrary::class.java).configureEach {
    val bin = File(File(System.getProperty("java.home")).parentFile, "lib")
    val libraries = ArrayList<File>()
    for (listFile in bin.listFiles()) {
        if (listFile.name.endsWith(".lib")) {
            libraries.add(listFile)
        }
    }
    val libsDir = File(projectDir, "libs")
    if (isWindows()) {
        libs.from(*libraries.toTypedArray(), File(libsDir, "injector-static.lib"))
    } else {
        libs.from(*libraries.toTypedArray(), File(libsDir, "libinjector.a"))
    }
}

library {
    linkage.set(arrayListOf(Linkage.SHARED))
    source.from("src/main/cpp")
    publicHeaders.from("src/main/public")
    privateHeaders.from("src/main/headers")
}

