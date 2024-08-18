#include "../Java.h"

void Lunar::GetLoadedClasses() {
    std::lock_guard<std::mutex> lock(mtx);
    jvmtiEnv* jvmti;
    if (vm->GetEnv(reinterpret_cast<void**>(&jvmti), JVMTI_VERSION_1_2) != JNI_OK) {
        std::cerr << "Failed to get JVMTI environment." << std::endl;
        return;
    }

    jclass lang = env->FindClass("java/lang/Class");
    jmethodID getName = env->GetMethodID(lang, "getName", "()Ljava/lang/String;");
    if (env->ExceptionOccurred()) {
        std::cerr << "Exception occurred while getting getName method ID." << std::endl;
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    jclass* classesPtr;
    jint amount;

    jvmti->GetLoadedClasses(&amount, &classesPtr);
    std::cout << "Number of loaded classes: " << amount << std::endl;

    for (int i = 0; i < amount; i++) {
        jstring name = (jstring)env->CallObjectMethod(classesPtr[i], getName);
        if (env->ExceptionOccurred()) {
            std::cerr << "Exception occurred while calling getName method on class " << i << "." << std::endl;
            env->ExceptionDescribe();
            env->ExceptionClear();
            continue;
        }

        const char* className = env->GetStringUTFChars(name, nullptr);
        if (env->ExceptionOccurred()) {
            std::cerr << "Exception occurred while getting string UTF chars for class " << i << "." << std::endl;
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(name);
            continue;
        }

        classes.emplace(std::make_pair(std::string(className), classesPtr[i]));

        env->ReleaseStringUTFChars(name, className);
        env->DeleteLocalRef(name);
    }
}

jclass Lunar::GetClass(const std::string& className) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = classes.find(className);
    if (it != classes.end()) {
        return it->second;
    }
    else {
        std::cerr << "Class " << className << " not found in cache." << std::endl;
        return nullptr;
    }
}

std::unique_ptr<Lunar> lc = std::make_unique<Lunar>();


