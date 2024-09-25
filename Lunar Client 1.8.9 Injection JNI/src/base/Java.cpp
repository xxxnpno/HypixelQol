#include "../java.h"

void Lunar::GetLoadedClasses() {
    std::lock_guard<std::mutex> lock(mtx);
    jvmtiEnv* jvmti;
    if (vm->GetEnv(reinterpret_cast<void**>(&jvmti), JVMTI_VERSION_1_2) != JNI_OK) {
        return;
    }

    jclass classClass = env->FindClass("java/lang/Class");
    jmethodID getNameMethod = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");

    jclass* classesPtr;
    jint classCount;

    jvmti->GetLoadedClasses(&classCount, &classesPtr);

    for (jint i = 0; i < classCount; ++i) {
        jstring classNameStr = (jstring)env->CallObjectMethod(classesPtr[i], getNameMethod);

        const char* classNameCStr = env->GetStringUTFChars(classNameStr, nullptr);

        classes.emplace(classNameCStr, classesPtr[i]);
    }
}

jclass Lunar::GetClass(const std::string& className) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = classes.find(className);
    if (it != classes.end()) {
        return it->second;
    }
    else {
        return nullptr;
    }
}

std::unique_ptr<Lunar> lc = std::make_unique<Lunar>();
