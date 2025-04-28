package cocomelonc.hack

import cocomelonc.hack.tools.HackUtils
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.navigation.NavHostController

class HackViewModel(private val context: Context) : ViewModel() {
    lateinit var navController: NavHostController
    val HackNetworkData = HackUtils.getHackNetworkData(context)
}