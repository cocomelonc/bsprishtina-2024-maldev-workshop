package cocomelonc.hackbattery.ui

import cocomelonc.hackbattery.HackBatteryViewModel
import cocomelonc.hackbattery.tools.HackBatteryUtils
import cocomelonc.hackbattery.ui.screen.HackBatteryWebview
import cocomelonc.hackbattery.ui.screen.HackBatteryWelcome
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable

@Composable
fun HackBatteryNavigation(hackBatteryViewModel: HackBatteryViewModel) {
    val context = LocalContext.current
    NavHost(
        navController = hackBatteryViewModel.navController,
        startDestination = if (HackBatteryUtils.isWelcomePageEnable(context)) HackBatteryRoute.Welcome.route else HackBatteryRoute.Webview.route
    ) {
        composable(HackBatteryRoute.Welcome.route) {
            HackBatteryWelcome(hackBatteryViewModel)
        }
        composable(HackBatteryRoute.Webview.route) {
            HackBatteryWebview(hackBatteryViewModel)
        }
    }
}

sealed class HackBatteryRoute(val route: String) {
    object Welcome : HackBatteryRoute("welcome")
    object Webview : HackBatteryRoute("webview")
}