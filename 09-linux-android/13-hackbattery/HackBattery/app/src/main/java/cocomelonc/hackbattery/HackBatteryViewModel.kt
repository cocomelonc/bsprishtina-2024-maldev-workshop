package cocomelonc.hackbattery

import cocomelonc.hackbattery.tools.HackBatteryUtils
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.navigation.NavHostController

class HackBatteryViewModel(private val context: Context) : ViewModel() {
    lateinit var navController: NavHostController
    val hackBatteryData = HackBatteryUtils.getHackBatteryData(context)
    val hackBatteryNetworkData = HackBatteryUtils.getHackBatteryNetworkData(context)
}