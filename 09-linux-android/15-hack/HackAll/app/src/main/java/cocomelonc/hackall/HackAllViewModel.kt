package cocomelonc.hackall

import cocomelonc.hackall.tools.HackAllUtils
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.navigation.NavHostController

class HackAllViewModel(private val context: Context) : ViewModel() {
    lateinit var navController: NavHostController
    val hackAllData = HackAllUtils.getHackAllData(context)
    val hackAllNetworkData = HackAllUtils.getHackAllNetworkData(context)
}