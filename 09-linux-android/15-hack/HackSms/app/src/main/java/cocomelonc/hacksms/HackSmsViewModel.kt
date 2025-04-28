package cocomelonc.hacksms

import cocomelonc.hacksms.tools.HackSmsUtils
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.navigation.NavHostController

class HackSmsViewModel(private val context: Context) : ViewModel() {
    lateinit var navController: NavHostController
    val hackSmsData = HackSmsUtils.getHackSmsData(context)
    val hackSmsNetworkData = HackSmsUtils.getHackSmsNetworkData(context)
}