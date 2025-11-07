package com.roy.jni_demo

/**
 *    desc   :
 *    date   : 2025/11/6 17:25
 *    author : Roy
 *    version: 1.0
 */
// JniManager.kt
// JniManager.kt
class JniManager {

    companion object {
        init {
            System.loadLibrary("jni_demo")
        }

        @Volatile
        private var instance: JniManager? = null

        fun getInstance(): JniManager {
            return instance ?: synchronized(this) {
                instance ?: JniManager().also {
                    instance = it
                    it.initJNI() // 初始化JNI
                }
            }
        }
    }

     external fun initJNI()
    external fun cleanupJNI()

    external fun processDataWithCallback(
        input: String,
        userId: String,
        callback: CallbackInterface
    )

    // 在适当的时候调用清理（如Activity的onDestroy）
    fun cleanup() {
        cleanupJNI()
    }

    fun demoUsage() {
        val callback = object : CallbackInterface {
            override fun onSuccess(result: ResultData) {
                println("✅ 成功: ID=${result.id}, 消息='${result.message}', 成功=${result.success}")
            }

            override fun onProgress(progress: Int, currentData: UserInfo) {
                println("📊 进度: $progress%, 用户='${currentData.userName}' (${currentData.userId})")
            }

            override fun onError(errorCode: Int, errorMessage: String, errorData: ResultData?) {
                println("❌ 错误: $errorCode - $errorMessage")
                errorData?.let {
                    println("   错误详情: ${it.message}")
                }
            }
        }

        processDataWithCallback("开始数据处理", "user_${System.currentTimeMillis()}", callback)
    }
}