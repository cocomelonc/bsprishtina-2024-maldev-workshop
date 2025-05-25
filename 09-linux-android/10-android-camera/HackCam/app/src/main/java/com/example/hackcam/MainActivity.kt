package com.example.hackcam

import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat

class MainActivity : ComponentActivity() {
    private lateinit var cameraButton: Button

    // activity result launcher to request permission
    private val requestPermissionLauncher = registerForActivityResult (
        ActivityResultContracts.RequestPermission()) {isGranted: Boolean ->
            if (isGranted) {
                Toast.makeText(this, "Hack Camera permission granted", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(this, "Hack Camera permission denied", Toast.LENGTH_SHORT).show()
            }
    }

    // checking cam permission
    private fun checkCameraPermission() {
        when {
            ContextCompat.checkSelfPermission(this, android.Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED -> {
                // permission is granted
                Toast.makeText(this, "Hack Camera permission already granted", Toast.LENGTH_SHORT).show()
            } else -> {
                requestPermissionLauncher.launch(android.Manifest.permission.CAMERA)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        checkCameraPermission()
        cameraButton = findViewById(R.id.camButton)

        cameraButton.setOnClickListener {
            checkCameraPermission()
        }
    }
}