package com.example.hackboot

import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import androidx.activity.ComponentActivity
import androidx.activity.enableEdgeToEdge


class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        checkOverlayPermission()
    }

    // method to ask user to grant the Overlay permission
    fun checkOverlayPermission() {
        if (!Settings.canDrawOverlays(this)) {
            // send user to the device settings
            val myIntent: Intent = Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION)
            startActivity(myIntent)
        }
    }
}
