//
// Created by fan87 on 1/23/23.
//

#include "type_convert.h"
#include "jni.h"
#include <cstring>
#include <cstdlib>
#include <string>

jclass Class_class;

jclass Boolean_class;
jclass boolean_type;
jmethodID Boolean_constructor_id;
jmethodID booleanValue_id;

jclass Byte_class;
jclass byte_type;
jmethodID Byte_constructor_id;
jmethodID byteValue_id;

jclass Character_class;
jclass char_type;
jmethodID Character_constructor_id;
jmethodID charValue_id;

jclass Short_class;
jclass short_type;
jmethodID Short_constructor_id;
jmethodID shortValue_id;

jclass Integer_class;
jclass int_type;
jmethodID Integer_constructor_id;
jmethodID intValue_id;

jclass Long_class;
jclass long_type;
jmethodID Long_constructor_id;
jmethodID longValue_id;

jclass Float_class;
jclass float_type;
jmethodID Float_constructor_id;
jmethodID floatValue_id;

jclass Double_class;
jclass double_type;
jmethodID Double_constructor_id;
jmethodID doubleValue_id;

char* parseSignature(char *signature) {
    bool readingObject = false;
    char *builder = (char*) malloc((strlen(signature) + 1) * sizeof(char));
    int outI = 0;

    char c;
    int i = 1;
    while ((c = signature[i++])) {
        if (c == ')') {
            break;
        }

        if (readingObject) {
            if (c == ';') {
                readingObject = false;
            }
            continue;
        }

        if (c == 'L') {
            readingObject = true;
        }

        if (c == '[') {
            if (signature[i] == 'L') {
                readingObject = true;
            } else {
                i++;
            }
        }

        builder[outI++] = c;
    }
    builder[outI++] = '\0';

    return (char*) realloc(builder, outI);
}

jvalue* objectsToJvalues(JNIEnv* env, char* signature, jobject* args, jsize length) {
    auto *values = (jvalue*) malloc(sizeof(jvalue) * length);
    char* targetParameterTypes = parseSignature(signature);

    for (int i = 0; i < length; i++) {
        jobject obj = args[i];
        jclass objClass = env->GetObjectClass(obj);
        char targetType = targetParameterTypes[i];

        if (targetType == 'Z' && env->IsSameObject(objClass, Boolean_class)) {
            values[i].z = env->CallBooleanMethod(obj, booleanValue_id);
            continue;
        }

        if (targetType == 'B' && env->IsSameObject(objClass, Byte_class)) {
            values[i].b = env->CallByteMethod(obj, byteValue_id);
            continue;
        }

        if (targetType == 'C' && env->IsSameObject(objClass, Character_class)) {
            values[i].c = env->CallCharMethod(obj, charValue_id);
            continue;
        }

        if (targetType == 'S' && env->IsSameObject(objClass, Short_class)) {
            values[i].s = env->CallShortMethod(obj, shortValue_id);
            continue;
        }

        if (targetType == 'I' && env->IsSameObject(objClass, Integer_class)) {
            values[i].i = env->CallIntMethod(obj, intValue_id);
            continue;
        }

        if (targetType == 'J' && env->IsSameObject(objClass, Long_class)) {
            values[i].j = env->CallLongMethod(obj, longValue_id);
            continue;
        }

        if (targetType == 'F' && env->IsSameObject(objClass, Float_class)) {
            values[i].f = env->CallFloatMethod(obj, floatValue_id);
            continue;
        }

        if (targetType == 'D' && env->IsSameObject(objClass, Double_class)) {
            values[i].d = env->CallDoubleMethod(obj, doubleValue_id);
            continue;
        }

        values[i].l = obj;
    }

    free(targetParameterTypes);
    return values;
}

char getReturnType(char* signature) {
    char* index = strchr(signature, ')');
    char type = index == nullptr ? *signature : *(index + 1);
    return type == '[' ? 'L' : type;
}

void init(JNIEnv *env) {
    
    Class_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Class"));

    // primitive types
    {
        // boolean
        Boolean_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Boolean"));
        boolean_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Boolean_class,
                                          env->GetStaticFieldID(Boolean_class, "TYPE", "Ljava/lang/Class;")));

        Boolean_constructor_id = env->GetMethodID(Boolean_class, "<init>", "(Z)V");
        booleanValue_id = env->GetMethodID(Boolean_class, "booleanValue", "()Z");

        // byte
        Byte_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Byte"));
        byte_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Byte_class,
                                          env->GetStaticFieldID(Byte_class, "TYPE", "Ljava/lang/Class;")));

        Byte_constructor_id = env->GetMethodID(Byte_class, "<init>", "(B)V");
        byteValue_id = env->GetMethodID(Byte_class, "byteValue", "()B");

        // char
        Character_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Character"));
        char_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Character_class,
                                          env->GetStaticFieldID(Character_class, "TYPE", "Ljava/lang/Class;")));

        Character_constructor_id = env->GetMethodID(Character_class, "<init>", "(C)V");
        charValue_id = env->GetMethodID(Character_class, "charValue", "()C");

        // short
        Short_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Short"));
        short_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Short_class,
                                          env->GetStaticFieldID(Short_class, "TYPE", "Ljava/lang/Class;")));

        Short_constructor_id = env->GetMethodID(Short_class, "<init>", "(S)V");
        shortValue_id = env->GetMethodID(Short_class, "shortValue", "()S");

        // int
        Integer_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Integer"));
        int_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Integer_class,
                                          env->GetStaticFieldID(Integer_class, "TYPE", "Ljava/lang/Class;")));

        Integer_constructor_id = env->GetMethodID(Integer_class, "<init>", "(I)V");
        intValue_id = env->GetMethodID(Integer_class, "intValue", "()I");

        // long
        Long_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Long"));
        long_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Long_class,
                                          env->GetStaticFieldID(Long_class, "TYPE", "Ljava/lang/Class;")));

        Long_constructor_id = env->GetMethodID(Long_class, "<init>", "(J)V");
        longValue_id = env->GetMethodID(Long_class, "longValue", "()J");

        // float
        Float_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Float"));
        float_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Float_class,
                                          env->GetStaticFieldID(Float_class, "TYPE", "Ljava/lang/Class;")));

        Float_constructor_id = env->GetMethodID(Float_class, "<init>", "(F)V");
        floatValue_id = env->GetMethodID(Float_class, "floatValue", "()F");

        // double
        Double_class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Double"));
        double_type = (jclass) env->NewGlobalRef(
                env->GetStaticObjectField(Double_class,
                                          env->GetStaticFieldID(Double_class, "TYPE", "Ljava/lang/Class;")));

        Double_constructor_id = env->GetMethodID(Double_class, "<init>", "(D)V");
        doubleValue_id = env->GetMethodID(Double_class, "doubleValue", "()D");
    }
}

jobject getField(JNIEnv *env, jfieldID id, jobject classInstance, char *signature) {
    char returnType = getReturnType(signature);

    jobject returnValue;
    switch (returnType) {
        case 'L':
            return env->GetObjectField(classInstance, id);

        case 'Z':
            jvalue constructorArgs[1];
            constructorArgs->z = env->GetBooleanField(classInstance, id);
            return env->NewObjectA(Boolean_class, Boolean_constructor_id, constructorArgs);
        case 'B':
            constructorArgs->b = env->GetByteField(classInstance, id);
            return env->NewObjectA(Byte_class, Byte_constructor_id, constructorArgs);
        case 'C':
            constructorArgs->c = env->GetCharField(classInstance, id);
            return env->NewObjectA(Character_class, Character_constructor_id, constructorArgs);
        case 'S':
            constructorArgs->s = env->GetShortField(classInstance, id);
            return env->NewObjectA(Short_class, Short_constructor_id, constructorArgs);
        case 'I':
            constructorArgs->i = env->GetIntField(classInstance, id);
            return env->NewObjectA(Integer_class, Integer_constructor_id, constructorArgs);
        case 'J':
            constructorArgs->j = env->GetLongField(classInstance, id);
            return env->NewObjectA(Long_class, Long_constructor_id, constructorArgs);
        case 'F':
            constructorArgs->f = env->GetFloatField(classInstance, id);
            return env->NewObjectA(Float_class, Float_constructor_id, constructorArgs);
        case 'D':
            constructorArgs->d = env->GetDoubleField(classInstance, id);
            return env->NewObjectA(Double_class, Double_constructor_id, constructorArgs);
    }

    return nullptr;
}

jobject getStaticField(JNIEnv *env, jfieldID id, jclass clazz, char *signature) {
    char returnType = getReturnType(signature);

    jobject returnValue;
    switch (returnType) {
        case 'L':
            return env->GetStaticObjectField(clazz, id);

        case 'Z':
            jvalue constructorArgs[1];
            constructorArgs->z = env->GetStaticBooleanField(clazz, id);
            return env->NewObjectA(Boolean_class, Boolean_constructor_id, constructorArgs);
        case 'B':
            constructorArgs->b = env->GetStaticByteField(clazz, id);
            return env->NewObjectA(Byte_class, Byte_constructor_id, constructorArgs);
        case 'C':
            constructorArgs->c = env->GetStaticCharField(clazz, id);
            return env->NewObjectA(Character_class, Character_constructor_id, constructorArgs);
        case 'S':
            constructorArgs->s = env->GetStaticShortField(clazz, id);
            return env->NewObjectA(Short_class, Short_constructor_id, constructorArgs);
        case 'I':
            constructorArgs->i = env->GetStaticIntField(clazz, id);
            return env->NewObjectA(Integer_class, Integer_constructor_id, constructorArgs);
        case 'J':
            constructorArgs->j = env->GetStaticLongField(clazz, id);
            return env->NewObjectA(Long_class, Long_constructor_id, constructorArgs);
        case 'F':
            constructorArgs->f = env->GetStaticFloatField(clazz, id);
            return env->NewObjectA(Float_class, Float_constructor_id, constructorArgs);
        case 'D':
            constructorArgs->d = env->GetStaticDoubleField(clazz, id);
            return env->NewObjectA(Double_class, Double_constructor_id, constructorArgs);
    }

    return nullptr;
}

void setField(JNIEnv *env, jfieldID id, jobject classInstance, jobject value, char* signature) {
    char returnType = getReturnType(signature);
    switch (returnType) {
        case 'L':
            env->SetObjectField(classInstance, id, value);
            break;
        case 'Z':
            env->SetBooleanField(classInstance, id, env->CallBooleanMethod(value, booleanValue_id));
            break;
        case 'B':
            env->SetByteField(classInstance, id, env->CallByteMethod(value, byteValue_id));
            break;
        case 'C':
            env->SetCharField(classInstance, id, env->CallCharMethod(value, charValue_id));
            break;
        case 'S':
            env->SetShortField(classInstance, id, env->CallShortMethod(value, shortValue_id));
            break;
        case 'I':
            env->SetIntField(classInstance, id, env->CallIntMethod(value, intValue_id));
            break;
        case 'J':
            env->SetLongField(classInstance, id, env->CallLongMethod(value, longValue_id));
            break;
        case 'F':
            env->SetFloatField(classInstance, id, env->CallFloatMethod(value, floatValue_id));
            break;
        case 'D':
            env->SetDoubleField(classInstance, id, env->CallDoubleMethod(value, doubleValue_id));
            break;
    }
}

void setStaticField(JNIEnv *env, jfieldID id, jclass clazz, jobject value, char* signature) {
    char returnType = getReturnType(signature);
    switch (returnType) {
        case 'L':
            env->SetStaticObjectField(clazz, id, value);
            break;
        case 'Z':
            env->SetStaticBooleanField(clazz, id, env->CallBooleanMethod(value, booleanValue_id));
            break;
        case 'B':
            env->SetStaticByteField(clazz, id, env->CallByteMethod(value, byteValue_id));
            break;
        case 'C':
            env->SetStaticCharField(clazz, id, env->CallCharMethod(value, charValue_id));
            break;
        case 'S':
            env->SetStaticShortField(clazz, id, env->CallShortMethod(value, shortValue_id));
            break;
        case 'I':
            env->SetStaticIntField(clazz, id, env->CallIntMethod(value, intValue_id));
            break;
        case 'J':
            env->SetStaticLongField(clazz, id, env->CallLongMethod(value, longValue_id));
            break;
        case 'F':
            env->SetStaticFloatField(clazz, id, env->CallFloatMethod(value, floatValue_id));
            break;
        case 'D':
            env->SetStaticDoubleField(clazz, id, env->CallDoubleMethod(value, doubleValue_id));
            break;
    }
}

jobject callMethod(JNIEnv *env, jmethodID id, jobject classInstance, jobject* objects, jsize length, char* signature) {
    char returnType = getReturnType(signature);

    jvalue *values = objectsToJvalues(env, signature, objects, length);

    jobject returnValue;
    switch (returnType) {
        case 'V':
            env->CallVoidMethodA(classInstance, id, values);
            returnValue = nullptr;
            break;
        case 'L':
            returnValue = env->CallObjectMethodA(classInstance, id, values);
            break;

        case 'Z':
            jvalue constructorArgs[1];
            constructorArgs->z = env->CallBooleanMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Boolean_class, Boolean_constructor_id, constructorArgs);
            break;
        case 'B':
            constructorArgs->b = env->CallByteMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Byte_class, Byte_constructor_id, constructorArgs);
            break;
        case 'C':
            constructorArgs->c = env->CallCharMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Character_class, Character_constructor_id, constructorArgs);
            break;
        case 'S':
            constructorArgs->s = env->CallShortMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Short_class, Short_constructor_id, constructorArgs);
            break;
        case 'I':
            constructorArgs->i = env->CallIntMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Integer_class, Integer_constructor_id, constructorArgs);
            break;
        case 'J':
            constructorArgs->j = env->CallLongMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Long_class, Long_constructor_id, constructorArgs);
            break;
        case 'F':
            constructorArgs->f = env->CallFloatMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Float_class, Float_constructor_id, constructorArgs);
            break;
        case 'D':
            constructorArgs->d = env->CallDoubleMethodA(classInstance, id, values);
            returnValue = env->NewObjectA(Double_class, Double_constructor_id, constructorArgs);
            break;
    }

    free(values);
    return returnValue;
}

jobject callStaticMethod(JNIEnv *env, jmethodID id, jclass clazz, jobject* objects, jsize length, char* signature) {
    char returnType = getReturnType(signature);

    jvalue *values = objectsToJvalues(env, signature, objects, length);

    jobject returnValue;
    switch (returnType) {
        case 'V':
            env->CallStaticVoidMethodA(clazz, id, values);
            returnValue = nullptr;
            break;

        case 'L':
            returnValue = env->CallStaticObjectMethodA(clazz, id, values);
            break;

        case 'Z':
            jvalue constructorArgs[1];
            constructorArgs->z = env->CallStaticBooleanMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Boolean_class, Boolean_constructor_id, constructorArgs);
            break;
        case 'B':
            constructorArgs->b = env->CallStaticByteMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Byte_class, Byte_constructor_id, constructorArgs);
            break;
        case 'C':
            constructorArgs->c = env->CallStaticCharMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Character_class, Character_constructor_id, constructorArgs);
            break;
        case 'S':
            constructorArgs->s = env->CallStaticShortMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Short_class, Short_constructor_id, constructorArgs);
            break;
        case 'I':
            constructorArgs->i = env->CallStaticIntMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Integer_class, Integer_constructor_id, constructorArgs);
            break;
        case 'J':
            constructorArgs->j = env->CallStaticLongMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Long_class, Long_constructor_id, constructorArgs);
            break;
        case 'F':
            constructorArgs->f = env->CallStaticFloatMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Float_class, Float_constructor_id, constructorArgs);
            break;
        case 'D':
            constructorArgs->d = env->CallStaticDoubleMethodA(clazz, id, values);
            returnValue = env->NewObjectA(Double_class, Double_constructor_id, constructorArgs);
            break;
    }

    free(values);
    return returnValue;
}