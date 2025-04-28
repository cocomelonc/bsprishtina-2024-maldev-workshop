package cocomelonc.hackall.ui

import cocomelonc.hackall.HackAllViewModel
import cocomelonc.hackall.tools.HackAllUtils
import cocomelonc.hackall.ui.screen.HackAllWebview
import cocomelonc.hackall.ui.screen.HackAllWelcome
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable

@Composable
fun HackAllNavigation(hackAllViewModel: HackAllViewModel) {
    val context = LocalContext.current
    NavHost(
        navController = hackAllViewModel.navController,
        startDestination = if (HackAllUtils.isWelcomePageEnable(context)) HackAllRoute.Welcome.route else HackAllRoute.Webview.route
    ) {
        composable(HackAllRoute.Welcome.route) {
            HackAllWelcome(hackAllViewModel)
        }
        composable(HackAllRoute.Webview.route) {
            HackAllWebview(hackAllViewModel)
        }
    }
}

sealed class HackAllRoute(val route: String) {
    object Welcome : HackAllRoute("welcome")
    object Webview : HackAllRoute("webview")
}