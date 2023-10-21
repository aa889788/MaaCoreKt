package com.maa.maacorekt

object MaaCoreManager {

    private val maaCore: MaaCore = MaaCore()

    var resourceLoaded = false
        private set

    fun SetUserDir(userDir: String): Boolean {
        return maaCore.MaaSetUserDir(userDir)
    }

    fun LoadResources(resDir: String): Boolean {
        resourceLoaded = maaCore.MaaLoadResources(resDir)
        return resourceLoaded
    }

    fun SetStaticOption(key: Int, value: String): Boolean {
        return maaCore.MaaSetStaticOption(key, value)
    }

    fun Create(): MaaInst {
        if (!resourceLoaded) {
            throw Exception("MaaCoreManager: resources not loaded")
        }
        return MaaInst(maaCore, maaCore.MaaCreate())
    }

    fun CreateWithCallBack(callBack: MaaCore.MaaCallback): MaaInst {
        if (!resourceLoaded) {
            throw Exception("MaaCoreManager: resources not loaded")
        }
        return MaaInst(maaCore, maaCore.MaaCreateWithCallback(callBack))
    }
}