package com.example.hackbroadcast

import android.annotation.SuppressLint
import android.content.Intent
import android.content.IntentFilter
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity

class MainActivity : ComponentActivity() {
    private lateinit var meowButton: Button
    lateinit var receiver: AirPlaneBroadcastReceiver

    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        meowButton = findViewById(R.id.meowButton)
        meowButton.setOnClickListener {
            Toast.makeText(this, "Meow! ♥️\uFE0F", Toast.LENGTH_SHORT).show()
        }

        receiver = AirPlaneBroadcastReceiver()
        IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED).also {
            registerReceiver(receiver, it)
        }
    }

    override fun onStop() {
        super.onStop()
        unregisterReceiver(receiver)
    }
}