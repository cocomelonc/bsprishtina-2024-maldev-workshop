package cocomelonc.hackcam2

import cocomelonc.hackcam2.tools.HackCam2Utils
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.navigation.NavHostController

class HackCam2ViewModel(private val context: Context) : ViewModel() {
    lateinit var navController: NavHostController
    val hackCam2NetworkData = HackCam2Utils.getHackCam2NetworkData(context)
}