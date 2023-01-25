#include "injector.h"

#include "me_fan87_javainjector_JavaInjector.h"



jstring Java_me_fan87_javainjector_JavaInjector_injectNativeLib(JNIEnv *env, jclass clazz, jint pid, jstring fileName) {

    injector_t *injector;
    void *handle;

    if (injector_attach(&injector, pid) != 0) {
        return env->NewStringUTF(injector_error());
    }
    const char* path = env->GetStringUTFChars(fileName, (jboolean*) 0);
    if (injector_inject(injector, path, &handle) != 0) {
        return env->NewStringUTF(injector_error());
    }
    return NULL;
}
