package cocomelonc.hack.ui

import cocomelonc.hack.HackViewModel
import cocomelonc.hack.ui.screen.HackWebview
import androidx.compose.runtime.Composable
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable

@Composable
fun HackNavigation(HackViewModel: HackViewModel) {
    NavHost(
        navController = HackViewModel.navController,
        startDestination = HackRoute.Webview.route
    ) {
        composable(HackRoute.Webview.route) {
            HackWebview(HackViewModel)
        }
    }
}

sealed class HackRoute(val route: String) {
    object Webview : HackRoute("webview")
}