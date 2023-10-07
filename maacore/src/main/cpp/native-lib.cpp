#include <jni.h>
#include <string>

#include "AsstCaller.h"

struct MaaHandle {
    AsstHandle handle;
    jobject callbackObj;
    jmethodID callbackMethodId;
};

JavaVM *gJavaVM = nullptr;

static JNIEnv* GetEnv(bool *needDetach) {
    JNIEnv *env = nullptr;
    *needDetach = false;
    if (gJavaVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_EDETACHED) {
        if (gJavaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            return nullptr;
        }
        *needDetach = true;
    }
    return env;
}

void Maa_callback(AsstMsgId msg, const char *details_json, void *custom_arg) {
    if (custom_arg == nullptr) {
        return;
    }
    bool needDetach = false;
    JNIEnv *env = GetEnv(&needDetach);
    if (env == nullptr) {
        return;
    }
    auto *maa_handle = reinterpret_cast<MaaHandle *>(custom_arg);
    if (maa_handle->callbackObj == nullptr) {
        return;
    }
    jstring details_json_str = env->NewStringUTF(details_json);
    env->CallVoidMethod(maa_handle->callbackObj, maa_handle->callbackMethodId, msg, details_json_str);
    env->DeleteLocalRef(details_json_str);
    if (needDetach) {
        gJavaVM->DetachCurrentThread();
    }
}

jboolean MaaSetUserDir(JNIEnv *env, jobject thiz, jstring user_dir) {
    const char *user_dir_cstr = env->GetStringUTFChars(user_dir, nullptr);
    bool ret = AsstSetUserDir(user_dir_cstr);
    env->ReleaseStringUTFChars(user_dir, user_dir_cstr);
    return ret;
}

jboolean MaaLoadResources(JNIEnv *env, jobject thiz, jstring res_dir) {
    const char *user_dir_cstr = env->GetStringUTFChars(res_dir, nullptr);
    bool ret = AsstLoadResource(user_dir_cstr);
    env->ReleaseStringUTFChars(res_dir, user_dir_cstr);
    return ret;
}

jboolean MaaSetStaticOption(JNIEnv *env, jobject thiz, jint key, jstring value) {
    const char *value_cstr = env->GetStringUTFChars(value, nullptr);
    bool ret = AsstSetStaticOption(static_cast<AsstStaticOptionKey>(key), value_cstr);
    env->ReleaseStringUTFChars(value, value_cstr);
    return ret;
}

jlong MaaCreate(JNIEnv *env, jobject thiz) {
    auto *maa_handle = new MaaHandle();
    maa_handle->handle = AsstCreate();
    return reinterpret_cast<jlong>(maa_handle);
}

jlong MaaCreateEx(JNIEnv *env, jobject thiz, jobject callback) {
    auto *maa_handle = new MaaHandle();
    maa_handle->callbackObj = env->NewGlobalRef(callback);
    jclass callbackClass = env->GetObjectClass(callback);
    maa_handle->callbackMethodId = env->GetMethodID(callbackClass, "callback", "(ILjava/lang/String;)V");
    maa_handle->handle = AsstCreateEx(Maa_callback, maa_handle);
    return reinterpret_cast<jlong>(maa_handle);
}

void MaaDestroy(JNIEnv *env, jobject thiz, jlong handle) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return;
    }
    AsstDestroy(maa_handle->handle);
    if (maa_handle->callbackObj != nullptr) {
        env->DeleteGlobalRef(maa_handle->callbackObj);
    }
    delete maa_handle;
}

jboolean MaaSetInstanceOption(JNIEnv *env, jobject thiz, jlong handle, jint key, jstring value) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    const char *value_cstr = env->GetStringUTFChars(value, nullptr);
    bool ret = AsstSetInstanceOption(maa_handle->handle, static_cast<AsstInstanceOptionKey>(key), value_cstr);
    env->ReleaseStringUTFChars(value, value_cstr);
    return ret;
}

jboolean MaaConnect(JNIEnv *env, jobject thiz, jlong handle, jstring adb_path, jstring address, jstring config) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    const char *adb_path_cstr = env->GetStringUTFChars(adb_path, nullptr);
    const char *address_cstr = env->GetStringUTFChars(address, nullptr);
    const char *config_cstr = env->GetStringUTFChars(config, nullptr);
    bool ret = AsstConnect(maa_handle->handle, adb_path_cstr, address_cstr, config_cstr);
    env->ReleaseStringUTFChars(adb_path, adb_path_cstr);
    env->ReleaseStringUTFChars(address, address_cstr);
    env->ReleaseStringUTFChars(config, config_cstr);
    return ret;
}

jint MaaAppendTask(JNIEnv *env, jobject thiz, jlong handle, jstring type, jstring params) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    const char *type_cstr = env->GetStringUTFChars(type, nullptr);
    const char *params_cstr = env->GetStringUTFChars(params, nullptr);
    auto task_id = AsstAppendTask(maa_handle->handle, type_cstr, params_cstr);
    env->ReleaseStringUTFChars(type, type_cstr);
    env->ReleaseStringUTFChars(params, params_cstr);
    return task_id;
}

jboolean MaaSetTaskParams(JNIEnv *env, jobject thiz, jlong handle, jint task_id, jstring params) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    const char *params_cstr = env->GetStringUTFChars(params, nullptr);
    bool ret = AsstSetTaskParams(maa_handle->handle, task_id, params_cstr);
    env->ReleaseStringUTFChars(params, params_cstr);
    return ret;
}

jboolean MaaStart(JNIEnv *env, jobject thiz, jlong handle) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    return AsstStart(maa_handle->handle);
}

jboolean MaaStop(JNIEnv *env, jobject thiz, jlong handle) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    return AsstStop(maa_handle->handle);
}

jboolean MaaRunning(JNIEnv *env, jobject thiz, jlong handle) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    return AsstRunning(maa_handle->handle);
}

jboolean MaaConnected(JNIEnv *env, jobject thiz, jlong handle) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    return AsstConnected(maa_handle->handle);
}

jint MaaAsyncConnect(JNIEnv *env, jobject thiz, jlong handle, jstring adb_path, jstring address, jstring config,
                     jboolean block) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    const char *adb_path_cstr = env->GetStringUTFChars(adb_path, nullptr);
    const char *address_cstr = env->GetStringUTFChars(address, nullptr);
    const char *config_cstr = env->GetStringUTFChars(config, nullptr);
    auto async_call_id = AsstAsyncConnect(maa_handle->handle, adb_path_cstr, address_cstr, config_cstr, block);
    env->ReleaseStringUTFChars(adb_path, adb_path_cstr);
    env->ReleaseStringUTFChars(address, address_cstr);
    env->ReleaseStringUTFChars(config, config_cstr);
    return async_call_id;
}

jint MaaAsyncClick(JNIEnv *env, jobject thiz, jlong handle, jint x, jint y, jboolean block) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    return AsstAsyncClick(maa_handle->handle, x, y, block);
}

jint MaaAsyncScreencap(JNIEnv *env, jobject thiz, jlong handle, jboolean block) {
    auto *maa_handle = reinterpret_cast<MaaHandle *>(handle);
    if (maa_handle == nullptr) {
        return JNI_FALSE;
    }
    return AsstAsyncScreencap(maa_handle->handle, block);
}

static const char *classPathName = "com/maa/maacorekt/MaaCore";

static const JNINativeMethod g_methods[] = {
        {"MaaSetUserDir", "(Ljava/lang/String;)Z", (void *) MaaSetUserDir},
        {"MaaLoadResources", "(Ljava/lang/String;)Z", (void *) MaaLoadResources},
        {"MaaSetStaticOption", "(ILjava/lang/String;)Z", (void *) MaaSetStaticOption},
        {"MaaCreate", "()J", (void *) MaaCreate},
        {"MaaCreateWithCallBack", "(Lcom/maa/maacorekt/MaaCore$MaaCoreCallBack;)J", (void *) MaaCreateEx},
        {"MaaDestroy", "(J)V", (void *) MaaDestroy},
        {"MaaSetInstanceOption", "(JILjava/lang/String;)Z", (void *) MaaSetInstanceOption},
        {"MaaConnect", "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *) MaaConnect},
        {"MaaAppendTask", "(JLjava/lang/String;Ljava/lang/String;)I", (void *) MaaAppendTask},
        {"MaaSetTaskParams", "(JILjava/lang/String;)Z", (void *) MaaSetTaskParams},
        {"MaaStart", "(J)Z", (void *) MaaStart},
        {"MaaStop", "(J)Z", (void *) MaaStop},
        {"MaaRunning", "(J)Z", (void *) MaaRunning},
        {"MaaConnected", "(J)Z", (void *) MaaConnected},
        {"MaaAsyncConnect", "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)I", (void *) MaaAsyncConnect},
        {"MaaAsyncClick", "(JIIZ)I", (void *) MaaAsyncClick},
        {"MaaAsyncScreencap", "(JZ)I", (void *) MaaAsyncScreencap},

};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    gJavaVM = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clazz = env->FindClass(classPathName);
    if (clazz == nullptr) {
        return JNI_ERR;
    }
    if (env->RegisterNatives(clazz, g_methods, sizeof(g_methods) / sizeof(g_methods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
