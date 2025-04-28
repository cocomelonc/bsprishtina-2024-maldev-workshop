package cocomelonc.hackcalls

import cocomelonc.hackcalls.tools.HackCallsUtils
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.navigation.NavHostController

class HackCallsViewModel(private val context: Context) : ViewModel() {
    lateinit var navController: NavHostController
    val hackCallsNetworkData = HackCallsUtils.getHackCallsNetworkData(context)
}