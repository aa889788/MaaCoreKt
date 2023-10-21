package com.maa.maacorekt

class MaaCore {

    interface MaaCallback {
        fun callback(msgId: Int, details: String)
    }

    /**
     * A native method that is implemented by the 'maacorekt' native library,
     * which is packaged with this application.
     */
    external fun MaaSetUserDir(userDir: String): Boolean
    external fun MaaLoadResources(resDir: String): Boolean
    external fun MaaSetStaticOption(key: Int, value: String): Boolean
    external fun MaaCreate(): Long
    external fun MaaCreateWithCallback(callBack: MaaCallback): Long
    external fun MaaSetCallback(handle: Long, callback: MaaCallback)
    external fun MaaDestroy(instance: Long)
    external fun MaaSetInstanceOption(instance: Long, key: Int, value: String): Boolean
    external fun MaaConnect(instance: Long, adbPath: String, address: String, config: String): Boolean
    external fun MaaAppendTask(instance: Long, type: String, params: String): Int
    external fun MaaSetTaskParams(instance: Long, taskId: Int, params: String): Boolean
    external fun MaaStart(instance: Long): Boolean
    external fun MaaStop(instance: Long): Boolean
    external fun MaaRunning(instance: Long): Boolean
    external fun MaaConnected(instance: Long): Boolean
    external fun MaaAsyncConnect(instance: Long, adbPath: String, address: String, config: String, block: Boolean): Int
    external fun MaaAsyncClick(instance: Long, x: Int, y: Int, block: Boolean): Int
    external fun MaaAsyncScreencap(instance: Long, block: Boolean): Int

    companion object {
        // Used to load the 'maacorekt' library on application startup.
        init {
            System.loadLibrary("maacorekt")
        }
    }
}