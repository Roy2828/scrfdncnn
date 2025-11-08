package com.roy.jni_demo


import android.annotation.SuppressLint
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.roy.jni_demo.databinding.ActivityMainBinding
import com.tencent.scrfdncnn.AssetCacheHelper
import com.tencent.scrfdncnn.SCRFDNcnn
import java.io.File


class MainActivity : AppCompatActivity()  {


    private lateinit var binding: ActivityMainBinding

    private val scrfdncnn: SCRFDNcnn = SCRFDNcnn()

    private  val embeddingPath = "embe.jpg" //
    private  val comparePath = "compare.jpg"

    private  val detectBin = "detect.bin"
    private  val detectParam = "detect.param"
    private  val featureBin = "feature.bin"
    private  val featureParam = "feature.param"

    var embeddingFloat: Array<FloatArray?>? = null


    @SuppressLint("MissingInflatedId")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)


        binding.tvInit.setOnClickListener {
            val detectBinPath =
                AssetCacheHelper.copyImageFromAssetsToCache(this@MainActivity, detectBin)
            val detectParamPath =
                AssetCacheHelper.copyImageFromAssetsToCache(this@MainActivity, detectParam)
            val featureBinPath =
                AssetCacheHelper.copyImageFromAssetsToCache(this@MainActivity, featureBin)
            val featureParamPath =
                AssetCacheHelper.copyImageFromAssetsToCache(this@MainActivity, featureParam)
            scrfdncnn.init(detectParamPath, detectBinPath, featureParamPath, featureBinPath)
        }

        binding.tvEmbed.setOnClickListener {
            val cacheDir = getCacheDir() // 内部缓存目录
            val targetDir = File(cacheDir, embeddingPath)


            embeddingFloat = scrfdncnn.getEmbeddingFromImage(targetDir.getAbsolutePath())
            Log.e("aa", "" + embeddingFloat)
        }

        binding.tvCompare.setOnClickListener {
            val cacheDir = getCacheDir()
            val compareStrPath = File(cacheDir, comparePath)
            val compare = scrfdncnn.compareEmbeddingWithImage(
                embeddingFloat,
                compareStrPath.getAbsolutePath()
            )
            Log.e("向量embedding", "" + compare)
        }

        binding.tvAssets.setOnClickListener {
            copyImageFromAssetsToCache(embeddingPath);
            copyImageFromAssetsToCache(comparePath);
        }


        val iv = binding.iv
        val cacheDir = getCacheDir() // 内部缓存目录
        val targetDir = File(cacheDir, embeddingPath)
        val bitmap = BitmapFactory.decodeFile(targetDir.getPath())
        iv.setImageBitmap(bitmap)

    }



    fun copyImageFromAssetsToCache(path: String?) {
        val cacheFilePath = AssetCacheHelper.copyImageFromAssetsToCache(this@MainActivity, path)

        if (cacheFilePath != null) {
            // 从缓存路径加载Bitmap并设置到ImageView
            val cachedBitmap = AssetCacheHelper.loadBitmapFromCache(cacheFilePath)
            binding.iv.setImageBitmap(cachedBitmap)
        } else {
            // 处理拷贝失败的情况
        }
    }





}



