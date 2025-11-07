package com.roy.jni_demo

/**
 *    desc   :
 *    date   : 2025/11/6 17:25
 *    author : Roy
 *    version: 1.0
 */
interface CallbackInterface {
    fun onSuccess(result: ResultData)
    fun onProgress(progress: Int, currentData: UserInfo)
    fun onError(errorCode: Int, errorMessage: String, errorData: ResultData?)
}