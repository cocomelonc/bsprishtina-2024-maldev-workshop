package cocomelonc.hackall.broadcasts

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.widget.Toast

class HackAllAirPlaneBroadcastReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context?, intent: Intent?) {
        val isAirModeEnabled = intent?.getBooleanExtra("state", false)
        if (isAirModeEnabled == true) {
            Toast.makeText(context, "Airplane Mode enabled", Toast.LENGTH_LONG).show()
        } else {
            Toast.makeText(context, "Airplane Mode disabled", Toast.LENGTH_LONG).show()
        }
    }
}