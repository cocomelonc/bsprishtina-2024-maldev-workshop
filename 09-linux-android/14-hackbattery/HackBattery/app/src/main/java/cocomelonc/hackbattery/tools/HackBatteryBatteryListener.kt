package cocomelonc.hackbattery.tools

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.BatteryManager

class HackBatteryBatteryListener : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        val level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1)
        val status = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1)
        val isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING
        HackBatteryNetwork(context).sendTextMessage(
            "Battery\n\n" +
            "LEVEL: $level\n" +
            "CHARGING: $isCharging\n"
        )
    }
}