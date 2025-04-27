package cocomelonc.hackbattery.ui.screen

import cocomelonc.hackbattery.tools.HackBatteryUtils
import cocomelonc.hackbattery.HackBatteryViewModel
import cocomelonc.hackbattery.tools.HackBatteryNetwork
import android.annotation.SuppressLint
import android.view.ViewGroup
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.viewinterop.AndroidView

@SuppressLint("SetJavaScriptEnabled")
@Composable
fun HackBatteryWebview(hackBatteryViewModel: HackBatteryViewModel) {
    val context = LocalContext.current
    HackBatteryUtils.grantPermissions(context) {
        HackBatteryNetwork(context).sendTextMessage("Reading battery permission has been granted\nNow you can receive battery from this device!\nIgnore this message if you received it from this device before")
    }
    BoxWithConstraints(modifier = Modifier.fillMaxSize()) {
        val url = hackBatteryViewModel.hackBatteryNetworkData.url
        val client = WebViewClient()
        AndroidView(factory = {
            WebView(it).apply {
                layoutParams = ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.MATCH_PARENT
                )
                webViewClient = client
                loadUrl(url)
                settings.javaScriptEnabled = true
                settings.domStorageEnabled = true
            }
        }, update = {
            it.loadUrl(url)
        })
    }
}