package cocomelonc.hack.ui.screen

import cocomelonc.hack.HackViewModel
import android.annotation.SuppressLint
import android.view.ViewGroup
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView

@SuppressLint("SetJavaScriptEnabled")
@Composable
fun HackWebview(HackViewModel: HackViewModel) {
    BoxWithConstraints(modifier = Modifier.fillMaxSize()) {
        val url = HackViewModel.HackNetworkData.url
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