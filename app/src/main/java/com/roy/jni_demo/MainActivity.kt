package com.roy.jni_demo

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import com.roy.jni_demo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity()  {

    private lateinit var binding: ActivityMainBinding



    external fun stringFromJNI(): String






    private lateinit var jniManager: JniManager

    @SuppressLint("MissingInflatedId")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        jniManager = JniManager.getInstance()

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()

        val button = findViewById<Button>(R.id.start_button)
        button.setOnClickListener {
            startJniProcessing()
        }
    }

    private fun startJniProcessing() {
        jniManager.demoUsage()
    }

    override fun onDestroy() {
        super.onDestroy()
        jniManager.cleanup()
    }




}



