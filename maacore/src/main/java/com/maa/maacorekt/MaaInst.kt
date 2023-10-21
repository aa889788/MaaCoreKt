package com.maa.maacorekt

class MaaInst(val maacore: MaaCore, val handle: Long) {

    fun setOptions(key: Int, value: String): Boolean {
        return maacore.MaaSetInstanceOption(handle, key, value)
    }

    fun connect(adbPath: String, address: String, config: String): Boolean {
        return maacore.MaaConnect(handle, adbPath, address, config)
    }

    fun appendTask(type: String, params: String): Int {
        return maacore.MaaAppendTask(handle, type, params)
    }

    fun setTaskParams(taskId: Int, params: String): Boolean {
        return maacore.MaaSetTaskParams(handle, taskId, params)
    }

    fun start(): Boolean {
        return maacore.MaaStart(handle)
    }

    fun stop(): Boolean {
        return maacore.MaaStop(handle)
    }

    fun isRunning(): Boolean {
        return maacore.MaaRunning(handle)
    }

    fun isConnected(): Boolean {
        return maacore.MaaConnected(handle)
    }

    fun asyncConnect(adbPath: String, address: String, config: String, block: Boolean): Int {
        return maacore.MaaAsyncConnect(handle, adbPath, address, config, block)
    }

    fun asyncClick(x: Int, y: Int, block: Boolean): Int {
        return maacore.MaaAsyncClick(handle, x, y, block)
    }

    fun asyncScreencap(block: Boolean): Int {
        return maacore.MaaAsyncScreencap(handle, block)
    }
}