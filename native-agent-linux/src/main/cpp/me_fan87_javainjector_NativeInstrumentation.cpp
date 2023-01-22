#include <cstring>
#include "me_fan87_javainjector_NativeInstrumentation.h"
#include "jvmti.h"



jboolean
checkForAndClearThrowable(  JNIEnv *    jnienv) {
    jboolean result = (*jnienv).ExceptionCheck();
    if ( result ) {
        (*jnienv).ExceptionClear();
    }
    return result;
}
void* allocate(jvmtiEnv *jvmtienv, size_t bytecount) {
    void *          resultBuffer    = NULL;
    jvmtiError      error           = JVMTI_ERROR_NONE;

    error = (*jvmtienv).Allocate(bytecount,
                                 (unsigned char**) &resultBuffer);
    /* may be called from any phase */
    if ( error != JVMTI_ERROR_NONE ) {
        resultBuffer = NULL;
    }
    return resultBuffer;
}
void deallocate(jvmtiEnv * jvmtienv, void * buffer) {
    jvmtiError  error = JVMTI_ERROR_NONE;

    error = (*jvmtienv).Deallocate((unsigned char*)buffer);
}
jboolean checkForThrowable(JNIEnv *jnienv) {
    return (*jnienv).ExceptionCheck();
}
void createAndThrowThrowableFromJVMTIErrorCode(JNIEnv * jnienv, jvmtiError errorCode) {
    if (errorCode == 0) {
        return;
    }
    const char * throwableClassName = NULL;
    const char * message            = NULL;
    jstring messageString           = NULL;

    switch ( errorCode ) {
        case JVMTI_ERROR_NULL_POINTER:
            throwableClassName = "java/lang/NullPointerException";
            break;

        case JVMTI_ERROR_ILLEGAL_ARGUMENT:
            throwableClassName = "java/lang/IllegalArgumentException";
            break;

        case JVMTI_ERROR_OUT_OF_MEMORY:
            throwableClassName = "java/lang/OutOfMemoryError";
            break;

        case JVMTI_ERROR_CIRCULAR_CLASS_DEFINITION:
            throwableClassName = "java/lang/ClassCircularityError";
            break;

        case JVMTI_ERROR_FAILS_VERIFICATION:
            throwableClassName = "java/lang/VerifyError";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_ADDED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to add a method";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_SCHEMA_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change the schema (add/remove fields)";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_HIERARCHY_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change superclass or interfaces";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_DELETED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to delete a method";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_CLASS_MODIFIERS_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change the class modifiers";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_MODIFIERS_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change method modifiers";
            break;

        case JVMTI_ERROR_UNSUPPORTED_VERSION:
            throwableClassName = "java/lang/UnsupportedClassVersionError";
            break;

        case JVMTI_ERROR_NAMES_DONT_MATCH:
            throwableClassName = "java/lang/NoClassDefFoundError";
            message = "class names don't match";
            break;

        case JVMTI_ERROR_INVALID_CLASS_FORMAT:
            throwableClassName = "java/lang/ClassFormatError";
            break;

        case JVMTI_ERROR_UNMODIFIABLE_CLASS:
            throwableClassName = "java/lang/instrument/UnmodifiableClassException";
            break;

        case JVMTI_ERROR_INVALID_CLASS:
            throwableClassName = "java/lang/InternalError";
            message = "class redefinition failed: invalid class";
            break;

        case JVMTI_ERROR_CLASS_LOADER_UNSUPPORTED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "unsupported operation";
            break;

        case JVMTI_ERROR_INTERNAL:
        default:
            throwableClassName = "java/lang/InternalError";
            break;
    }

    jnienv->ThrowNew(jnienv->FindClass(throwableClassName), message);

}
void jplis_assert(bool condition) {
    if (!condition) {
        fprintf(stderr, "Failed condition somewhere\n");
    }
}
jstring error(JNIEnv *jnienv, const char* message) {
    fprintf(stderr, "%s", message);
    fflush(stdout);
    return jnienv->NewStringUTF(message);
}
jvmtiEnv* jvmti1 = nullptr;
jvmtiEnv* GetJvmTiEnv(JNIEnv *jnienv) {
    if (jvmti1 == nullptr) {
        JavaVM *vm;
        JNIEnv *jni;
        jvmtiEnv *jvmti0;
        jsize size;
        if (JNI_GetCreatedJavaVMs(&vm, 1, &size) != JNI_OK || size != 1) {
            error(jnienv, "Failed to GetCreatedJavaVMs");
        }
        jint result = vm->GetEnv((void**) &jni, JNI_VERSION_1_6);
        if (result == JNI_EDETACHED) {
            result = vm->AttachCurrentThread((void**) &jni, nullptr);
        }
        result = vm->GetEnv((void**) &jvmti0, JVMTI_VERSION_1_0);
        if (result != JNI_OK) {
            error(jnienv, "Failed to get JvmTi");
        }
        jvmtiCapabilities capabilities = {};
        capabilities.can_retransform_classes = 1;
        capabilities.can_retransform_any_class = 1;
        capabilities.can_redefine_classes = 1;
        capabilities.can_redefine_any_class = 1;
        capabilities.can_generate_all_class_hook_events = 1;
        jvmti0->AddCapabilities(&capabilities);
        jvmti1 = jvmti0;
    }

    return jvmti1;
}


jboolean Java_me_fan87_javainjector_NativeInstrumentation_isModifiableClass0(JNIEnv *env, jclass clazz, jclass) {
    jvmtiEnv *          jvmtienv = GetJvmTiEnv(env);
    jvmtiError          jvmtierror;
    jboolean            is_modifiable = JNI_FALSE;

    jvmtierror = (*jvmtienv).IsModifiableClass( clazz,
                                                 &is_modifiable);
    createAndThrowThrowableFromJVMTIErrorCode(env, jvmtierror);
    return is_modifiable;
}

void Java_me_fan87_javainjector_NativeInstrumentation_setHasRetransformableTransformers(JNIEnv *env, jclass clazz, jboolean has) {
    (*GetJvmTiEnv(env)).SetEventNotificationMode(
            has? JVMTI_ENABLE : JVMTI_DISABLE,
            JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,
            NULL /* all threads */);
}

void Java_me_fan87_javainjector_NativeInstrumentation_retransformClasses0(JNIEnv *jnienv, jclass clazz, jobjectArray classes) {
    jvmtiEnv *  retransformerEnv     = GetJvmTiEnv(jnienv);
    jboolean    errorOccurred        = JNI_FALSE;
    jvmtiError  errorCode            = JVMTI_ERROR_NONE;
    jsize       numClasses           = 0;
    jclass *    classArray           = NULL;


    /* This was supposed to be checked by caller too */
    if (!errorOccurred && classes == NULL) {
        jplis_assert(classes != NULL);
        errorOccurred = JNI_TRUE;
        errorCode = JVMTI_ERROR_NULL_POINTER;
    }

    if (!errorOccurred) {
        numClasses = (*jnienv).GetArrayLength(classes);
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);

        if (!errorOccurred && numClasses == 0) {
            jplis_assert(numClasses != 0);
            errorOccurred = JNI_TRUE;
            errorCode = JVMTI_ERROR_NULL_POINTER;
        }
    }

    if (!errorOccurred) {
        classArray = (jclass *) allocate(retransformerEnv,
                                         numClasses * sizeof(jclass));
        errorOccurred = (classArray == NULL);
        jplis_assert(!errorOccurred);
        if (errorOccurred) {
            errorCode = JVMTI_ERROR_OUT_OF_MEMORY;
        }
    }

    if (!errorOccurred) {
        jint index;
        for (index = 0; index < numClasses; index++) {
            classArray[index] = static_cast<jclass>((*jnienv).GetObjectArrayElement(classes, index));
            errorOccurred = checkForThrowable(jnienv);
            jplis_assert(!errorOccurred);
            if (errorOccurred) {
                break;
            }

            if (classArray[index] == NULL) {
                jplis_assert(classArray[index] != NULL);
                errorOccurred = JNI_TRUE;
                errorCode = JVMTI_ERROR_NULL_POINTER;
                break;
            }
        }
    }

    if (!errorOccurred) {
        errorCode = (*retransformerEnv).RetransformClasses(
                                                            numClasses, classArray);
        errorOccurred = (errorCode != JVMTI_ERROR_NONE);
    }

    /* Give back the buffer if we allocated it.  Throw any exceptions after.
     */
    if (classArray != NULL) {
        deallocate(retransformerEnv, (void*)classArray);
    }

    if (errorCode != JVMTI_ERROR_NONE) {
        printf("Asdf %d\n", errorCode);
        createAndThrowThrowableFromJVMTIErrorCode(jnienv, errorCode);
    }

//    mapThrownThrowableIfNecessary(jnienv, redefineClassMapper);
}

void Java_me_fan87_javainjector_NativeInstrumentation_redefineClasses0(JNIEnv *jnienv, jclass clazz, jobjectArray classDefinitions) {
    jvmtiEnv*   jvmtienv                        = GetJvmTiEnv(jnienv);
    jboolean    errorOccurred                   = JNI_FALSE;
    jclass      classDefClass                   = NULL;
    jmethodID   getDefinitionClassMethodID      = NULL;
    jmethodID   getDefinitionClassFileMethodID  = NULL;
    jvmtiClassDefinition* classDefs             = NULL;
    jbyteArray* targetFiles                     = NULL;
    jsize       numDefs                         = 0;

    jplis_assert(classDefinitions != NULL);

    numDefs = jnienv->GetArrayLength(classDefinitions);
    errorOccurred = checkForThrowable(jnienv);
    jplis_assert(!errorOccurred);

    if (!errorOccurred) {
        jplis_assert(numDefs > 0);
        /* get method IDs for methods to call on class definitions */
        classDefClass = jnienv->FindClass("java/lang/instrument/ClassDefinition");
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);
    }

    if (!errorOccurred) {
        getDefinitionClassMethodID = jnienv->GetMethodID(
                                                                classDefClass,
                                                                "getDefinitionClass",
                                                                "()Ljava/lang/Class;");
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);
    }

    if (!errorOccurred) {
        getDefinitionClassFileMethodID = jnienv->GetMethodID(
                                                                    classDefClass,
                                                                    "getDefinitionClassFile",
                                                                    "()[B");
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);
    }

    if (!errorOccurred) {
        classDefs = (jvmtiClassDefinition *) allocate(
                jvmtienv,
                numDefs * sizeof(jvmtiClassDefinition));
        errorOccurred = (classDefs == NULL);
        jplis_assert(!errorOccurred);
        if ( errorOccurred ) {
            createAndThrowThrowableFromJVMTIErrorCode(jnienv, JVMTI_ERROR_OUT_OF_MEMORY);
        }

        else {
            /*
             * We have to save the targetFile values that we compute so
             * that we can release the class_bytes arrays that are
             * returned by GetByteArrayElements(). In case of a JNI
             * error, we can't (easily) recompute the targetFile values
             * and we still want to free any memory we allocated.
             */
            targetFiles = (jbyteArray *) allocate(jvmtienv,
                                                  numDefs * sizeof(jbyteArray));
            errorOccurred = (targetFiles == NULL);
            jplis_assert(!errorOccurred);
            if ( errorOccurred ) {
                deallocate(jvmtienv, (void*)classDefs);
                createAndThrowThrowableFromJVMTIErrorCode(jnienv,
                                                          JVMTI_ERROR_OUT_OF_MEMORY);
            }
            else {
                jint i, j;

                // clear classDefs so we can correctly free memory during errors
                memset(classDefs, 0, numDefs * sizeof(jvmtiClassDefinition));

                for (i = 0; i < numDefs; i++) {
                    jclass      classDef    = NULL;

                    classDef = static_cast<jclass>(jnienv->GetObjectArrayElement(classDefinitions, i));
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    classDefs[i].klass = static_cast<jclass>(jnienv->CallObjectMethod(classDef,
                                                                                      getDefinitionClassMethodID));
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    targetFiles[i] = static_cast<jbyteArray>(jnienv->CallObjectMethod(classDef,
                                                                                      getDefinitionClassFileMethodID));
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    classDefs[i].class_byte_count = jnienv->GetArrayLength(targetFiles[i]);
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    /*
                     * Allocate class_bytes last so we don't have to free
                     * memory on a partial row error.
                     */
                    classDefs[i].class_bytes = (unsigned char*)jnienv->GetByteArrayElements(targetFiles[i], NULL);
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }
                }

                if (!errorOccurred) {
                    jvmtiError  errorCode = JVMTI_ERROR_NONE;
                    errorCode = jvmtienv->RedefineClasses(numDefs, classDefs);
                    if (errorCode == JVMTI_ERROR_WRONG_PHASE) {
                        /* insulate caller from the wrong phase error */
                        errorCode = JVMTI_ERROR_NONE;
                    } else {
                        errorOccurred = (errorCode != JVMTI_ERROR_NONE);
                        if ( errorOccurred ) {
                            createAndThrowThrowableFromJVMTIErrorCode(jnienv, errorCode);
                        }
                    }
                }

                /*
                 * Cleanup memory that we allocated above. If we had a
                 * JNI error, a JVM/TI error or no errors, index 'i'
                 * tracks how far we got in processing the classDefs
                 * array. Note:  ReleaseByteArrayElements() is safe to
                 * call with a JNI exception pending.
                 */
                for (j = 0; j < i; j++) {
                    if ((jbyte *)classDefs[j].class_bytes != NULL) {
                        jnienv->ReleaseByteArrayElements(
                                                            targetFiles[j], (jbyte *)classDefs[j].class_bytes,
                                                            0 /* copy back and free */);
                        /*
                         * Only check for error if we didn't already have one
                         * so we don't overwrite errorOccurred.
                         */
                        if (!errorOccurred) {
                            errorOccurred = checkForThrowable(jnienv);
                            jplis_assert(!errorOccurred);
                        }
                    }
                }
                deallocate(jvmtienv, (void*)targetFiles);
                deallocate(jvmtienv, (void*)classDefs);
            }
        }
    }

//    mapThrownThrowableIfNecessary(jnienv, redefineClassMapper);

}

jclass nativeInstrumentation;
jmethodID transform;

void ClassFileLoad(jvmtiEnv *jvmti_env,
                   JNIEnv* jni_env,
                   jclass class_being_redefined,
                   jobject loader,
                   const char* name,
                   jobject protection_domain,
                   jint class_data_len,
                   const unsigned char* class_data,
                   jint* new_class_data_len,
                   unsigned char** new_class_data) {
    if (nativeInstrumentation == nullptr) {
        printf("Class not found! Loading %s\n", name);
        *new_class_data_len = class_data_len;
        void * bytes = allocate(jvmti_env, class_data_len + 1);
        memcpy(bytes, class_data, class_data_len);
        *new_class_data = (unsigned char*) bytes;
        return;
    }
    if (transform == nullptr) {
        error(jni_env, "Method not found!!!!\n");
        return;
    }
    jbyteArray jbyteArray1 = jni_env->NewByteArray(class_data_len);
    jni_env->SetByteArrayRegion(jbyteArray1, 0, class_data_len, (signed char*)class_data);
    jbyteArray out = (jbyteArray) jni_env->CallStaticObjectMethod(nativeInstrumentation, transform, loader, jni_env->NewStringUTF(name), class_being_redefined, protection_domain, jbyteArray1);
    if (out == nullptr) {
        return;
    }
    *new_class_data_len = jni_env->GetArrayLength(out);
    *new_class_data = (unsigned char*) jni_env->GetByteArrayElements(out, nullptr);
}
jvmtiEventCallbacks callbacks{};

void Java_me_fan87_javainjector_NativeInstrumentation_init(JNIEnv *env, jclass) {
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    callbacks.ClassFileLoadHook = ClassFileLoad;
    jvmtienv->SetEventCallbacks(&callbacks, sizeof(callbacks));
    jvmtienv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);

    nativeInstrumentation = env->FindClass("me/fan87/javainjector/NativeInstrumentation");
    transform = env->GetStaticMethodID(nativeInstrumentation, "transform", "(Ljava/lang/ClassLoader;Ljava/lang/String;Ljava/lang/Class;Ljava/security/ProtectionDomain;[B)[B");
}

jobjectArray Java_me_fan87_javainjector_NativeInstrumentation_getAllLoadedClasses0(JNIEnv *env, jclass clazz) {
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    jint size;
    jclass *classes;
    jvmtienv->GetLoadedClasses(&size, &classes);
    jobjectArray array = env->NewObjectArray(size, (jclass) env->NewGlobalRef(env->FindClass("java/lang/Object")), nullptr);
    for (int i = 0; i < size; ++i) {
        env->SetObjectArrayElement(array, i, classes[i]);
    }
    
    return array;
}

jobjectArray Java_me_fan87_javainjector_NativeInstrumentation_getInitiatedClasses0(JNIEnv *env, jclass clazz, jobject classLoader) {
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    jint size;
    jclass *classes;
    jvmtienv->GetClassLoaderClasses(classLoader, &size, &classes);
    jobjectArray array = env->NewObjectArray(size, env->FindClass("java/lang/Class"), nullptr);
    for (int i = 0; i < size; ++i) {
        env->SetObjectArrayElement(array, size, classes[i]);
    }
    return array;
}

jlong Java_me_fan87_javainjector_NativeInstrumentation_getObjectSize0(JNIEnv *env, jclass clazz, jobject obj) {
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    jlong objectSize = -1;
    jvmtienv->GetObjectSize(obj, &objectSize);
    return objectSize;
}

void Java_me_fan87_javainjector_NativeInstrumentation_appendToClassLoaderSearch0(JNIEnv *jnienv, jclass clazz, jstring jarFile, jboolean isBootLoader) {
    if (isBootLoader) {
        createAndThrowThrowableFromJVMTIErrorCode(jnienv, GetJvmTiEnv(jnienv)->AddToBootstrapClassLoaderSearch(jnienv->GetStringUTFChars(jarFile, nullptr)));
    } else {
        createAndThrowThrowableFromJVMTIErrorCode(jnienv, GetJvmTiEnv(jnienv)->AddToSystemClassLoaderSearch(jnienv->GetStringUTFChars(jarFile, nullptr)));
    }
}

void
Java_me_fan87_javainjector_NativeInstrumentation_setNativeMethodPrefixes(JNIEnv *jnienv, jclass clazz, jobjectArray prefixArray, jboolean isRetransformable) {
    jvmtiEnv*   jvmtienv                        = GetJvmTiEnv(jnienv);
    jvmtiError  err                             = JVMTI_ERROR_NONE;
    jsize       arraySize;
    jboolean    errorOccurred                   = JNI_FALSE;

    arraySize = (*jnienv).GetArrayLength(prefixArray);
    errorOccurred = checkForThrowable(jnienv);

    if (!errorOccurred) {
        /* allocate the native to hold the native prefixes */
        const char** prefixes = (const char**) allocate(jvmtienv,
                                                        arraySize * sizeof(char*));
        /* since JNI ReleaseStringUTFChars needs the jstring from which the native
         * string was allocated, we store them in a parallel array */
        jstring* originForRelease = (jstring*) allocate(jvmtienv,
                                                        arraySize * sizeof(jstring));
        errorOccurred = (prefixes == NULL || originForRelease == NULL);
        jplis_assert(!errorOccurred);
        if ( errorOccurred ) {
//            createAndThrowThrowableFromJVMTIErrorCode(jnienv, JVMTI_ERROR_OUT_OF_MEMORY);
        }
        else {
            jint inx = 0;
            jint i;
            for (i = 0; i < arraySize; i++) {
                jstring      prefixStr  = NULL;
                const char*  prefix;
                jsize        prefixLen;
                jboolean     isCopy;

                prefixStr = (jstring) (jnienv->GetObjectArrayElement(prefixArray, i));
                errorOccurred = checkForThrowable(jnienv);
//                jplis_assert(!errorOccurred);
                if (errorOccurred) {
                    break;
                }
                if (prefixStr == NULL) {
                    continue;
                }

                prefixLen = jnienv->GetStringUTFLength(prefixStr);
                errorOccurred = checkForThrowable(jnienv);
//                jplis_assert(!errorOccurred);
                if (errorOccurred) {
                    break;
                }

                if (prefixLen > 0) {
                    prefix = jnienv->GetStringUTFChars(prefixStr, &isCopy);
                    errorOccurred = checkForThrowable(jnienv);
//                    jplis_assert(!errorOccurred);
                    if (!errorOccurred && prefix != NULL) {
                        prefixes[inx] = prefix;
                        originForRelease[inx] = prefixStr;
                        ++inx;
                    }
                }
            }

            err = (*jvmtienv).SetNativeMethodPrefixes(inx, (char**)prefixes);
            /* can be called from any phase */
//            jplis_assert(err == JVMTI_ERROR_NONE);

            for (i = 0; i < inx; i++) {
                (*jnienv).ReleaseStringUTFChars(originForRelease[i], prefixes[i]);
            }
        }
        deallocate(jvmtienv, (void*)prefixes);
        deallocate(jvmtienv, (void*)originForRelease);
    }
}


