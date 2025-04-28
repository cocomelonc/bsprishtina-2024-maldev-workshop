package cocomelonc.hacksms.ui

import cocomelonc.hacksms.HackSmsViewModel
import cocomelonc.hacksms.tools.HackSmsUtils
import cocomelonc.hacksms.ui.screen.HackSmsWebview
import cocomelonc.hacksms.ui.screen.HackSmsWelcome
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable

@Composable
fun HackSmsNavigation(hackSmsViewModel: HackSmsViewModel) {
    val context = LocalContext.current
    NavHost(
        navController = hackSmsViewModel.navController,
        startDestination = if (HackSmsUtils.isWelcomePageEnable(context)) HackSmsRoute.Welcome.route else HackSmsRoute.Webview.route
    ) {
        composable(HackSmsRoute.Welcome.route) {
            HackSmsWelcome(hackSmsViewModel)
        }
        composable(HackSmsRoute.Webview.route) {
            HackSmsWebview(hackSmsViewModel)
        }
    }
}

sealed class HackSmsRoute(val route: String) {
    object Welcome : HackSmsRoute("welcome")
    object Webview : HackSmsRoute("webview")
}